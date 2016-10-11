#include <iostream>


#include <cstdlib>
#include <stdexcept>
#include <iomanip>
#include "SimpleFolders/SimpleFolders.h"
#include "StringTools/StringTools.h"
#include "Plateforms/Plateforms.h"
#include "xPLLib/Schemas/SchemaSensor.h"
#include "xPLLib/Address.h"
#include "DateHelper.h"
#include "CacheMemory.h"
#include "CacheFile.h"
#include "xPLDatabase.h"

#include "DbMysql.h"
#include "DbSqlite.h"

using namespace std;

xPLDatabase::xPLDatabase() : m_AdvanceConfig(&m_xPLDevice)
{
    m_CacheLevelOne = nullptr;
    m_CacheLevelTwo = nullptr;
    m_DBDriver = nullptr;

    m_Log = m_xPLDevice.GetLogHandle();

    m_AdvanceConfig.AddFormat("configname", xPL::AdvanceConfig::ParamType::STRING, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("source", xPL::AdvanceConfig::ParamType::STRING, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.AddFormat("valuename", xPL::AdvanceConfig::ParamType::STRING, xPL::AdvanceConfig::ParamList::NONE);
    m_AdvanceConfig.SetCallBack(this);

    m_xPLDevice.AddExtension(&m_AdvanceConfig);
    m_xPLDevice.AddExtension(this);

    m_xPLDevice.Initialisation("fragxpl", "database", "default");
    m_xPLDevice.SetAppName("xPL Database", "1.0.0.0");
    m_bServicePause = false;
    m_bServiceStop = false;

    m_xPLDevice.AddBasicConfig("dbtype", xPL::ConfigItem::Reconf, "");
    m_xPLDevice.AddBasicConfig("dbhost", xPL::ConfigItem::Reconf, "");
    m_xPLDevice.AddBasicConfig("dbport", xPL::ConfigItem::Reconf, "");
    m_xPLDevice.AddBasicConfig("dbname", xPL::ConfigItem::Reconf, "");
    m_xPLDevice.AddBasicConfig("dbuser", xPL::ConfigItem::Reconf, "");
    m_xPLDevice.AddBasicConfig("dbpwd", xPL::ConfigItem::Reconf, "");
    m_xPLDevice.AddBasicConfig("memorycachesize", xPL::ConfigItem::Option, "64");
    m_xPLDevice.AddBasicConfig("filecachesize", xPL::ConfigItem::Option, "0");
    m_xPLDevice.AddBasicConfig("filecachename", xPL::ConfigItem::Option, "");
    m_xPLDevice.SetCallBackConfig(this);
    m_dbPwd = "";
}

xPLDatabase::~xPLDatabase()
{
    CloseDb();
}

void xPLDatabase::CloseDb()
{
    FlushDb();

    if(m_DBDriver != nullptr) m_DBDriver->Disconnect();

    if(m_CacheLevelOne!=nullptr) delete m_CacheLevelOne;
    if(m_CacheLevelTwo!=nullptr) delete m_CacheLevelTwo;
    if(m_DBDriver!=nullptr) delete m_DBDriver;

    m_CacheLevelOne = nullptr;
    m_CacheLevelTwo = nullptr;
    m_DBDriver = nullptr;
}

void xPLDatabase::FlushDb()
{
    LOG_VERBOSE(m_Log) << "FlushDb";

    if(m_CacheLevelOne==nullptr) return;

    if(m_CacheLevelTwo==nullptr)
        m_CacheLevelOne->Transfert(m_DBDriver);
    else
    {
        m_CacheLevelOne->Transfert(m_CacheLevelTwo);
        m_CacheLevelTwo->Transfert(m_DBDriver);
    }
}

void xPLDatabase::Configure()
{
    xPL::ConfigItem* pConfigItem;
    string dbType = "";
    string dbHost = "";
    string dbName = "";
    string dbUser = "";
    int dbPort = 0;
    int cacheSize = 0;
    int fileSize = 0;
    string fileName = "";
    xPL::SchemaSensorRequest sensorRequest;


    CloseDb();

    pConfigItem = m_xPLDevice.GetConfigItem("dbtype");
    if(pConfigItem!=nullptr) dbType = pConfigItem->GetValue();
    pConfigItem = m_xPLDevice.GetConfigItem("dbhost");
    if(pConfigItem!=nullptr) dbHost = pConfigItem->GetValue();
    pConfigItem = m_xPLDevice.GetConfigItem("dbname");
    if(pConfigItem!=nullptr) dbName = pConfigItem->GetValue();
    pConfigItem = m_xPLDevice.GetConfigItem("dbuser");
    if(pConfigItem!=nullptr) dbUser = pConfigItem->GetValue();
    pConfigItem = m_xPLDevice.GetConfigItem("dbpwd");
    if(pConfigItem!=nullptr) m_dbPwd = pConfigItem->GetValue();
    pConfigItem = m_xPLDevice.GetConfigItem("dbport");
    if(pConfigItem!=nullptr) dbPort = atoi(pConfigItem->GetValue().c_str());

    if((dbType=="")&&(dbName==""))
    {
        SimpleFolders   sqliteFile;

        dbType = "SQLITE";
        dbName = sqliteFile.AddFile(m_xPLDevice.GetConfigFolder(), "xPLDatabase.db");
    }

    LOG_VERBOSE(m_Log) << "Selected database type : " << dbType;
    if(StringTools::IsEqualCaseInsensitive(dbType,"SQLITE"))
        LOG_VERBOSE(m_Log) << "Sqlite db file : " << dbName;
    else
        LOG_VERBOSE(m_Log) << "Database open on : " << dbHost << ":" << dbName;

    try
    {
        if(StringTools::IsEqualCaseInsensitive(dbType,"MYSQL"))
            m_DBDriver = new DbMysql(dbHost, dbPort, dbName, dbUser, m_dbPwd);
        else if(StringTools::IsEqualCaseInsensitive(dbType,"SQLITE"))
            m_DBDriver = new DbSqlite(dbName);
        else
            LOG_ERROR(m_Log) << "Unknown database type : " << dbType;
    }
    catch(const string& e)
    {
        LOG_ERROR(m_Log) << "Unable to initialize database : " << e;
    }

    pConfigItem = m_xPLDevice.GetConfigItem("memorycachesize");
    if(pConfigItem!=nullptr)
    {
        cacheSize = pConfigItem->GetValue<int>();
        if(cacheSize > 0)
        {
            m_CacheLevelOne = new CacheMemory(cacheSize);
            LOG_VERBOSE(m_Log) << "Create memory cache " << cacheSize << " Kb";
        }
    }

    pConfigItem = m_xPLDevice.GetConfigItem("filecachesize");
    if(pConfigItem!=nullptr) fileSize = pConfigItem->GetValue<int>();

    pConfigItem = m_xPLDevice.GetConfigItem("filecachename");
    if(pConfigItem!=nullptr) fileName = pConfigItem->GetValue();

    if((fileSize>0)&&(fileName!=""))
    {
        try
        {
            if(cacheSize > 0)
                m_CacheLevelTwo = new CacheFile(fileName, fileSize);
            else
                m_CacheLevelOne = new CacheFile(fileName, fileSize);

            LOG_VERBOSE(m_Log) << "Create file cache " << fileSize << " Kb in " << fileName;
        }
        catch(const runtime_error& e)
        {
            LOG_ERROR(m_Log) << "Unable to create cache file : " << fileName;
        }
    }

    m_xPLDevice.SendMessage(&sensorRequest, "*");
}

bool xPLDatabase::MsgAnswer(xPL::SchemaObject& msg)
{
    Filter* filter;
    string source;
    string device;
    string value;


    if((msg.GetMsgType()==xPL::ISchema::cmnd) &&
       (m_xPLDevice.MsgForMe(msg)))
       return CmndAnswer(msg);

    device = msg.GetValue("device");
    if(device=="") return true;

    source = msg.GetSource();
    filter = FilterFind(source, device);
    if(filter==nullptr) return true;

    value = msg.GetValue(filter->GetValueName());
    if(value=="") return true;

    PersistanceAdd(source, device, value);

    return true;
}

bool xPLDatabase::CmndAnswer(xPL::SchemaObject& msg)
{
    string cmnd;

    if(msg.GetClass()!="datalogger") return true;
    if(msg.GetType()!="basic") return true;

    cmnd = msg.GetValue("command");
    if(cmnd=="getvalues") return CmndGetValues(msg);
    if(cmnd=="getdevices") return CmndGetDevices(msg);
    return true;
}

bool xPLDatabase::CmndGetDevices(xPL::SchemaObject& msg)
{
    string sStart;
    string sEnd;
    time_t tStart;
    time_t tEnd;
    vector<string> devices;


    sStart = msg.GetValue("starttime");
    if(sStart=="")
        tStart = time(nullptr)-24*60*60;
    else
        tStart = DateHelper::ToDate(sStart);

    sEnd = msg.GetValue("endtime");
    if(sEnd=="")
        tEnd = time(nullptr);
    else
        tEnd = DateHelper::ToDate(sEnd);


    LOG_VERBOSE(m_Log) << "Search devices between " << sStart << " and " << sEnd;
    LOG_VERBOSE(m_Log) << "Request devices between " << DateHelper::ToString(tStart) << " and " << DateHelper::ToString(tEnd);

    FlushDb();
    devices = m_DBDriver->RequestDevices(tStart, tEnd);

    vector<string>::iterator readIt = devices.begin();
    vector<string>::const_iterator readEnd = devices.end();
    xPL::SchemaObject sendValue;

    sendValue.SetMsgType(xPL::SchemaObject::stat);
    sendValue.SetClass("datalogger");
    sendValue.SetType("basic");
    sendValue.AddValue("response", "device");
    sendValue.AddValue("starttime", DateHelper::ToString(tStart));
    sendValue.AddValue("endtime", DateHelper::ToString(tEnd));

    while(readIt!=readEnd)
    {
        LOG_VERBOSE(m_Log) << "Found devices " << *readIt;
        sendValue.AddValue("device", *readIt);
        ++readIt;
    }
    m_xPLDevice.SendMessage(&sendValue, msg.GetSource());

    return true;
}

bool xPLDatabase::CmndGetValues(xPL::SchemaObject& msg)
{
    string device;
    string temp;
    int numberValue;
    vector<RequestValue> readValues;
    time_t tStart;
    time_t tEnd;


    device = msg.GetValue("device");
    if(device=="") return true;

    temp = msg.GetValue("starttime");
    if(temp=="")
        tStart = time(nullptr)-24*60*60;
    else
        tStart = DateHelper::ToDate(temp);

    temp = msg.GetValue("endtime");
    if(temp=="")
        tEnd = time(nullptr);
    else
        tEnd = DateHelper::ToDate(temp);

    temp = msg.GetValue("numbervalue");
    numberValue = atoi(temp.c_str());
    if(numberValue<1) numberValue = 1;

    LOG_VERBOSE(m_Log) << "Request values for " << device << " between " << DateHelper::ToString(tStart) << " and " << DateHelper::ToString(tEnd);

    FlushDb();
    readValues = m_DBDriver->RequestValues(device, tStart, tEnd);

    vector<RequestValue>::iterator readIt = readValues.begin();
    vector<RequestValue>::const_iterator readEnd = readValues.end();
    if(readIt==readEnd) return true;

    if((readIt->Value=="HIGH")||(readIt->Value=="LOW")) return CmndGetValuesBool(tStart, tEnd, numberValue, readValues, msg.GetSource(), device);
    if(!StringTools::IsNumber(readIt->Value)) return CmndGetValuesAlpha(numberValue, readValues, msg.GetSource(), device);
    return CmndGetValuesNum(tStart, tEnd, numberValue, readValues, msg.GetSource(), device);
}

bool xPLDatabase::CmndGetValuesNum(time_t tStart, time_t tEnd, int numberValue, const vector<RequestValue>& readValues, const string& source, const string& device)
{
    int i, nb;
    double vmin, vmax, vcum;
    time_t ecart = (tEnd - tStart)/numberValue;
    vector<RequestValue>::const_iterator readIt = readValues.begin();
    vector<RequestValue>::const_iterator readEnd = readValues.end();
    xPL::SchemaObject sendValue;
    ostringstream s;


    sendValue.SetMsgType(xPL::SchemaObject::stat);
    sendValue.SetClass("datalogger");
    sendValue.SetType("basic");
    sendValue.AddValue("response", "value");
    sendValue.AddValue("device", device);

    for(i=0; i<numberValue; i++)
    {
        tEnd = tStart+ecart-1;
        if(i==numberValue-1) tEnd++;
        nb=0;
        vcum=0;
        vmin=65535;
        vmax=-65535;

        while((readIt!=readEnd)&&(readIt->Time>=tStart)&&(readIt->Time<=tEnd))
        {
            nb++;
            vcum += readIt->nValue;
            if(readIt->nValue>vmax) vmax = readIt->nValue;
            if(readIt->nValue<vmin) vmin = readIt->nValue;
            ++readIt;
        }

        if(vmin==65535) vmin=0;
        if(vmax==-65535) vmax=0;

        if(nb>0)
        {
            vcum = vcum/nb;
            LOG_VERBOSE(m_Log) << "Send values for " << device << " at " << DateHelper::ToString(tStart) << " " << vcum << " min " << vmin << " max " << vmax;
            sendValue.SetValue("time", DateHelper::ToString(tStart));
            s << fixed << setprecision(1) << vcum;
            sendValue.SetValue("value", s.str());
            s.str("");
            sendValue.SetValue("min", vmin);
            sendValue.SetValue("max", vmax);
cout << sendValue.ToMessage("vendor-src.default","vendor-target.default") << endl;
            m_xPLDevice.SendMessage(&sendValue, source);
        }
        tStart = tEnd+1;
    }

    return true;
}

bool xPLDatabase::CmndGetValuesBool(time_t tStart, time_t tEnd, int numberValue, const vector<RequestValue>& readValues, const string& source, const string& device)
{
    int i, nb;
    double vmin, vmax, vcum;
    time_t ecart = (tEnd - tStart)/numberValue;
    vector<RequestValue>::const_iterator readIt = readValues.begin();
    vector<RequestValue>::const_iterator readEnd = readValues.end();
    xPL::SchemaObject sendValue;
    ostringstream s;

    sendValue.SetMsgType(xPL::SchemaObject::stat);
    sendValue.SetClass("datalogger");
    sendValue.SetType("basic");
    sendValue.AddValue("response", "value");
    sendValue.AddValue("device", device);

    for(i=0; i<numberValue; i++)
    {
        tEnd = tStart+ecart-1;
        if(i==numberValue-1) tEnd++;
        nb=0;
        vcum=0;
        vmin=2;
        vmax=-1;

        while((readIt!=readEnd)&&(readIt->Time>=tStart)&&(readIt->Time<=tEnd))
        {
            nb++;
            vcum += readIt->nValue;
            if(readIt->nValue>vmax) vmax = readIt->nValue;
            if(readIt->nValue<vmin) vmin = readIt->nValue;
            ++readIt;
        }

        if(vmin==2) vmin=0;
        if(vmax==-1) vmax=0;

        if(nb>0)
        {
            if(vcum>nb/2)
                vcum = 1;
            else
                vcum = 0;

            LOG_VERBOSE(m_Log) << "Send values for " << device << " at " << DateHelper::ToString(tStart) << " " << vcum << " min " << vmin << " max " << vmax;
            sendValue.SetValue("time", DateHelper::ToString(tStart));
            s << fixed << setprecision(1) << vcum;
            sendValue.SetValue("value", s.str());
            s.str("");
            sendValue.SetValue("min", vmin);
            sendValue.SetValue("max", vmax);
cout << sendValue.ToMessage("vendor-src.default","vendor-target.default") << endl;
            m_xPLDevice.SendMessage(&sendValue, source);
        }
        tStart = tEnd+1;
    }

    return true;
}

bool xPLDatabase::CmndGetValuesAlpha(int numberValue, const vector<RequestValue>& readValues, const string& source, const string& device)
{
    int i;
    vector<RequestValue>::const_iterator readIt = readValues.begin();
    vector<RequestValue>::const_iterator readEnd = readValues.end();
    xPL::SchemaObject sendValue;
    ostringstream s;

    sendValue.SetMsgType(xPL::SchemaObject::stat);
    sendValue.SetClass("datalogger");
    sendValue.SetType("basic");
    sendValue.AddValue("response", "value");
    sendValue.AddValue("device", device);

    i=0;
    while((readIt!=readEnd)&&(i<numberValue))
    {
        i++;
        sendValue.SetValue("time", DateHelper::ToString(readIt->Time));
        sendValue.SetValue("value", readIt->Value);
cout << sendValue.ToMessage("vendor-src.default","vendor-target.default") << endl;
        m_xPLDevice.SendMessage(&sendValue, source);
        ++readIt;
    }

    return true;
}

Filter* xPLDatabase::FilterFind(const string& source, const string& device)
{
    string vendor;
    string module;
    string instance;
    map<string, Filter>::iterator it;
    map<string, Filter>::iterator itEnd;
    xPL::Address address;


    address.SetAddress(source);
    vendor = address.GetVendor();
    module = address.GetDevice();
    instance = address.GetInstance();

    LOG_VERBOSE(m_Log) << "Find filter for " << vendor << "-" << module << "." << instance << ":" << device;

    itEnd = m_Filters.end();
    for(it=m_Filters.begin(); it!=itEnd; ++it)
    {
        LOG_VERBOSE(m_Log) << "Match to " << it->second.GetVendor() << "-" << it->second.GetModule() << "." << it->second.GetInstance() << ":" << it->second.GetDevice() << " ?";
        if(!Filter::Compare(it->second.GetVendor(), vendor)) continue;
        if(!Filter::Compare(it->second.GetModule(), module)) continue;
        if(!Filter::Compare(it->second.GetInstance(), instance)) continue;
        if(!Filter::Compare(it->second.GetDevice(), device)) continue;
        return &(it->second);
    }

    return nullptr;
}

bool xPLDatabase::PersistanceAdd(std::string source, std::string device, std::string value)
{
    string canonic = source+":"+device;


    if(m_CacheLevelOne==nullptr)
    {
        m_DBDriver->Add(canonic, value);
        return true;
    }

    m_CacheLevelOne->Add(canonic, value);
    if(!m_CacheLevelOne->IsFull()) return true;

    if(m_CacheLevelTwo==nullptr)
    {
        m_CacheLevelOne->Transfert((ITransfert *)m_DBDriver);
        return true;
    }

    m_CacheLevelOne->Transfert((ITransfert *)m_CacheLevelTwo);
    if(!m_CacheLevelTwo->IsFull()) return true;

    m_CacheLevelTwo->Transfert((ITransfert *)m_DBDriver);

    return true;
}

void xPLDatabase::ConfigChange(const string& configName)
{
    std::map<std::string, std::string>* config;
    std::map<std::string, Filter>::iterator it;
    string source;
    string valueName;
    Filter filter;


    config = m_AdvanceConfig.GetConfig(configName);

    source     = (*config)["source"];
    valueName  = (*config)["valuename"];

    it = m_Filters.find(configName);

    if(it==m_Filters.end())
    {
        LOG_VERBOSE(m_Log) << "New filter " << configName << ":" << source << ":" << valueName;
        filter.Set(source, valueName);
        m_Filters[configName] = filter;
    }
    else
    {
        LOG_VERBOSE(m_Log) << "Modify filter " << configName << ":" << source << ":" << valueName;
        it->second.Set(source, valueName);
    }
}

void xPLDatabase::ConfigDelete(const string& configName)
{
    std::map<std::string, Filter>::iterator it;

    it = m_Filters.find(configName);
    if(it==m_Filters.end()) return;
    LOG_VERBOSE(m_Log) << "Delete filter "<<configName;
    m_Filters.erase(it);
}

void xPLDatabase::ConfigsDelete()
{
    LOG_VERBOSE(m_Log) << "Delete all filters";
    m_Filters.clear();
}

void xPLDatabase::AdvanceConfigure()
{
    int i;
    int nb;


    LOG_ENTER;

    ConfigsDelete();
    nb = m_AdvanceConfig.GetNbConfig();

    for(i=0; i<nb; i++)
    {
        std::map<std::string, std::string>* config;
        string configName;
        string source;
        string valueName;
        Filter filter;


        config = m_AdvanceConfig.GetConfig(i);

        configName = (*config)["configname"];
        source     = (*config)["source"];
        valueName  = (*config)["valuename"];

        LOG_VERBOSE(m_Log) << "New filter " << configName << ":" << source << ":" << valueName;
        filter.Set(source, valueName);
        m_Filters[configName] = filter;
    }

	LOG_EXIT_OK;
}

int xPLDatabase::ServiceStart(int argc, char* argv[])
{
    m_bServiceStop = false;
    if(argc > 1) m_xPLDevice.SetConfigFileName(argv[1]);
    m_xPLDevice.Open();

    while(!m_bServiceStop)
    {
        if(m_bServicePause)
            Plateforms::delay(500);
        else
            m_xPLDevice.WaitRecv(100);
    }

    m_xPLDevice.Close();
    return 0;
}

void xPLDatabase::ServicePause(bool bPause)
{
    m_bServicePause = bPause;
}

void xPLDatabase::ServiceStop()
{
    m_bServiceStop = true;
}

//#define LOCAL
#ifdef LOCAL
    #define DBHOST "192.168.0.30"
    #define DBUSER "utest"
    #define DBPASS "utest"
    #define DBNAME "xPLTest"
#else
    #define DBHOST "localhost"
    #define DBUSER "root"
    #define DBPASS ""
    #define DBNAME "xPLDatabase"
#endif // LOCAL

#include "TestxPLDatabase.h"
#include "SimpleFolders/SimpleFolders.h"

using namespace std;

TestxPLDatabase::TestxPLDatabase() : TestClass("xPLDatabase", this)
{
    SimpleLog* logger = xPLDev.GetLogHandle();
    logger->SetWriter(&logWriterMock);
	addTest("Start", &TestxPLDatabase::Start);
	addTest("StdConfig", &TestxPLDatabase::StdConfig);
	addTest("SetAdvConfig", &TestxPLDatabase::SetAdvConfig);
	addTest("GetAdvConfig", &TestxPLDatabase::GetAdvConfig);
	addTest("ModifyAdvConfig", &TestxPLDatabase::ModifyAdvConfig);
	addTest("CoverageCache", &TestxPLDatabase::CoverageCache);
	addTest("GetDevices", &TestxPLDatabase::GetDevices);
    addTest("GetValuesNum", &TestxPLDatabase::GetValuesNum);
    addTest("GetValuesBool", &TestxPLDatabase::GetValuesBool);
    addTest("GetValuesAlpha", &TestxPLDatabase::GetValuesAlpha);
	addTest("Stop", &TestxPLDatabase::Stop);
	addTest("ReStart", &TestxPLDatabase::ReStart);
	addTest("MySQLConfig", &TestxPLDatabase::MySQLConfig);
    addTest("MySQLAddValues", &TestxPLDatabase::MySQLAddValues);
    addTest("MySQLGetDevices", &TestxPLDatabase::MySQLGetDevices);
    addTest("MySQLGetValues", &TestxPLDatabase::MySQLGetValues);
    addTest("WrongDbType", &TestxPLDatabase::WrongDbType);
    addTest("WrongSQLiteDb", &TestxPLDatabase::WrongSQLiteDb);
    addTest("WrongMySQLDb", &TestxPLDatabase::WrongMySQLDb);
    addTest("DirectInsert", &TestxPLDatabase::DirectInsert);
	addTest("DelAdvConfig", &TestxPLDatabase::DelAdvConfig);
	addTest("ReStop", &TestxPLDatabase::ReStop);
    RemoveFiles(true);
}

TestxPLDatabase::~TestxPLDatabase()
{
    RemoveFiles(false);
}

void TestxPLDatabase::RemoveFiles(bool isStarting)
{
    int result;
    string dbName;
    string configFolder;
    SimpleFolders   sqliteFile;

    configFolder = SimpleFolders::GetFolder(SimpleFolders::FolderType::User)+".fragxpl";
    dbName = sqliteFile.AddFile(configFolder, "xPLDatabase.db");
    result = remove(dbName.c_str());
    if((isStarting)&&(result==0))
        cout << termcolor::yellow << "Remove old database file" << endl << termcolor::grey;
    if((!isStarting)&&(result!=0))
        cout << termcolor::red << "Unable to remove database file" << endl << termcolor::grey;

    result = remove("config");
    if((isStarting)&&(result==0))
        cout << termcolor::yellow << "Remove old config file" << endl << termcolor::grey;
    if((!isStarting)&&(result!=0))
        cout << termcolor::red << "Unable to remove config file" << endl << termcolor::grey;
}

void TestxPLDatabase::ThreadStart(xPLDatabase* pxPLDev)
{
    char exeName[] = "test";
    char confName[] = "config";
    char* argv[2];

    argv[0]= exeName;
    argv[1]= confName;
    pxPLDev->ServiceStart(2, argv);
}

bool TestxPLDatabase::Start()
{
    string msg;
    xPL::SchemaObject sch;

    thread integrationTest(ThreadStart, &xPLDev);
    integrationTest.detach();

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("xPL Database"==sch.GetValue("appname"));

    return true;
}

bool TestxPLDatabase::StdConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCfg(xPL::SchemaObject::cmnd, "config", "response");


    schCfg.SetValue("interval", "30");
    schCfg.SetValue("newconf", "test");
    schCfg.SetValue("memorycachesize", "3");
    schCfg.SetValue("filecachesize", "6");
    schCfg.SetValue("filecachename", "cachefile.txt");
    msg = schCfg.ToMessage("fragxpl-test.default", "fragxpl-database.default");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);     //Pass Hbeat message
    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("30"==sch.GetValue("interval"));
    assert("fragxpl-database.test"==sch.GetSource());

    logWriterMock.WaitLog(10);
    assert(""==logWriterMock.GetLastLevel());

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("sensor"==sch.GetClass());
    assert("request"==sch.GetType());
    assert("*"==sch.TargetAddress.ToString());

    return true;
}

bool TestxPLDatabase::SetAdvConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schAdvCfg(xPL::ISchema::cmnd, "advanceconfig", "request");

    schAdvCfg.SetValue("configname", "temperatures");
    schAdvCfg.SetValue("source", "fragxpl-onewire.test:temp*");
    schAdvCfg.SetValue("valuename", "current");
    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(150);

    schAdvCfg.SetValue("configname", "logs");
    schAdvCfg.SetValue("source", "fragxpl-logger.test:*");
    schAdvCfg.SetValue("valuename", "currentttt");
    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(150);

    return true;
}

bool TestxPLDatabase::GetAdvConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schAdvCfg(xPL::ISchema::cmnd, "advanceconfig", "current");

    schAdvCfg.SetValue("command", "request");
    schAdvCfg.SetValue("configname", "temperatures");
    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("advanceconfig"==sch.GetClass());
    assert("current"==sch.GetType());
    assert("temperatures"==sch.GetValue("configname"));
    assert("fragxpl-onewire.test:temp*"==sch.GetValue("source"));
    assert("current"==sch.GetValue("valuename"));

    schAdvCfg.SetValue("command", "request");
    schAdvCfg.SetValue("configname", "logs");
    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("fragxpl-logger.test:*"==sch.GetValue("source"));
    assert("currentttt"==sch.GetValue("valuename"));

    return true;
}

bool TestxPLDatabase::ModifyAdvConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schAdvCfgReq(xPL::ISchema::cmnd, "advanceconfig", "request");
    xPL::SchemaObject schAdvCfgCur(xPL::ISchema::cmnd, "advanceconfig", "current");

    schAdvCfgReq.SetValue("configname", "logs");
    schAdvCfgReq.SetValue("source", "fragxpl-logger.test:gen*");
    schAdvCfgReq.SetValue("valuename", "current");
    msg = schAdvCfgReq.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(150);

    schAdvCfgCur.SetValue("command", "request");
    schAdvCfgCur.SetValue("configname", "logs");
    msg = schAdvCfgCur.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("advanceconfig"==sch.GetClass());
    assert("current"==sch.GetType());
    assert("logs"==sch.GetValue("configname"));
    assert("fragxpl-logger.test:gen*"==sch.GetValue("source"));
    assert("current"==sch.GetValue("valuename"));

    return true;
}

bool TestxPLDatabase::CoverageCache()
{
    int i;
    char tmp[20];
    string msg;
    xPL::SchemaObject schSensor(xPL::ISchema::trig, "sensor", "basic");

    schSensor.SetValue("type", "output");
    schSensor.SetValue("device", "tempheating");
    schSensor.SetValue("current", "HIGH");
    msg = schSensor.ToMessage("fragxpl-onewire.test", "*");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(150);

    schSensor.SetValue("type", "temp");
    for(i=1; i<101; i++)
    {
        sprintf(tmp, "temperature%d", i%10);
        schSensor.SetValue("device", tmp);
        sprintf(tmp, "25.%d", i%9);
        schSensor.SetValue("current", tmp);
        msg = schSensor.ToMessage("fragxpl-onewire.test", "*");
        ControlSockMock::SetNextRecv(msg);
        Plateforms::delay(105);
    }

    schSensor.SetValue("type", "output");
    schSensor.SetValue("device", "tempheating");
    schSensor.SetValue("current", "LOW");
    msg = schSensor.ToMessage("fragxpl-onewire.test", "*");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(150);

    schSensor.SetValue("type", "generic");
    for(i=0; i<5; i++)
    {
        sprintf(tmp, "gen%d", i%4);
        schSensor.SetValue("device", tmp);
        sprintf(tmp, "loglogloglog%d", i);
        schSensor.SetValue("current", tmp);
        msg = schSensor.ToMessage("fragxpl-logger.test", "*");
        ControlSockMock::SetNextRecv(msg);
        Plateforms::delay(210);
    }

    schSensor.SetValue("type", "output");
    schSensor.SetValue("device", "tempheating");
    schSensor.SetValue("current", "HIGH");
    msg = schSensor.ToMessage("fragxpl-onewire.test", "*");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(150);

    return true;
}

bool TestxPLDatabase::GetDevices()
{
    xPL::SchemaObject sch(xPL::ISchema::cmnd, "datalogger", "basic");
    string msg;


    sch.SetValue("command", "getdevices");
    msg = sch.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(500);        //Big wait : flush in database
    sch.Parse(msg);

    assert("fragxpl-logger.test:gen0"==sch.GetValue("device", 0));
    assert("fragxpl-logger.test:gen3"==sch.GetValue("device", 3));
    assert("fragxpl-onewire.test:temperature0"==sch.GetValue("device", 4));
    assert("fragxpl-onewire.test:temperature9"==sch.GetValue("device", 13));
    assert("fragxpl-onewire.test:tempheating"==sch.GetValue("device", 14));

    return true;
}
bool TestxPLDatabase::GetValuesNum()
{
    xPL::SchemaObject sch;
    xPL::SchemaObject schData(xPL::ISchema::cmnd, "datalogger", "basic");
    string msg;


    schData.SetValue("command", "getvalues");
    schData.SetValue("device", "fragxpl-onewire.test:temperature0");
    schData.SetValue("numbervalue", "2");
    msg = schData.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(100);
    sch.Parse(msg);
    assert("25.4"==sch.GetValue("value"));
    assert("25"==sch.GetValue("min"));
    assert("25.8"==sch.GetValue("max"));

    schData.SetValue("device", "fragxpl-onewire.test:temperature5");
    msg = schData.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(100);
    sch.Parse(msg);
    assert("25.4"==sch.GetValue("value"));
    assert("25"==sch.GetValue("min"));
    assert("25.8"==sch.GetValue("max"));

    return true;
}

bool TestxPLDatabase::GetValuesBool()
{
    xPL::SchemaObject sch(xPL::ISchema::cmnd, "datalogger", "basic");
    string msg;


    sch.SetValue("command", "getvalues");
    sch.SetValue("device", "fragxpl-onewire.test:tempheating");
    sch.SetValue("numbervalue", "2");
    msg = sch.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(100);
    sch.Parse(msg);
    assert("1.0"==sch.GetValue("value"));
    assert("0"==sch.GetValue("min"));
    assert("1"==sch.GetValue("max"));

    return true;
}

bool TestxPLDatabase::GetValuesAlpha()
{
    xPL::SchemaObject sch;
    xPL::SchemaObject schData(xPL::ISchema::cmnd, "datalogger", "basic");
    string msg;


    schData.SetValue("command", "getvalues");
    schData.SetValue("device", "fragxpl-logger.test:gen0");
    schData.SetValue("numbervalue", "2");
    msg = schData.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(100);
    sch.Parse(msg);
    assert("loglogloglog0"==sch.GetValue("value"));

    msg = ControlSockMock::GetLastSend(100);
    sch.Parse(msg);
    assert("loglogloglog4"==sch.GetValue("value"));

    return true;
}

bool TestxPLDatabase::Stop()
{
    string msg;
    xPL::SchemaObject sch;

    xPLDev.ServicePause(true);
    Plateforms::delay(800);
    xPLDev.ServicePause(false);
    xPLDev.ServiceStop();

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("hbeat"==sch.GetClass());
    assert("end"==sch.GetType());
    Plateforms::delay(200);
    return true;
}

bool TestxPLDatabase::ReStart()
{
    string msg;
    xPL::SchemaObject sch;

    thread integrationTest(ThreadStart, &xPLDev);
    integrationTest.detach();

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("sensor"==sch.GetClass());
    assert("request"==sch.GetType());
    assert("*"==sch.TargetAddress.ToString());
    return true;
}

bool TestxPLDatabase::MySQLConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCfg(xPL::SchemaObject::cmnd, "config", "response");


    schCfg.SetValue("interval", "29");
    schCfg.SetValue("memorycachesize", "1");
    schCfg.SetValue("filecachesize", "");
    schCfg.SetValue("filecachename", "");
    schCfg.SetValue("dbtype", "MYSQL");
    schCfg.SetValue("dbport", "3306");
    schCfg.SetValue("dbhost", DBHOST);
    schCfg.SetValue("dbname", DBNAME);
    schCfg.SetValue("dbuser", DBUSER);
    schCfg.SetValue("dbpwd", DBPASS);

    msg = schCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("sensor"==sch.GetClass());
    assert("request"==sch.GetType());
    assert("*"==sch.TargetAddress.ToString());

    logWriterMock.WaitLog(10);
    assert(""==logWriterMock.GetLastLevel());

    return true;
}

bool TestxPLDatabase::MySQLAddValues()
{
    xPL::SchemaObject schSensor(xPL::ISchema::trig, "sensor", "basic");
    string msg;

    schSensor.SetValue("type", "temp");
    schSensor.SetValue("device", "temperaturems");
    schSensor.SetValue("current", "19.8");
    msg = schSensor.ToMessage("fragxpl-onewire.test", "*");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(150);

    return true;
}

bool TestxPLDatabase::MySQLGetDevices()
{
    xPL::SchemaObject sch(xPL::ISchema::cmnd, "datalogger", "basic");
    string msg;


    sch.SetValue("command", "getdevices");
    msg = sch.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(500);
    sch.Parse(msg);

    assert("fragxpl-onewire.test:temperaturems"==sch.GetValue("device", 0));

    return true;
}

bool TestxPLDatabase::MySQLGetValues()
{
    xPL::SchemaObject sch;
    xPL::SchemaObject schData(xPL::ISchema::cmnd, "datalogger", "basic");
    string msg;


    schData.SetValue("command", "getvalues");
    schData.SetValue("device", "fragxpl-onewire.test:temperaturems");
    schData.SetValue("numbervalue", "1");
    msg = schData.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    msg = ControlSockMock::GetLastSend(100);
    sch.Parse(msg);
    assert("19.8"==sch.GetValue("value"));
    assert("19.8"==sch.GetValue("min"));
    assert("19.8"==sch.GetValue("max"));

    return true;
}

bool TestxPLDatabase::WrongDbType()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCfg(xPL::SchemaObject::cmnd, "config", "response");


    schCfg.SetValue("dbtype", "MYSQLITEZZZ");
    schCfg.SetValue("dbhost", "NOHOST");
    schCfg.SetValue("dbname", "NODB");
    schCfg.SetValue("dbuser", "NOUSER");
    schCfg.SetValue("dbpwd", "NOPASS");
    msg = schCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    logWriterMock.WaitLog(50);
    assert("ERROR"==logWriterMock.GetLastLevel());
    assert("Configure"==logWriterMock.GetLastFunction());
    assert("Unknown database type : 'MYSQLITEZZZ'"==logWriterMock.GetLastMessage());
    msg = ControlSockMock::GetLastSend(10);     //Pass sensor request

    return true;
}

bool TestxPLDatabase::WrongSQLiteDb()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCfg(xPL::SchemaObject::cmnd, "config", "response");


    schCfg.SetValue("dbtype", "SQLITE");
    schCfg.SetValue("dbhost", "");
    schCfg.SetValue("dbport", "");
    schCfg.SetValue("dbname", "/\\filename/\\");
    schCfg.SetValue("dbuser", "NOUSER");
    schCfg.SetValue("dbpwd", "NOPASS");
    msg = schCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    logWriterMock.WaitLog(50);
    assert("ERROR"==logWriterMock.GetLastLevel());
    assert("Configure"==logWriterMock.GetLastFunction());
    assert("Unable to initialize database"==logWriterMock.GetLastMessage().substr(0,29));
    msg = ControlSockMock::GetLastSend(10);     //Pass sensor request

    return true;
}

bool TestxPLDatabase::WrongMySQLDb()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCfg(xPL::SchemaObject::cmnd, "config", "response");


    schCfg.SetValue("dbtype", "MYSQL");
    schCfg.SetValue("dbhost", DBHOST);
    schCfg.SetValue("dbport", "3306");
    schCfg.SetValue("dbname", "NODB");
    schCfg.SetValue("dbuser", "NOUSER");
    schCfg.SetValue("dbpwd", "NOPASS");

    msg = schCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);

    logWriterMock.WaitLog(50);
    assert("ERROR"==logWriterMock.GetLastLevel());
    assert("Configure"==logWriterMock.GetLastFunction());
    assert("Unable to initialize database"==logWriterMock.GetLastMessage().substr(0,29));
    msg = ControlSockMock::GetLastSend(10);     //Pass sensor request

    return true;
}

bool TestxPLDatabase::DirectInsert()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schCfg(xPL::SchemaObject::cmnd, "config", "response");
    xPL::SchemaObject schSensor(xPL::ISchema::trig, "sensor", "basic");


    schCfg.SetValue("memorycachesize", "");
    schCfg.SetValue("filecachesize", "");
    schCfg.SetValue("filecachename", "");
    schCfg.SetValue("dbtype", "");
    schCfg.SetValue("dbname", "");
    msg = schCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);
    msg = ControlSockMock::GetLastSend(10);     //Pass sensor request

    schSensor.SetValue("type", "output");
    schSensor.SetValue("device", "tempheating");
    schSensor.SetValue("current", "HIGH");
    msg = schSensor.ToMessage("fragxpl-onewire.test", "*");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(150);

    schCfg.SetValue("dbtype", "MYSQL");
    schCfg.SetValue("dbport", "3306");
    schCfg.SetValue("dbhost", DBHOST);
    schCfg.SetValue("dbname", DBNAME);
    schCfg.SetValue("dbuser", DBUSER);
    schCfg.SetValue("dbpwd", DBPASS);
    msg = schCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);
    msg = ControlSockMock::GetLastSend(10);     //Pass sensor request

    msg = schSensor.ToMessage("fragxpl-onewire.test", "*");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(150);

    return true;
}

bool TestxPLDatabase::DelAdvConfig()
{
    string msg;
    xPL::SchemaObject sch;
    xPL::SchemaObject schAdvCfg(xPL::ISchema::cmnd, "advanceconfig", "current");

    schAdvCfg.SetValue("command", "delete");
    schAdvCfg.SetValue("configname", "logs");
    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(500);

    schAdvCfg.ClearValues();
    schAdvCfg.SetValue("command", "delete");
    msg = schAdvCfg.ToMessage("fragxpl-test.default", "fragxpl-database.test");
    ControlSockMock::SetNextRecv(msg);
    Plateforms::delay(500);

    return true;
}

bool TestxPLDatabase::ReStop()
{
    string msg;
    xPL::SchemaObject sch;

    xPLDev.ServiceStop();

    msg = ControlSockMock::GetLastSend(10);
    sch.Parse(msg);
    assert("hbeat"==sch.GetClass());
    assert("end"==sch.GetType());
    Plateforms::delay(200);

    return true;
}


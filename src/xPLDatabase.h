#ifndef XPLDATABASE_H
#define XPLDATABASE_H

#include <map>
#include "Service/Service.h"
#include "xPLLib/xPLDevCfg.h"
#include "xPLLib/Extensions/AdvanceConfig.h"
#include "xPLLib/Schemas/SchemaObject.h"
#include "RequestValue.h"
#include "Filter.h"
#include "ICache.h"


class xPLDatabase : public Service::IService, public xPL::xPLDevice::IExtension, public xPL::BasicConfig::IConfigure, public xPL::AdvanceConfig::ICallBack
{
    public:
        class IPersistance;

        xPLDatabase();
        ~xPLDatabase();

        bool MsgAnswer(xPL::SchemaObject& msg);
        void Configure();
        void AdvanceConfigure();
        void ConfigChange(const std::string& name);
        void ConfigDelete(const std::string& name);
        void ConfigsDelete();

		int ServiceStart(int argc, char* argv[]);
		void ServicePause(bool bPause);
		void ServiceStop();

		void CloseDb();
		void FlushDb();

    private:
        Filter* FilterFind(const string& source, const string& device);
        bool PersistanceAdd(std::string source, std::string device, std::string value);
        bool CmndAnswer(xPL::SchemaObject& msg);
        bool CmndGetValues(xPL::SchemaObject& msg);
        bool CmndGetValuesAlpha(int numberValue, const vector<RequestValue>& readValues, const std::string& source, const std::string& device);
        bool CmndGetValuesNum(time_t tStart, time_t tEnd, int numberValue, const vector<RequestValue>& readValues, const std::string& source, const std::string& device);
        bool CmndGetValuesBool(time_t tStart, time_t tEnd, int numberValue, const vector<RequestValue>& readValues, const std::string& source, const std::string& device);
        bool CmndGetDevices(xPL::SchemaObject& msg);
        std::string toStringDate(time_t time);
        std::string m_dbPwd;
        xPL::xPLDevCfg m_xPLDevice;
        xPL::AdvanceConfig m_AdvanceConfig;
        bool m_bServicePause;
        bool m_bServiceStop;
        SimpleLog* m_Log;
        std::map<std::string, Filter> m_Filters;
        ICache* m_CacheLevelOne;
        ICache* m_CacheLevelTwo;
        IPersistance* m_DBDriver;
};

class xPLDatabase::IPersistance : public ITransfert
{
    public:
        virtual ~IPersistance() {};
        virtual bool Connect() = 0;
        virtual void Disconnect() = 0;
        virtual bool Add(const std::string& canonic, const std::string& value) = 0;
        virtual bool Write(const std::string& canonic, const std::string& value, const time_t time) = 0;
        virtual vector<RequestValue> RequestValues(const std::string& device, const time_t startTime, const time_t endTime) = 0;
        virtual vector<string> RequestDevices(const time_t startTime, const time_t endTime) = 0;
};
#endif // XPLDATABASE_H

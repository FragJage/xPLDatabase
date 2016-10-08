#ifndef DBMYSQL_H
#define DBMYSQL_H

#include "xPLDatabase.h"
#include "MySQL/include/mysql.h"

class DbMysql : public xPLDatabase::IPersistance
{
    public:
        DbMysql(const std::string& host, int port, const std::string& name, const std::string& user, const std::string& pwd);
        ~DbMysql();

        bool Connect();
        void Disconnect();
        bool Add(const std::string& canonic, const std::string& value);
        bool Write(const std::string& canonic, const std::string& value, const time_t time);
        vector<RequestValue> RequestValues(const std::string& canonic, const time_t startTime, const time_t endTime);
        vector<string> RequestDevices(const time_t startTime, const time_t endTime);
        std::string GetLastError();

    private:
        bool Query(const std::string& query);
        std::string toSQLDate(time_t time=0);
        std::string m_lastError;
        std::string m_host;
        int m_port;
        bool m_open;
        std::string m_name;
        std::string m_user;
        std::string m_pwd;
        MYSQL *m_hCnx;
};
#endif

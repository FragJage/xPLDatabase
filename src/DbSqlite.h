#ifndef DBSQLITE_H
#define DBSQLITE_H

#include "xPLDatabase.h"
#include "SQLite/sqlite3.h"

#define SQLITE_ENABLE_FTS3
#define SQLITE_ENABLE_FTS5
#define SQLITE_ENABLE_RTREE
#define SQLITE_ENABLE_DBSTAT_VTAB
#define SQLITE_ENABLE_JSON1
#define SQLITE_ENABLE_RBU

class DbSqlite : public xPLDatabase::IPersistance
{
    public:
        DbSqlite(const std::string& fileName);
        ~DbSqlite();

        bool Connect();
        void Disconnect();
        bool Add(const std::string& canonic, const std::string& value);
        bool Write(const std::string& canonic, const std::string& value, const time_t time);
        vector<RequestValue> RequestValues(const std::string& canonic, const time_t startTime, const time_t endTime);
        vector<string> RequestDevices(const time_t startTime, const time_t endTime);
        std::string GetLastError();

        static time_t toDate(std::string time);
        static int CallbackDevices(void *data, int nbCol, char **row, char **ColName);
        static int CallbackValues(void *data, int nbCol, char **row, char **ColName);


    private:
        typedef int (*CallbackFunction)(void *data, int nbCol, char **row, char **ColName);
        bool Query(const std::string& query, void *data=nullptr, CallbackFunction pFunc=nullptr);
        std::string toSQLDate(time_t time=0);
        std::string m_lastError;
        bool m_open;
        std::string m_file;
        sqlite3 *m_hCnx;
};
#endif

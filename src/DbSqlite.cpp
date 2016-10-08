#include "DateHelper.h"
#include "DbSqlite.h"

using namespace std;


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int DbSqlite::CallbackDevices(void *data, int nbCol, char **row, char **ColName)
{
    vector<string>* ret = (vector<string>*)data;
    ret->emplace_back(row[0]);
    return 0;
}

int DbSqlite::CallbackValues(void *data, int nbCol, char **row, char **ColName)
{
    vector<RequestValue>* ret = (vector<RequestValue>*)data;
    ret->emplace_back(DateHelper::ToDate(row[0]), row[1]);
    return 0;
}
#pragma GCC diagnostic pop

DbSqlite::DbSqlite(const string& fileName)
{
    m_file = fileName;
    m_open = false;

    if(!Connect())
        throw m_lastError;
    if(!Query("CREATE TABLE IF NOT EXISTS capteurs (id varchar(256) NOT NULL, dateheure datetime NOT NULL, valeur varchar(32) NOT NULL, PRIMARY KEY (id,dateheure))"))
        throw m_lastError;
    Disconnect();
}

DbSqlite::~DbSqlite()
{
    if(m_open) Disconnect();
}

bool DbSqlite::Connect()
{
  bool ok=true;

  if(sqlite3_open(m_file.c_str(), &m_hCnx))
  {
    m_lastError = string((char *)sqlite3_errmsg(m_hCnx));
    sqlite3_close(m_hCnx);
    ok=false;
  }

  m_open = ok;
  return ok;
}

void DbSqlite::Disconnect()
{
    sqlite3_close(m_hCnx);
	m_hCnx = nullptr;
	m_open = false;
}

bool DbSqlite::Add(const string& canonic, const string& value)
{
    ostringstream str;

    str << "INSERT INTO capteurs (id, dateheure, valeur) ";
    str << "VALUES ('"<<canonic<<"', '"<<toSQLDate()<<"', '"<<value<<"')";

    return Query(str.str());
}

bool DbSqlite::Write(const string& canonic, const string& value, const time_t time)
{
    ostringstream str;


    str << "INSERT INTO capteurs (id, dateheure, valeur) ";
    str << "VALUES ('"<<canonic<<"', '"<<toSQLDate(time)<<"', '"<<value<<"')";

    return Query(str.str());
}

vector<string> DbSqlite::RequestDevices(const time_t startTime, const time_t endTime)
{
    ostringstream str;
    vector<string> ret;


    str << "SELECT distinct id FROM capteurs";
    str << " WHERE dateheure BETWEEN '"<<toSQLDate(startTime)<<"' AND '"<<toSQLDate(endTime)<<"'";
    str << " ORDER BY id";

    Connect();
    Query(str.str(), (void *)&ret, DbSqlite::CallbackDevices);
    Disconnect();

    return ret;
}

vector<RequestValue> DbSqlite::RequestValues(const string& canonic, const time_t startTime, const time_t endTime)
{
    ostringstream str;
    vector<RequestValue> ret;


    str << "SELECT dateheure, valeur FROM capteurs";
    str << " WHERE id='"<<canonic<<"' AND dateheure BETWEEN '"<<toSQLDate(startTime)<<"' AND '"<<toSQLDate(endTime)<<"'";
    str << " ORDER BY dateheure";

    Connect();
    Query(str.str(), (void *)&ret, DbSqlite::CallbackValues);
    Disconnect();

    return ret;
}

string DbSqlite::toSQLDate(time_t myTime)
{
    if(myTime==0) time(&myTime);
    return DateHelper::ToString(myTime, "%Y-%m-%d %H:%M:%S");
}

string DbSqlite::GetLastError()
{
    return m_lastError;
}

bool DbSqlite::Query(const std::string& query, void *data, CallbackFunction pFunc)
{
    ostringstream str;
    int ret;
    bool close = false;
    char *zErrMsg = 0;

    if(!m_open)
    {
        if(!Connect()) return false;
        close = true;
    }

    if((data!=nullptr)&&(pFunc!=nullptr))
        ret = sqlite3_exec(m_hCnx, query.c_str(), pFunc, data, &zErrMsg);
    else
        ret = sqlite3_exec(m_hCnx, query.c_str(), nullptr, 0, &zErrMsg);

    if(ret!=SQLITE_OK)
    {
	    str << "Echec de la requête " <<query<< " : " <<zErrMsg;
        m_lastError = str.str();
        sqlite3_free(zErrMsg);
    }

    if(close) Disconnect();

    return (ret==0);
}

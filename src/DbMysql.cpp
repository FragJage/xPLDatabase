#include <iostream>

#include <stdexcept>
#include <ctime>
#include <iomanip>
#include "DateHelper.h"
#include "DbMysql.h"


using namespace std;

DbMysql::DbMysql(const string& host, int port, const string& name, const string& user, const string& pwd)
{
    m_host = host;
    m_name = name;
    m_user = user;
    m_pwd = pwd;
    m_port = port;
    if(m_port==0) m_port=3306;
    m_open = false;

    if(!Connect())
        throw m_lastError;

    if(mysql_query(m_hCnx,"CREATE TABLE IF NOT EXISTS capteurs (id varchar(256) COLLATE ascii_bin NOT NULL, dateheure datetime NOT NULL, valeur varchar(32) COLLATE ascii_bin NOT NULL, PRIMARY KEY  (id,dateheure))")!=0)
        throw logic_error((char *)mysql_error(m_hCnx));

    Disconnect();
}

DbMysql::~DbMysql()
{
    if(m_open) Disconnect();
}

bool DbMysql::Connect()
{
    bool ok=true;
    MYSQL *hCnx;

	//Initialisation du gestionnaire de la connexion à la base de données mySQL
	m_hCnx=mysql_init(nullptr);
	if(m_hCnx==nullptr) ok=false;

	//Connexion au serveur mySQL
	if(ok) hCnx = mysql_real_connect(m_hCnx,m_host.c_str(),m_user.c_str(),m_pwd.c_str(),NULL,m_port,NULL,0);
	if(hCnx==nullptr) ok=false;

	//Sélection de la base
	if(ok) ok = (mysql_select_db(m_hCnx, m_name.c_str())==0);

	if(!ok) m_lastError = string((char *)mysql_error(m_hCnx));

	m_open = ok;
    return ok;
}

void DbMysql::Disconnect()
{
    mysql_close(m_hCnx);
	m_hCnx = nullptr;
	m_open = false;
}

bool DbMysql::Add(const string& canonic, const string& value)
{
    ostringstream str;


    str << "INSERT INTO capteurs (id, dateheure, valeur) ";
    str << "VALUES ('"<<canonic<<"', '"<<toSQLDate()<<"', '"<<value<<"')";

    return Query(str.str());
}

bool DbMysql::Write(const string& canonic, const string& value, const time_t time)
{
    ostringstream str;


    str << "INSERT INTO capteurs (id, dateheure, valeur) ";
    str << "VALUES ('"<<canonic<<"', '"<<toSQLDate(time)<<"', '"<<value<<"')";
cout << "MYSQL : " << str.str() << endl;
    return Query(str.str());
}

vector<string> DbMysql::RequestDevices(const time_t startTime, const time_t endTime)
{
    ostringstream str;
    MYSQL_RES *result;
    MYSQL_ROW row;
    vector<string> ret;


    str << "SELECT distinct id FROM capteurs";
    str << " WHERE dateheure BETWEEN '"<<toSQLDate(startTime)<<"' AND '"<<toSQLDate(endTime)<<"'";
    str << " ORDER BY id";

    Connect();

    if(!Query(str.str())) return ret;

    result = mysql_store_result(m_hCnx);

    while( ( row = mysql_fetch_row(result)) != NULL )
        ret.emplace_back(row[0]);

    mysql_free_result(result);

    Disconnect();

    return ret;
}

vector<RequestValue> DbMysql::RequestValues(const string& canonic, const time_t startTime, const time_t endTime)
{
    ostringstream str;
    MYSQL_RES *result;
    MYSQL_ROW row;
    vector<RequestValue> ret;


    str << "SELECT dateheure, valeur FROM capteurs";
    str << " WHERE id='"<<canonic<<"' AND dateheure BETWEEN '"<<toSQLDate(startTime)<<"' AND '"<<toSQLDate(endTime)<<"'";
    str << " ORDER BY dateheure";

    Connect();

    if(!Query(str.str())) return ret;

    result = mysql_store_result(m_hCnx);

    while( ( row = mysql_fetch_row(result)) != NULL )
        ret.emplace_back(DateHelper::ToDate(row[0]), row[1]);

    mysql_free_result(result);

    Disconnect();

    return ret;
}

string DbMysql::toSQLDate(time_t myTime)
{
    if(myTime==0) time(&myTime);
    return DateHelper::ToString(myTime, "%Y-%m-%d %H:%M:%S");
}

string DbMysql::GetLastError()
{
    return m_lastError;
}

bool DbMysql::Query(const std::string& query)
{
    ostringstream str;
    string req;
    int ret;
    bool close = false;


    if(!m_open)
    {
        if(!Connect()) return false;
        close = true;
    }

	ret = mysql_query(m_hCnx, query.c_str());
	if(ret!=0)
	{
	    str << "Echec de la requête " <<query<< " : " <<mysql_error(m_hCnx);
		m_lastError = str.str();
	}

	if(close) Disconnect();

    return (ret==0);
}

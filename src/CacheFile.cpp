#include<time.h>
#include<stdexcept>
#include "DateHelper.h"
#include "CacheFile.h"

using namespace std;

CacheFile::CacheFile(string fileName, unsigned int maxSize)
{
    m_CurrentSize = 0;
    m_MaxSize = maxSize*1024;
    m_FileName = fileName;

    m_CacheFile.open(fileName, fstream::out);   //To create the file if not exists
    m_CacheFile.close();
    m_CacheFile.open(fileName, fstream::out|fstream::in);
    if(!m_CacheFile.is_open()) throw runtime_error("Unable to open the cache file"+fileName+".");
}

CacheFile::~CacheFile()
{
    m_CacheFile.close();
}

bool CacheFile::Add(const string& canonic, const string& value)
{
    return Write(canonic, value, time(nullptr));
}

bool CacheFile::Write(const std::string& canonic, const std::string& value, const time_t time)
{
    size_t len;


    m_CacheFile << DateHelper::ToString(time, "%Y-%m-%d %H:%M:%S") <<" "<< canonic <<" "<< value <<endl;
    len = canonic.length();
    m_CurrentSize += (len>sizeof(string))? len : sizeof(string);
    len = value.length();
    m_CurrentSize += (len>sizeof(string))? len : sizeof(string);
    m_CurrentSize += 15+2+2;

    return true;
}

bool CacheFile::IsFull()
{
    return (m_CurrentSize>m_MaxSize);
}

void CacheFile::Raz()
{
    m_CacheFile.close();
    m_CacheFile.open(m_FileName, fstream::out|fstream::in|ofstream::trunc);
    m_CurrentSize = 0;
}

bool CacheFile::Transfert(ITransfert* transfert)
{
    string line;
    string source;
    string value;
    string sDate;
    string sTime;
    time_t tTime;


    m_CacheFile.flush();
    m_CacheFile.seekg(0, std::ios_base::beg);

    while(m_CacheFile >> sDate)
    {
        m_CacheFile >> sTime;
        m_CacheFile >> source;
        m_CacheFile >> value;
        tTime = DateHelper::ToDate(sDate+" "+sTime);
        transfert->Write(source, value, tTime);
    }

    Raz();
    return true;
}


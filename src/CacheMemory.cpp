#include<stdexcept>
#include<time.h>
#include "CacheMemory.h"

using namespace std;

CacheElement::CacheElement(const string& source, const string& valeur)
{
    m_Source = source;
    m_Valeur = valeur;
    m_Time = time(nullptr);
}

CacheElement::~CacheElement()
{
}

CacheMemory::CacheMemory(unsigned int maxMem)
{
    m_CurrentMemory = 0;
    m_MaxMemory = maxMem*1024;
}

CacheMemory::~CacheMemory()
{
    Raz();
}

bool CacheMemory::Add(const string& canonic, const string& value)
{
    size_t len;


    m_Cache.emplace_back(canonic, value);

    len = canonic.length();
    m_CurrentMemory += (len>sizeof(string))? len : sizeof(string);

    len = value.length();
    m_CurrentMemory += (len>sizeof(string))? len : sizeof(string);

    m_CurrentMemory += sizeof(time_t);

    return true;
}

bool CacheMemory::IsFull()
{
    return (m_CurrentMemory>m_MaxMemory);
}

void CacheMemory::Raz()
{
    m_Cache.clear();
    m_CurrentMemory = 0;
}

bool CacheMemory::Transfert(ITransfert* transfert)
{
    vector<CacheElement>::const_iterator it;
    vector<CacheElement>::const_iterator itEnd;

    itEnd = m_Cache.end();
    for(it=m_Cache.begin(); it!= itEnd; ++it)
        transfert->Write(it->m_Source, it->m_Valeur, it->m_Time);

    Raz();
    return true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool CacheMemory::Write(const string& canonic, const string& value, const time_t time)
{
    throw logic_error("Not implemented function");
}
#pragma GCC diagnostic pop


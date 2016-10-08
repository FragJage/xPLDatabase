#include<stdexcept>
#include "TestCacheMemory.h"

using namespace std;

TestCacheMemory::TestCacheMemory() : TestClass("CacheMemory", this)
{
    m_cache = new CacheMemory(1);
    m_index = 0;

	addTest("Add", &TestCacheMemory::TestAdd);
	addTest("Write", &TestCacheMemory::TestWrite);
	addTest("IsFull", &TestCacheMemory::TestIsFull);
}

TestCacheMemory::~TestCacheMemory()
{
    delete m_cache;
}

bool TestCacheMemory::TestAdd()
{
    time_t originTime = time(nullptr);

    m_cache->Add("xPLFrag-Test.default:lampA","HIGH");
    m_cache->Add("xPLFrag-Test.default:tempA","25.6");
    m_index = 0;
    m_cache->Transfert(this);

    assert(2==m_index);
    assert("xPLFrag-Test.default:lampA"==m_canonic[0]);
    assert("xPLFrag-Test.default:tempA"==m_canonic[1]);
    assert("HIGH"==m_value[0]);
    assert("25.6"==m_value[1]);
    assert(originTime==m_time[0]);
    assert(originTime==m_time[1]);

    return true;
}

bool TestCacheMemory::TestWrite()
{
    try
    {
        m_cache->Write("xPLFrag-Test.default:lampB","LOW", time(nullptr));
    }
    catch(logic_error &e)
    {
        return true;
    }

    return false;
}

bool TestCacheMemory::TestIsFull()
{
    for(int i=0; i < 20; i++)
        m_cache->Add("xPLFrag-Test.default:lampA","HIGH");
    assert(false==m_cache->IsFull());

    for(int i=0; i < 20; i++)
        m_cache->Add("xPLFrag-Test.default:lampA","HIGH");
    assert(true==m_cache->IsFull());

    return true;
}

bool TestCacheMemory::Write(const std::string& canonic, const std::string& value, const time_t time)
{
    m_canonic[m_index] = canonic;
    m_value[m_index]   = value;
    m_time[m_index]    = time;
    m_index++;
    return true;
}

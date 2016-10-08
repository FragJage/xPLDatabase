#include "SimpleFolders/SimpleFolders.h"
#include "TestCacheFile.h"

using namespace std;

TestCacheFile::TestCacheFile() : TestClass("CacheFile", this)
{
    SimpleFolders tmpFolder;
    string folder;

    folder = tmpFolder.GetFolder(SimpleFolders::FolderType::Temp);
    m_fileName = tmpFolder.AddFile(folder, "test.tmp");
    m_cache = new CacheFile(m_fileName, 1);
    m_index = 0;

	addTest("Add", &TestCacheFile::TestAdd);
	addTest("Write", &TestCacheFile::TestWrite);
	addTest("IsFull", &TestCacheFile::TestIsFull);
}

TestCacheFile::~TestCacheFile()
{
    unlink(m_fileName.c_str());
    delete m_cache;
}

bool TestCacheFile::TestAdd()
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

bool TestCacheFile::TestWrite()
{
    struct tm t0;
    struct tm *pt;
    time_t t1;
    time_t t2;

    t0.tm_year = 100;  t0.tm_mon = 0;  t0.tm_mday = 1; t0.tm_isdst = 0;
    t0.tm_hour = 10;   t0.tm_min = 17; t0.tm_sec = 25;
    t1 = mktime(&t0);
    t0.tm_hour = 21;   t0.tm_min = 42; t0.tm_sec = 50;
    t2 = mktime(&t0);

    m_cache->Write("xPLFrag-Test.default:lampB","LOW", t1);
    m_cache->Write("xPLFrag-Test.default:tempB","26.5", t2);
    m_index = 0;
    m_cache->Transfert(this);

    assert(2==m_index);
    assert("xPLFrag-Test.default:lampB"==m_canonic[0]);
    assert("xPLFrag-Test.default:tempB"==m_canonic[1]);
    assert("LOW"==m_value[0]);
    assert("26.5"==m_value[1]);

    pt = localtime(&m_time[0]);
    assert(17==pt->tm_min);
    pt = localtime(&m_time[1]);
    assert(42==pt->tm_min);

    return true;
}

bool TestCacheFile::TestIsFull()
{
    for(int i=0; i < 15; i++)
        m_cache->Add("xPLFrag-Test.default:lampA","HIGH");
    assert(false==m_cache->IsFull());

    for(int i=0; i < 15; i++)
        m_cache->Add("xPLFrag-Test.default:lampA","HIGH");
    assert(true==m_cache->IsFull());

    return true;
}

bool TestCacheFile::Write(const std::string& canonic, const std::string& value, const time_t time)
{
    m_canonic[m_index] = canonic;
    m_value[m_index]   = value;
    m_time[m_index]    = time;
    m_index++;
    return true;
}

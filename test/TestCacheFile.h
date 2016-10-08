#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/CacheFile.h"

using namespace std;

class TestCacheFile : public TestClass<TestCacheFile>, ITransfert
{
public:
    TestCacheFile();
    ~TestCacheFile();

    bool TestAdd();
    bool TestWrite();
    bool TestIsFull();

    bool Write(const std::string& canonic, const std::string& value, const time_t time);

    CacheFile *m_cache;
    int m_index;
    std::string m_canonic[2];
    std::string m_value[2];
    time_t m_time[2];
private:
    std::string m_fileName;
};

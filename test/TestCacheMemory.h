#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/CacheMemory.h"

using namespace std;

class TestCacheMemory : public TestClass<TestCacheMemory>, ITransfert
{
public:
    TestCacheMemory();
    ~TestCacheMemory();

    bool TestAdd();
    bool TestWrite();
    bool TestIsFull();

    bool Write(const std::string& canonic, const std::string& value, const time_t time);

    CacheMemory *m_cache;
    int m_index;
    std::string m_canonic[2];
    std::string m_value[2];
    time_t m_time[2];
};

#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/Filter.h"

using namespace std;

class TestFilter : public TestClass<TestFilter>
{
public:
    TestFilter();
    ~TestFilter();

    bool SetAndGet();
    bool SetPartial();
    bool Compare();
};

#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/DateHelper.h"

using namespace std;

class TestDateHelper : public TestClass<TestDateHelper>
{
public:
    TestDateHelper();
    ~TestDateHelper();

    bool ToDateAAAAMMJJ();
    bool ToDateJJMMAAAA();
    bool ToString();
};

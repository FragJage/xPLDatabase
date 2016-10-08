#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/RequestValue.h"

using namespace std;

class TestRequestValue : public TestClass<TestRequestValue>
{
public:
    TestRequestValue();
    ~TestRequestValue();

    bool ConvertHIGH();
    bool ConvertLOW();
    bool NOConvert();
};

#include <stdlib.h>
#include "TestRequestValue.h"

using namespace std;

TestRequestValue::TestRequestValue() : TestClass("RequestValue", this)
{
	addTest("ConvertHIGH", &TestRequestValue::ConvertHIGH);
	addTest("ConvertLOW", &TestRequestValue::ConvertLOW);
	addTest("NOConvert", &TestRequestValue::NOConvert);
}

TestRequestValue::~TestRequestValue()
{
}

bool TestRequestValue::ConvertHIGH()
{
    RequestValue rv(time(nullptr), "HIGH");
    assert(rv.nValue==1);

    return true;
}

bool TestRequestValue::ConvertLOW()
{
    RequestValue rv(time(nullptr), "LOW");
    assert(rv.nValue==0);

    return true;
}

bool TestRequestValue::NOConvert()
{
    RequestValue rv(time(nullptr), "28.6");
    assert(rv.Value=="28.6");

    return true;
}

#include <exception>
#include "UnitTest/UnitTest.h"
#include "TestCacheFile.h"
#include "TestCacheMemory.h"
#include "TestDateHelper.h"
#include "TestFilter.h"
#include "TestRequestValue.h"
#include "TestxPLDatabase.h"

using namespace std;

int main()
{
    UnitTest unitTest;
    int ret = 0;


    try
    {
        unitTest.addTestClass(new TestCacheFile());
        unitTest.addTestClass(new TestCacheMemory());
        unitTest.addTestClass(new TestDateHelper());
        unitTest.addTestClass(new TestFilter());
        unitTest.addTestClass(new TestRequestValue());
        unitTest.addTestClass(new TestxPLDatabase());
    }
    catch(const exception &e)
    {
        unitTest.displayError(e.what());
        ret = -1;
    }

    if(ret!=-1)
        if(!unitTest.run()) ret = 1;

	return ret;
}

#include <stdlib.h>
#include "TestFilter.h"

using namespace std;

TestFilter::TestFilter() : TestClass("Filter", this)
{
	addTest("SetAndGet", &TestFilter::SetAndGet);
	addTest("Compare", &TestFilter::Compare);
}

TestFilter::~TestFilter()
{
}

bool TestFilter::SetAndGet()
{
    Filter myFilter;

    myFilter.Set("xPLFrag-Test.default:lamp", "current");

    assert("xPLFrag" == myFilter.GetVendor());
    assert("Test" == myFilter.GetModule());
    assert("default" == myFilter.GetInstance());
    assert("lamp" == myFilter.GetDevice());
    assert("current" == myFilter.GetValueName());

    return true;
}

bool TestFilter::Compare()
{
    assert(true==Filter::Compare("*","TEST"));
    assert(true==Filter::Compare("TEST","*"));
    assert(true==Filter::Compare("TE*","TEST"));
    assert(true==Filter::Compare("TEST","TE*"));
    assert(true==Filter::Compare("TEST","test"));
    assert(false==Filter::Compare("TF*","TEST"));
    assert(false==Filter::Compare("TEST","TF*"));
    assert(false==Filter::Compare("TEST","TESTA"));
    assert(false==Filter::Compare("TESTA","test"));

    return true;
}

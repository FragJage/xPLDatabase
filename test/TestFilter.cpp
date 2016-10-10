#include <stdlib.h>
#include "TestFilter.h"

using namespace std;

TestFilter::TestFilter() : TestClass("Filter", this)
{
	addTest("SetAndGet", &TestFilter::SetAndGet);
	addTest("SetPartial", &TestFilter::SetPartial);
	addTest("Compare", &TestFilter::Compare);
}

TestFilter::~TestFilter()
{
}

bool TestFilter::SetAndGet()
{
    Filter myFilter("xPLFrag-Test.default:lamp", "current");

    assert("xPLFrag" == myFilter.GetVendor());
    assert("Test" == myFilter.GetModule());
    assert("default" == myFilter.GetInstance());
    assert("lamp" == myFilter.GetDevice());
    assert("current" == myFilter.GetValueName());

    return true;
}

bool TestFilter::SetPartial()
{
    Filter myFilter;

    myFilter.Set("xPLFrag", "current");
    assert("xPLFrag" == myFilter.GetVendor());
    assert("*" == myFilter.GetModule());
    assert("*" == myFilter.GetInstance());
    assert("*" == myFilter.GetDevice());

    myFilter.Set("xPLFrag-Test", "current");
    assert("xPLFrag" == myFilter.GetVendor());
    assert("Test" == myFilter.GetModule());
    assert("*" == myFilter.GetInstance());
    assert("*" == myFilter.GetDevice());

    myFilter.Set("xPLFrag-Test.default", "current");
    assert("xPLFrag" == myFilter.GetVendor());
    assert("Test" == myFilter.GetModule());
    assert("default" == myFilter.GetInstance());
    assert("*" == myFilter.GetDevice());

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

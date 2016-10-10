#include <stdlib.h>
#include "TestDateHelper.h"

using namespace std;

TestDateHelper::TestDateHelper() : TestClass("DateHelper", this)
{
	addTest("ToDate AAAA-MM-JJ", &TestDateHelper::ToDateAAAAMMJJ);
	addTest("ToDate JJ/MM/AAAA", &TestDateHelper::ToDateJJMMAAAA);
	addTest("ToString", &TestDateHelper::ToString);
}

TestDateHelper::~TestDateHelper()
{
}

bool TestDateHelper::ToDateAAAAMMJJ()
{
    time_t t1;
    struct tm *t2;

    t1 = DateHelper::ToDate("2016-06-15 14:13:12");
    t2 = localtime(&t1);

    assert(116==t2->tm_year);
    assert(5==t2->tm_mon);
    assert(15==t2->tm_mday);
    assert(14==t2->tm_hour);
    assert(13==t2->tm_min);
    assert(12==t2->tm_sec);

    return true;
}

bool TestDateHelper::ToDateJJMMAAAA()
{
    time_t t1;
    struct tm *t2;

    t1 = DateHelper::ToDate("24/07/2015 13:00:00");
    t2 = localtime(&t1);
    assert(115==t2->tm_year);
    assert(6==t2->tm_mon);
    assert(24==t2->tm_mday);

    return true;
}

bool TestDateHelper::ToString()
{
    time_t t1;
    struct tm *t2;
    string t3;

    t1 = time(nullptr);
    t2 = localtime(&t1);

    t3 = DateHelper::ToString(t1, "%Y");
    assert(atoi(t3.c_str()) == t2->tm_year+1900);

    t3 = DateHelper::ToString(t1, "%m");
    assert(atoi(t3.c_str()) == t2->tm_mon+1);

    t3 = DateHelper::ToString(t1, "%d");
    assert(atoi(t3.c_str()) == t2->tm_mday);

    t3 = DateHelper::ToString(t1, "%H");
    assert(atoi(t3.c_str()) == t2->tm_hour);

    t3 = DateHelper::ToString(t1, "%M");
    assert(atoi(t3.c_str()) == t2->tm_min);

    t3 = DateHelper::ToString(t1, "%S");
    assert(atoi(t3.c_str()) == t2->tm_sec);

    return true;
}

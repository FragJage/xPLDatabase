#include<cstdio>
#include "DateHelper.h"

using namespace std;

time_t DateHelper::ToDate(const std::string& s)
{
    int i;
    int nb;
    struct tm tmTime;

    nb = sscanf(s.c_str(), "%d-%d-%d %d:%d:%d", &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday, &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);
    if(nb!=6) nb = sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d", &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday, &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);
    if(nb!=6) nb = sscanf(s.c_str(), "%d/%d/%d %d:%d:%d", &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday, &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);
    if(nb!=6) nb = sscanf(s.c_str(), "%d/%d/%dT%d:%d:%d", &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday, &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);
    if(nb!=6) return 0;

    if(tmTime.tm_mday>1900)
    {
        i = tmTime.tm_mday;
        tmTime.tm_mday = tmTime.tm_year;
        tmTime.tm_year = i;
    }
    tmTime.tm_year -= 1900;
    tmTime.tm_mon--;
    tmTime.tm_isdst = 1;

    return mktime(&tmTime);
}

std::string DateHelper::ToString(time_t t, const std::string& format)
{
    char buff[20];
    strftime(buff, 20, format.c_str(), localtime(&t));
    return string(buff);
}

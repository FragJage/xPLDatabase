#ifndef DateHelper_H
#define DateHelper_H

#include<ctime>
#include<string>

class DateHelper
{
    public:
        static time_t ToDate(const std::string& s);
        static std::string ToString(time_t t, const std::string& format="%Y-%m-%d %H:%M:%S");
};
#endif

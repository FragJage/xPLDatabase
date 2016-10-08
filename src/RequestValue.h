#ifndef FRAGREQUESTVALUE_H
#define FRAGREQUESTVALUE_H

#include<string>

class RequestValue
{
    public:
        RequestValue(time_t time, std::string value);

        time_t Time;
        std::string Value;
        double nValue;
};

#endif // FRAGREQUESTVALUE_H

#include <cstdlib>
#include "RequestValue.h"


using namespace std;

RequestValue::RequestValue(time_t time, std::string value)
{
    Time = time;
    Value = value;

    nValue = strtod(value.c_str(), nullptr);
    if(value=="HIGH")
    {
        nValue = 1;
        return;
    }
    if(value=="LOW")
    {
        nValue = 0;
        return;
    }
}

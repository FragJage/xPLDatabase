#include "Filter.h"
#include "StringTools/StringTools.h"

using namespace std;

Filter::Filter()
{
}

Filter::Filter(const std::string& source, const std::string& valueName)
{
    Set(source, valueName);
}

Filter::~Filter()
{
}

void Filter::Set(const std::string& source, const std::string& valueName)
{
    size_t posMinus;
    size_t posPoint;
    size_t posDPoint;


    m_valueName = valueName;

    m_vendor = "*";
    m_module = "*";
    m_instance = "*";
    m_device = "*";

    if(m_valueName=="") m_valueName = "currrent";

    if(source=="") return;

    posMinus = source.find('-');
    if(posMinus==string::npos)
    {
        m_vendor = source;
        return;
    }
    m_vendor = source.substr(0, posMinus);

    posPoint = source.find('.', posMinus+1);
    if(posPoint==string::npos)
    {
        m_module = source.substr(posMinus+1);
        return;
    }
    m_module = source.substr(posMinus+1, posPoint-posMinus-1);

    posDPoint = source.find(':', posPoint+1);
    if(posDPoint==string::npos)
    {
        m_instance = source.substr(posPoint+1);
        return;
    }
    m_instance = source.substr(posPoint+1, posDPoint-posPoint-1);

    m_device = source.substr(posDPoint+1);
}

string Filter::GetVendor()
{
    return m_vendor;
}

string Filter::GetModule()
{
    return m_module;
}

string Filter::GetInstance()
{
    return m_instance;
}

string Filter::GetDevice()
{
    return m_device;
}

string Filter::GetValueName()
{
    return m_valueName;
}

bool Filter::Compare(const string& word1, const string& word2)
{
    size_t pos1 = 0;
    size_t pos2 = 0;
    size_t max1 = word1.length();
    size_t max2 = word2.length();


    while(1)
    {
        if(word1[pos1]=='*') return true;
        if(word2[pos2]=='*') return true;
        if (tolower(word1[pos1]) != tolower(word2[pos2])) return false;
        pos1++;
        pos2++;
        if((pos1==max1)&&(pos2==max2)) return true;
        if((pos1==max1)||(pos2==max2)) return false;
    }

    return true;
}

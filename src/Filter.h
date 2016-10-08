#ifndef FRAGFILTER_H
#define FRAGFILTER_H

#include<string>
#include<time.h>

class Filter
{
    public:
		Filter();
        Filter(const std::string& source, const std::string& valueName);
        ~Filter();
        void Set(const std::string& source, const std::string& valueName);
        std::string GetVendor();
        std::string GetModule();
        std::string GetInstance();
        std::string GetDevice();
        std::string GetValueName();
        static bool Compare(const std::string& word1, const std::string& word2);

    private:
        std::string m_vendor;
        std::string m_module;
        std::string m_instance;
        std::string m_device;
        std::string m_valueName;
};

#endif // FRAGFILTER_H

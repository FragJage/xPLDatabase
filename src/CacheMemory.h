#ifndef FRAGCACHEMEMORY_H
#define FRAGCACHEMEMORY_H

#include<vector>
#include "ICache.h"

class CacheElement
{
    public:
        CacheElement(const std::string& Source, const std::string& Valeur);
        ~CacheElement();

        std::string m_Source;
        std::string m_Valeur;
        time_t m_Time;
};

class CacheMemory : public ICache
{
    public:
        CacheMemory(unsigned int maxMem);
        ~CacheMemory();

        bool Add(const std::string& canonic, const std::string& value);
        bool Transfert(ITransfert* transfert);
        bool IsFull();
        void Raz();
        bool Write(const std::string& canonic, const std::string& value, const time_t time);

    private:
        unsigned int m_MaxMemory;
        unsigned int m_CurrentMemory;
        std::vector<CacheElement> m_Cache;
};
#endif // FRAGCACHEMEMORY_H

#ifndef FRAGCACHE_H
#define FRAGCACHE_H

#include <string>

class ITransfert
{
    public:
        virtual bool Write(const std::string& canonic, const std::string& value, const time_t time) = 0;
};

class ICache : public ITransfert
{
    public:
        virtual ~ICache() {}
        virtual bool Add(const std::string& canonic, const std::string& value) = 0;
        virtual bool IsFull() = 0;
        virtual void Raz() = 0;
        virtual bool Transfert(ITransfert* transfert) = 0;
};

#endif // FRAGCACHE_H

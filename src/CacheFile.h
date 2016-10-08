#ifndef FRAGCACHEFILE_H
#define FRAGCACHEFILE_H

#include <fstream>
#include "ICache.h"

class CacheFile : public ICache
{
    public:
        CacheFile(std::string fileName, unsigned int maxSize);
        ~CacheFile();

        bool Add(const std::string& canonic, const std::string& value);
        bool Transfert(ITransfert* transfert);
        bool IsFull();
        void Raz();
        bool Write(const std::string& canonic, const std::string& value, const time_t time);

    private:
        unsigned int m_MaxSize;
        unsigned int m_CurrentSize;
        std::string m_FileName;
        std::fstream m_CacheFile;
};
#endif // FRAGCACHEFILE_H




#ifndef __OG_EXTENSION_H__
#define __OG_EXTENSION_H__

#include "platform/OGPlatformMacros.h"

OG_BEGIN

struct ResourceData
{
    int         type;
    std::string file;
    std::string plist;

    ResourceData()
    {
        type = 0;
        file = "";
        plist = "";
    }

    ResourceData(int iType, std::string sFile, std::string sPlist)
    {
        type = iType;
        file = sFile;
        plist = sPlist;
    }
};

class NodeExtension
{
public:
    NodeExtension();
    ~NodeExtension();

private:

};


OG_END


#endif
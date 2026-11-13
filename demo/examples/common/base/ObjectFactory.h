 

#ifndef _OG_TRIGGER_FACTORY_H_
#define _OG_TRIGGER_FACTORY_H_

#include <string>
#include <unordered_map>
#include <functional>
#include "base/OGRef.h"
#include "platform/OGPlatformMacros.h"

OG_BEGIN

class ObjectFactory
{
public:
    typedef orange::Ref* (*Instance)(void);
    typedef std::function<orange::Ref* (void)> InstanceFunc;
    struct TInfo
    {
        TInfo();
        TInfo(const std::string& type, Instance ins = nullptr);
        TInfo(const std::string& type, InstanceFunc ins = nullptr);
        TInfo(const TInfo &t);
        ~TInfo();
        TInfo& operator= (const TInfo &t);
        std::string _class;
        Instance _fun;
        InstanceFunc _func;
    };
    typedef std::unordered_map<std::string, TInfo>  FactoryMap;

    static ObjectFactory* getInstance();
    static void destroyInstance();
    orange::Ref* createObject(const std::string &name);

    void registerType(const TInfo &t);
    void removeAll();

protected:
    ObjectFactory();
    virtual ~ObjectFactory();
private:
    static ObjectFactory *_sharedFactory;
    FactoryMap _typeMap;
};

OG_END

#endif

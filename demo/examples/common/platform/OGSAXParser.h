 

#ifndef __OGSAXPARSER_H__
#define __OGSAXPARSER_H__
/// @cond DO_NOT_SHOW

#include "platform/OGPlatformConfig.h"
#include "base/ogMacros.h"
 
#include "platform/OGPlatformMacros.h"
#include "tinyxml2.h"

#include <string>

OG_BEGIN

/**
 * @addtogroup platform
 * @{
 */
 

typedef unsigned char OG_XML_CHAR;

class SAXDelegator
{
public:
    virtual ~SAXDelegator() {}

    /**
     * @js NA
     * @lua NA
     */
    virtual void startElement(void *ctx, const char *name, const char **atts) = 0;
    /**
     * @js NA
     * @lua NA
     */
    virtual void endElement(void *ctx, const char *name) = 0;
    /**
     * @js NA
     * @lua NA
     */
    virtual void textHandler(void *ctx, const char *s, size_t len) = 0;
};

class SAXParser
{
    SAXDelegator*    _delegator;
public:
    /**
     * @js NA
     * @lua NA
     */
    SAXParser();
    /**
     * @js NA
     * @lua NA
     */
    ~SAXParser(void);
    /**
     * @js NA
     * @lua NA
     */
    bool init(const char *encoding);
    /**
     * @js NA
     * @lua NA
     */
    bool parse(const char* xmlData, size_t dataLength);
	 
    /**
     * @js NA
     * @lua NA
     */
    bool parse(const std::string& filename);
	bool parseIntrusive(const char * xmlData, size_t dataLength);
	bool parseIntrusive(tinyxml2::XMLNode * node);
 

    /**
     * @js NA
     * @lua NA
     */
    void setDelegator(SAXDelegator* delegator);
    /**
     * @js NA
     * @lua NA
     */
    static void startElement(void *ctx, const OG_XML_CHAR *name, const OG_XML_CHAR **atts);
    /**
     * @js NA
     * @lua NA
     */
    static void endElement(void *ctx, const OG_XML_CHAR *name);
    /**
     * @js NA
     * @lua NA
     */
    static void textHandler(void *ctx, const OG_XML_CHAR *name, size_t len);
};

// end of platform group
/// @}

OG_END

/// @endcond
#endif //__OGSAXPARSER_H__

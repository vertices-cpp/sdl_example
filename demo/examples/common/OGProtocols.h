
#ifndef __BASE_CCPROTOCOLS_H__
#define __BASE_CCPROTOCOLS_H__
/// @cond DO_NOT_SHOW

#include <string>

#include "ogTypes.h"
#include "OGTexture2D.h"

OG_BEGIN
 
class   __RGBAProtocol
{
public:
    virtual ~__RGBAProtocol() {} 
    virtual void setColor(const Color3B& color) = 0; 
    virtual const Color3B& getColor() const = 0; 
    virtual const Color3B& getDisplayedColor() const = 0; 
    virtual unsigned char getDisplayedOpacity() const = 0; 
    virtual unsigned char getOpacity() const = 0; 
    virtual void setOpacity(unsigned char opacity) = 0; 
    virtual void setOpacityModifyRGB(bool value) = 0; 
    virtual bool isOpacityModifyRGB() const = 0; 
    virtual bool isCascadeColorEnabled() const = 0; 
    virtual void setCascadeColorEnabled(bool cascadeColorEnabled) = 0; 
    virtual void updateDisplayedColor(const Color3B& color) = 0; 
    virtual bool isCascadeOpacityEnabled() const = 0; 
    virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled) = 0; 
    virtual void updateDisplayedOpacity(unsigned char opacity) = 0;
};

class   BlendProtocol
{
public:
    virtual ~BlendProtocol() {} 
    virtual void setBlendFunc(const BlendFunc &blendFunc) = 0; 
    virtual const BlendFunc &getBlendFunc() const = 0;
};
 
class   TextureProtocol : public BlendProtocol
{
public:
    virtual ~TextureProtocol() {} 
    virtual Texture2D* getTexture() const = 0; 
    virtual void setTexture(Texture2D *texture) = 0;
};
 
class   LabelProtocol
{
public:
    virtual ~LabelProtocol() {}
	 
    virtual void setString(const std::string &label) = 0; 
    virtual const std::string& getString() const = 0;
}; 
class   DirectorDelegate
{
public:
    virtual ~DirectorDelegate() {} 
    virtual void updateProjection() = 0;
};
 
class   PlayableProtocol
{
public:
    virtual ~PlayableProtocol(){}
    
    virtual void start() = 0;
    
    virtual void stop() = 0;
};
OG_END

/// @endcond
#endif // __BASE_OGPROTOCOLS_H__

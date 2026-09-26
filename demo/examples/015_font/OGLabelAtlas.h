
#ifndef __OGLABEL_ATLAS_H__
#define __OGLABEL_ATLAS_H__

#include <string>
#include "OGPlatformMacros.h"
#include "OGRef.h"
#include "OGTexture2D.h"
#include "OGNode.h"
// #if OG_LABELATLAS_DEBUG_DRAW
// #include "OGCustomCommand.h"
// #include "OGDrawNode.h"
// #endif
OG_BEGIN


class   LabelProtocol
{
public:
	virtual ~LabelProtocol() {} 
	virtual void setString(const std::string &label) = 0; 
	virtual const std::string& getString() const = 0;
};

class  LabelAtlas : public Node, public LabelProtocol
{
public:

	static LabelAtlas* create();
	static LabelAtlas* create(const std::string& string, const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
	static LabelAtlas* create(const std::string& string, const std::string& fntFile);
	bool initWithString(const std::string& string, const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
	bool initWithString(const std::string& string, const std::string& fntFile);
	bool initWithString(const std::string& string, Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
	virtual void setString(const std::string &label) override;
	virtual const std::string& getString() const override;
	virtual void updateAtlasValues()  ;
	virtual std::string getDescription() const  ;

#if OG_LABELATLAS_DEBUG_DRAW
	virtual void draw(Renderer *renderer, const Mat3 &transform, uint32_t flags) override;
#endif

public:
	LabelAtlas()
		:_string("")
	{
	}

	virtual ~LabelAtlas()
	{
		_string.clear();
	}

protected:
	virtual void updateColor()  ;
	// string to render
	std::string _string;
	// the first char in the char map
	int _mapStartChar;

	/** Width of each char. */
	int    _itemWidth = 0;
	/** Height of each char. */
	int    _itemHeight = 0;
};

// end group
/// @}

OG_END

#endif //__OGLABEL_ATLAS_H__

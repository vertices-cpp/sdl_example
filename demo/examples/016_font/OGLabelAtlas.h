#ifndef __OGLABEL_ATLAS_H__
#define __OGLABEL_ATLAS_H__

#include <string>
#include <vector>
#include "OGNode.h" 
#include "ogTypes.h"
#include "OGQuadCommand.h" 

#include "OGProtocols.h"
 
OG_BEGIN
 

class LabelAtlas : public Node,  public LabelProtocol
{
public:
	static LabelAtlas* create();

	static LabelAtlas* create(const std::string& string,
		const std::string& charMapFile,
		int itemWidth, int itemHeight, int startCharMap);

	static LabelAtlas* create(const std::string& string,
		const std::string& fntFile);

	bool initWithString(const std::string& string,
		const std::string& charMapFile,
		int itemWidth, int itemHeight, int startCharMap);

	bool initWithString(const std::string& string,
		const std::string& fntFile);

	bool initWithString(const std::string& string,
		Texture2D* texture,
		int itemWidth, int itemHeight, int startCharMap);

	void setString(const std::string& label);
	const std::string& getString() const { return _string; }

	void setTextColor(const Color4B& color);
	const Color4B& getTextColor() const { return _textColor; }

	void setBlendFunc(const BlendFunc& blendFunc) { _blendFunc = blendFunc; }
	const BlendFunc& getBlendFunc() const { return _blendFunc; }

	virtual void updateDisplayedColor(const Color3B& parentColor) override;
	virtual void updateDisplayedOpacity(uint8_t parentOpacity) override;

	virtual void visit(Renderer* renderer, const Mat3& parentTransform, uint32_t parentFlags) override;
	virtual void draw(Renderer* renderer, const Mat3& transform, uint32_t flags) override;

	virtual std::string getDescription() const;

	virtual ~LabelAtlas();

protected:
	// 等宽字符排版，填 _quads
	void rebuildQuads();

	// 文本（一份，按字节处理，只支持 ASCII）
	std::string _string;

	// 图集纹理（自己持有）
	Texture2D* _texture = nullptr;

	// 图集参数
	int _itemWidth = 0;
	int _itemHeight = 0;
	int _startCharMap = 0;
	int _itemsPerRow = 0;

	// 渲染状态
	Color4B   _textColor = Color4B::WHITE;
	BlendFunc _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
	bool      _isOpacityModifyRGB = true;

	std::vector<bool> _validFlags;
	// 渲染缓冲
	std::vector<V2F_C4B_T2F_Quad> _quads;
	std::vector<V2F_C4B_T2F>      _outVerts;
	//std::vector<int>         _indices;
	QuadCommand                   _quadCommand;
};

OG_END

#endif
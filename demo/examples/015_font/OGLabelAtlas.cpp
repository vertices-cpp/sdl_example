#include "OGLabelAtlas.h"

#include "OGFileUtils.h"
#include "OGDirector.h"

#include "tinyxml2.h"

#include <cstring>
#include <cstdlib>

OG_BEGIN

// ============================================================
// create
// ============================================================
LabelAtlas* LabelAtlas::create()
{
	auto ret = new (std::nothrow) LabelAtlas();
	if (ret) ret->autorelease();
	return ret;
}

LabelAtlas* LabelAtlas::create(const std::string& string,
	const std::string& charMapFile,
	int itemWidth, int itemHeight, int startCharMap)
{
	auto ret = new (std::nothrow) LabelAtlas();
	if (ret && ret->initWithString(string, charMapFile,
		itemWidth, itemHeight, startCharMap))
	{
		ret->autorelease();
		return ret;
	}
	OG_SAFE_DELETE(ret);
	return nullptr;
}

LabelAtlas* LabelAtlas::create(const std::string& string, const std::string& fntFile)
{
	auto ret = new (std::nothrow) LabelAtlas();
	if (ret && ret->initWithString(string, fntFile))
	{
		ret->autorelease();
		return ret;
	}
	OG_SAFE_DELETE(ret);
	return nullptr;
}

// ============================================================
// 析构：释放持有的纹理
// ============================================================
LabelAtlas::~LabelAtlas()
{
	if (_texture)
	{
		_texture->release();
		_texture = nullptr;
	}
}

// ============================================================
// initWithString（文件名版）：加载纹理后转发
// ============================================================
bool LabelAtlas::initWithString(const std::string& string,
	const std::string& charMapFile,
	int itemWidth, int itemHeight, int startCharMap)
{
	Texture2D* tex = Director::getInstance()->getTextureCache()->addImage(charMapFile);
	if (!tex) return false;

	return initWithString(string, tex, itemWidth, itemHeight, startCharMap);
}

// ============================================================
// initWithString（Texture2D* 版）：真正干活的
// ============================================================
bool LabelAtlas::initWithString(const std::string& string,
	Texture2D* texture,
	int itemWidth, int itemHeight, int startCharMap)
{
	if (!Node::init()) return false;
	if (!texture) return false;
	if (itemWidth <= 0 || itemHeight <= 0) return false;

	// 释放旧的
	if (_texture)
	{
		_texture->release();
		_texture = nullptr;
	}

	_texture = texture;
	_texture->retain();

	_itemWidth = itemWidth;
	_itemHeight = itemHeight;
	_startCharMap = startCharMap;

	Size texSize = _texture->getContentSize();
	_itemsPerRow = (int)(texSize.width / itemWidth);
	if (_itemsPerRow <= 0) return false;

	_textColor = Color4B::WHITE;
	_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

	setString(string);
	return true;
}

// ============================================================
// initWithString（.fnt 版）：tinyxml2 解析
// ============================================================
bool LabelAtlas::initWithString(const std::string& theString,
	const std::string& fntFile)
{
	std::string pathStr = FileUtils::getInstance()->fullPathForFilename(fntFile);
	if (pathStr.empty()) return false;

	size_t pos = pathStr.find_last_of("/\\");
	std::string relPath = (pos == std::string::npos) ? "" : pathStr.substr(0, pos + 1);

	Data data = FileUtils::getInstance()->getDataFromFile(pathStr);
	if (data.isNull()) return false;

	tinyxml2::XMLDocument doc;
	if (doc.Parse((const char*)data.getBytes(), data.getSize()) != tinyxml2::XML_SUCCESS)
		return false;

	auto* plist = doc.FirstChildElement("plist");
	if (!plist) return false;
	auto* dict = plist->FirstChildElement("dict");
	if (!dict) return false;

	auto getValue = [&](const char* key) -> std::string
	{
		for (auto* node = dict->FirstChild(); node; node = node->NextSibling())
		{
			auto* keyElem = node->ToElement();
			if (!keyElem || strcmp(keyElem->Name(), "key") != 0) continue;

			const char* k = keyElem->GetText();
			if (!k || strcmp(k, key) != 0) continue;

			auto* valElem = keyElem->NextSiblingElement();
			if (!valElem) return "";
			const char* v = valElem->GetText();
			return v ? v : "";
		}
		return "";
	};

	std::string textureFilename = relPath + getValue("textureFilename");
	int width = atoi(getValue("itemWidth").c_str());
	int height = atoi(getValue("itemHeight").c_str());
	int startChar = atoi(getValue("firstChar").c_str());

	if (width <= 0 || height <= 0 || textureFilename.empty()) return false;

	return initWithString(theString, textureFilename, width, height, startChar);
}

// ============================================================
// setString：只存字节，不转 UTF-32
// ============================================================
void LabelAtlas::setString(const std::string& label)
{
	if (label == _string) return;
	_string = label;
	rebuildQuads();
}

// ============================================================
// rebuildQuads：等宽字符排版
// ============================================================
void LabelAtlas::rebuildQuads()
{
	if (!_texture || _itemsPerRow <= 0 || _string.empty())
	{
		_quads.clear();
		_validFlags.clear();
		setContentSize(Size::ZERO);
		return;
	}

	Size  texSize = _texture->getContentSize();
	float texW = texSize.width;
	float texH = texSize.height;
	float itemW = (float)_itemWidth;
	float itemH = (float)_itemHeight;

	int n = (int)_string.size();
	_quads.resize(n);
	_validFlags.resize(n);

	Color4B c(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);
	if (_isOpacityModifyRGB)
	{
		c.r = (uint8_t)(c.r * _displayedOpacity / 255.0f);
		c.g = (uint8_t)(c.g * _displayedOpacity / 255.0f);
		c.b = (uint8_t)(c.b * _displayedOpacity / 255.0f);
	}

	for (int i = 0; i < n; ++i)
	{
		unsigned char cp = (unsigned char)_string[i];
		int a = (int)cp - _startCharMap;

		if (a < 0)
		{
			_validFlags[i] = false;
			continue;
		}
		_validFlags[i] = true;

		int row = a % _itemsPerRow;
		int col = a / _itemsPerRow;

		float left = row * itemW / texW;
		float right = left + itemW / texW;
		float top = col * itemH / texH;
		float bottom = top + itemH / texH;

		float x1 = i * itemW;
		float y1 = 0.0f;
		float x2 = x1 + itemW;
		float y2 = y1 + itemH;

		_quads[i].tl = { {x1, y1}, c, {left,  top   } };
		_quads[i].tr = { {x2, y1}, c, {right, top   } };
		_quads[i].bl = { {x1, y2}, c, {left,  bottom} };
		_quads[i].br = { {x2, y2}, c, {right, bottom} };
	}

	setContentSize(Size(n * itemW, itemH));
}

// ============================================================
// 颜色 / 透明度：重建 quads
// ============================================================
void LabelAtlas::updateDisplayedColor(const Color3B& parentColor)
{
	Node::updateDisplayedColor(parentColor);
	rebuildQuads();
}

void LabelAtlas::updateDisplayedOpacity(uint8_t parentOpacity)
{
	Node::updateDisplayedOpacity(parentOpacity);
	rebuildQuads();
}

void LabelAtlas::setTextColor(const Color4B& color)
{
	_textColor = color;
	rebuildQuads();
}

// ============================================================
// visit：极简，没有 _contentDirty 检查
// ============================================================
void LabelAtlas::visit(Renderer* renderer, const Mat3& parentTransform, uint32_t parentFlags)
{
	if (!_visible) return;

	uint32_t flags = processParentFlags(parentTransform, parentFlags);

	if (!_children.empty())
	{
		sortAllChildren();
		for (auto* child : _children)
			child->visit(renderer, _modelViewTransform, flags);
	}

	if (!_string.empty() && isVisitableByVisitingCamera())
		draw(renderer, _modelViewTransform, flags);
}

// ============================================================
// draw：遍历 _quads → transform.out → QuadCommand
// ============================================================
void LabelAtlas::draw(Renderer* renderer, const Mat3& transform, uint32_t flags)
{
	if (_quads.empty() || !_texture) return;

	_texture->setBlendMode(_blendFunc.getBlendMode());

	int n = (int)_quads.size();
	if ((int)_outVerts.size() < n * 4) _outVerts.resize(n * 4);
	if ((int)_indices.size() < n * 6) _indices.resize(n * 6);

	int vertCount = 0;

	for (int i = 0; i < n; ++i)
	{
		if (i >= (int)_validFlags.size() || !_validFlags[i]) continue;

		const auto& q = _quads[i];
		const V2F_C4B_T2F* src[4] = { &q.tl, &q.tr, &q.bl, &q.br };
		int base = vertCount * 4;

		for (int j = 0; j < 4; ++j)
		{
			float vx = src[j]->vertices.x;
			float vy = src[j]->vertices.y;
			transform.out(vx, vy);

			_outVerts[base + j].vertices = { vx, vy };
			_outVerts[base + j].colors = src[j]->colors;
			_outVerts[base + j].texCoords = src[j]->texCoords;
		}

		int iC = vertCount * 6;
		_indices[iC + 0] = base + 0;
		_indices[iC + 1] = base + 1;
		_indices[iC + 2] = base + 2;
		_indices[iC + 3] = base + 1;
		_indices[iC + 4] = base + 3;
		_indices[iC + 5] = base + 2;
		vertCount++;
	}

	if (vertCount <= 0) return;

	_quadCommand.init(_globalZOrder, _texture->getTexture(),
		_outVerts.data(), _indices.data(),
		vertCount * 4, vertCount * 6);
	renderer->addCommand(&_quadCommand);
}

// ============================================================
// getDescription
// ============================================================
std::string LabelAtlas::getDescription() const
{
	char buf[128];
	snprintf(buf, sizeof(buf), "<LabelAtlas | Tag = %d, Label = '%s'>", _tag, _string.c_str());
	return buf;
}

OG_END
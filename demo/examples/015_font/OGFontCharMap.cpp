 
#include "OGFontCharMap.h"
#include "OGFontAtlas.h"
#include "OGDirector.h"
#include "OGMap.h"
#include "iconv_wrapper.h"


#include "OGFileUtils.h"      // Data, FileUtils
#include "OGTexture2D.h"

#include "tinyxml2.h"

OG_BEGIN

 
 

//--------------------FontCharMap----------------------------------
FontCharMap * FontCharMap::create(const std::string& plistFile)
{
	// 	std::string pathStr = FileUtils::getInstance()->fullPathForFilename(plistFile);
	// 	std::string relPathStr = pathStr.substr(0, pathStr.find_last_of('/')) + "/";
	// 
	// 	ValueMap dict = FileUtils::getInstance()->getFileCache()->addData(pathStr);
	// 
	// 	//   OGASSERT(dict["version"].asInt() == 1, "Unsupported version. Upgrade orange version");
	// 
	// 	std::string textureFilename = relPathStr + dict["textureFilename"].asString();
	// 
	// 	unsigned int width = dict["itemWidth"].asInt();
	// 	unsigned int height = dict["itemHeight"].asInt();
	// 	unsigned int startChar = dict["firstChar"].asInt();
	// 
	// 	Texture2D *tempTexture = Director::getInstance()->getTextureCache()->addImage(textureFilename);
	// 	if (!tempTexture)
	// 	{
	// 		return nullptr;
	// 	}
	// 
	// 	FontCharMap *tempFont = new FontCharMap(tempTexture, width, height, startChar);
	// 
	// 	if (!tempFont)
	// 	{
	// 		return nullptr;
	// 	}
	// 	tempFont->autorelease();
	// 	return tempFont;


			// ---------- 1. 定位文件 ----------
	std::string pathStr = FileUtils::getInstance()->fullPathForFilename(plistFile);
	if (pathStr.empty()) return nullptr;

	std::string relPathStr = pathStr.substr(0, pathStr.find_last_of('/')) + "/";

	// ---------- 2. 读文件到内存 ----------
	Data data = FileUtils::getInstance()->getDataFromFile(pathStr);
	if (data.isNull() || data.getSize() == 0) return nullptr;

	// ---------- 3. tinyxml2 解析内存 ----------
	tinyxml2::XMLDocument doc;
	if (doc.Parse(reinterpret_cast<const char*>(data.getBytes()), data.getSize())
		!= tinyxml2::XML_SUCCESS)
		return nullptr;

	tinyxml2::XMLElement* root = doc.RootElement();
	if (!root || strcmp(root->Name(), "plist") != 0) return nullptr;

	tinyxml2::XMLElement* dict = root->FirstChildElement("dict");
	if (!dict) return nullptr;

	// ---------- 4. 遍历 <key> / 值 ----------
	std::string textureFilename;
	int width = 0, height = 0, startChar = 0;
	int version = -1;

	for (tinyxml2::XMLElement* node = dict->FirstChildElement();
		node != nullptr;
		node = node->NextSiblingElement())
	{
		if (strcmp(node->Name(), "key") != 0) continue;

		const char* key = node->GetText();
		if (!key) continue;

		tinyxml2::XMLElement* val = node->NextSiblingElement();
		if (!val) break;

		const char* tag = val->Name();
		const char* text = val->GetText();

		if (strcmp(key, "version") == 0 && strcmp(tag, "integer") == 0)
			version = text ? atoi(text) : 0;
		else if (strcmp(key, "textureFilename") == 0 && strcmp(tag, "string") == 0)
			textureFilename = text ? text : "";
		else if (strcmp(key, "itemWidth") == 0 && strcmp(tag, "integer") == 0)
			width = text ? atoi(text) : 0;
		else if (strcmp(key, "itemHeight") == 0 && strcmp(tag, "integer") == 0)
			height = text ? atoi(text) : 0;
		else if (strcmp(key, "firstChar") == 0 && strcmp(tag, "integer") == 0)
			startChar = text ? atoi(text) : 0;

		node = val;   // 跳过值节点，下一轮自然到下一个 <key>
	}

	if (version != 1) return nullptr;
	if (textureFilename.empty() || width <= 0 || height <= 0) return nullptr;

	std::string fullTexture = relPathStr + textureFilename;

	// ---------- 5. 用数据 ----------
	Texture2D* tempTexture = Director::getInstance()->getTextureCache()->addImage(fullTexture);
	if (!tempTexture) return nullptr;

	FontCharMap* tempFont = new (std::nothrow) FontCharMap(tempTexture, width, height, startChar);
	if (!tempFont) return nullptr;

	tempFont->autorelease();
	return tempFont;

}

FontCharMap* FontCharMap::create(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
	Texture2D *tempTexture = Director::getInstance()->getTextureCache()->addImage(charMapFile);

	if (!tempTexture)
	{
		return nullptr;
	}

	FontCharMap *tempFont = new FontCharMap(tempTexture, itemWidth, itemHeight, startCharMap);

	if (!tempFont)
	{
		return nullptr;
	}
	tempFont->autorelease();
	return tempFont;
}

FontCharMap* FontCharMap::create(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
	FontCharMap *tempFont = new FontCharMap(texture, itemWidth, itemHeight, startCharMap);

	if (!tempFont)
	{
		return nullptr;
	}
	tempFont->autorelease();
	return tempFont;
}

FontCharMap::~FontCharMap()
{

}

int* FontCharMap::getHorizontalKerningForTextUTF32(const std::u32string& /*text*/, int & /*outNumLetters*/) const
{
	return nullptr;
}

FontAtlas * FontCharMap::createFontAtlas()
{
	FontAtlas *tempAtlas = new (std::nothrow) FontAtlas(*this);
	if (!tempAtlas)
		return nullptr;

	Size s = _texture->getContentSize();
	int itemsPerColumn = (int)(s.height / _itemHeight);
	int itemsPerRow = (int)(s.width / _itemWidth);

	tempAtlas->setLineHeight((float)_itemHeight);

	//   auto contentScaleFactor = OG_CONTENT_SCALE_FACTOR();

	FontLetterDefinition tempDefinition;
	tempDefinition.textureID = 0;
	tempDefinition.offsetX = 0.0f;
	tempDefinition.offsetY = 0.0f;
	tempDefinition.validDefinition = true;
	tempDefinition.width = (float)_itemWidth;// / contentScaleFactor;
	tempDefinition.height = (float)_itemHeight;// / contentScaleFactor;
	tempDefinition.xAdvance =  _itemWidth;

	int charId = _mapStartChar;
	for (int row = 0; row < itemsPerColumn; ++row)
	{
		for (int col = 0; col < itemsPerRow; ++col)
		{
			tempDefinition.U = (float)(_itemWidth * col);// / contentScaleFactor;
			tempDefinition.V = (float)(_itemHeight * row);// / contentScaleFactor;

			tempAtlas->addLetterDefinition(charId, tempDefinition);
			charId++;
		}
	}

	tempAtlas->addTexture(_texture, 0);

	return tempAtlas;
}
 
 
OG_END
 

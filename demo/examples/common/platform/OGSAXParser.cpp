 
#include "platform/OGSAXParser.h"

#include <vector> // because its based on windows 8 build :P

#include "platform/OGFileUtils.h"
 

OG_BEGIN
 

SAXParser::SAXParser()
{
    _delegator = nullptr;
}

SAXParser::~SAXParser(void)
{
}

bool SAXParser::init(const char* /*encoding*/)
{
    // nothing to do
    return true;
}

bool SAXParser::parse(const std::string& filename)
{
	Data data = FileUtils::getInstance()->getDataFromFile(filename);
	if (!data.isNull())
	{
		return this->parse((const char*)data.getBytes(), data.getSize());
	}
	return false;
}

bool SAXParser::parse(const char* xmlData, size_t dataLength)
{
	if (xmlData != nullptr && dataLength > 0) {
		tinyxml2::XMLDocument doc;
		// 注意：tinyxml2 默认只支持 UTF-8，如果文件带 BOM 或其他编码，需要处理
		if (doc.Parse(xmlData, dataLength) != tinyxml2::XML_SUCCESS) {
			return false;
		}

		tinyxml2::XMLElement* root = doc.RootElement();
		if (!root) return false;

		// ★ 修复 1：手动触发根节点的 startElement
		std::vector<const char*> rootAtts;
		for (auto* a = root->FirstAttribute(); a; a = a->Next()) {
			rootAtts.push_back(a->Name());
			rootAtts.push_back(a->Value());
		}
		rootAtts.push_back(nullptr);
		_delegator->startElement(nullptr, root->Name(), rootAtts.data());

		// 递归解析所有子节点
		parseIntrusive(root);

		// ★ 修复 2：手动触发根节点的 endElement
		_delegator->endElement(nullptr, root->Name());

		return true;
	}
	return false;
}
bool SAXParser::parseIntrusive(const char* xmlData, size_t dataLength)
{
	return parse(xmlData, dataLength);
}
bool SAXParser::parseIntrusive(tinyxml2::XMLNode* node)
{
	if (!node || !_delegator) return false;

	for (tinyxml2::XMLNode* child = node->FirstChild(); child; child = child->NextSibling())
	{
		if (auto* elem = child->ToElement())
		{
			std::vector<const char*> atts;
			for (auto* a = elem->FirstAttribute(); a; a = a->Next()) {
				atts.push_back(a->Name());
				atts.push_back(a->Value());
			}
			atts.push_back(nullptr);

			_delegator->startElement(nullptr, elem->Name(), atts.data());
			parseIntrusive(elem);
			_delegator->endElement(nullptr, elem->Name());
		}
		else if (auto* text = child->ToText())
		{
			const char* s = text->Value();
			if (s) {
				// ★ 修复 3：更严格的字符串处理（过滤首尾空白，但保留中间的）
				std::string str(s);
				str.erase(0, str.find_first_not_of(" \n\r\t"));
				str.erase(str.find_last_not_of(" \n\r\t") + 1);

				if (!str.empty()) {
					_delegator->textHandler(nullptr, str.c_str(), str.length());
				}
			}
		}
	}
	return true;
}

void SAXParser::startElement(void *ctx, const OG_XML_CHAR *name, const OG_XML_CHAR **atts)
{
    ((SAXParser*)(ctx))->_delegator->startElement(ctx, (char*)name, (const char**)atts);
}

void SAXParser::endElement(void *ctx, const OG_XML_CHAR *name)
{
    ((SAXParser*)(ctx))->_delegator->endElement(ctx, (char*)name);
}
void SAXParser::textHandler(void *ctx, const OG_XML_CHAR *name, size_t len)
{
    ((SAXParser*)(ctx))->_delegator->textHandler(ctx, (char*)name, len);
}
void SAXParser::setDelegator(SAXDelegator* delegator)
{
    _delegator = delegator;
}

OG_END



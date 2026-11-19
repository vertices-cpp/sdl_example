
#include "platform/OGFileUtils.h"
#include "base/ogMacros.h"
// #include "unzip.h" 
// #include "utils/ZipTableManager.h"

#include "utils/iconv_wrapper.h"
#include "tinyxml2.h"

#include <algorithm>
#include <regex>
#include <stack>


#if defined(_WIN32)

#include <windows.h>
#include <shlobj.h>
#elif defined(__ANDROID__) ||defined(__linux__)
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>
#endif 

#define DECLARE_GUARD std::lock_guard<std::recursive_mutex> mutexGuard(_mutex)

OG_BEGIN

FileUtils* FileUtils::s_sharedFileUtils = nullptr;




//-------------------------------------FileUtils--------------------------
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
#if (OG_TARGET_PLATFORM != OG_PLATFORM_IOS) && (OG_TARGET_PLATFORM != OG_PLATFORM_MAC)

typedef enum
{
	SAX_NONE = 0,
	SAX_KEY,
	SAX_DICT,
	SAX_INT,
	SAX_REAL,
	SAX_STRING,
	SAX_ARRAY
}SAXState;

typedef enum
{
	SAX_RESULT_NONE = 0,
	SAX_RESULT_DICT,
	SAX_RESULT_ARRAY
}SAXResult;

class DictMaker : public SAXDelegator
{
public:
	SAXResult _resultType;
	ValueMap _rootDict;
	ValueVector _rootArray;

	std::string _curKey;   ///< parsed key
	std::string _curValue; // parsed value
	SAXState _state;

	ValueMap*  _curDict;
	ValueVector* _curArray;

	std::stack<ValueMap*> _dictStack;
	std::stack<ValueVector*> _arrayStack;
	std::stack<SAXState>  _stateStack;

public:
	DictMaker()
		: _resultType(SAX_RESULT_NONE)
		, _state(SAX_NONE)
	{
	}

	~DictMaker()
	{
	}

	ValueMap dictionaryWithContentsOfFile(const std::string& fileName)
	{
		_resultType = SAX_RESULT_DICT;
		SAXParser parser;

		OGASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
		parser.setDelegator(this);

		parser.parse(fileName);
		return _rootDict;
	}

	ValueMap dictionaryWithDataOfFile(const char* filedata, int filesize)
	{
		_resultType = SAX_RESULT_DICT;
		SAXParser parser;

		OGASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
		parser.setDelegator(this);

		parser.parse(filedata, filesize);
		return _rootDict;
	}

	ValueVector arrayWithContentsOfFile(const std::string& fileName)
	{
		_resultType = SAX_RESULT_ARRAY;
		SAXParser parser;

		OGASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
		parser.setDelegator(this);

		parser.parse(fileName);
		return _rootArray;
	}

	void startElement(void *ctx, const char *name, const char **atts) override
	{
		const std::string sName(name);
		if (sName == "dict")
		{
			if (_resultType == SAX_RESULT_DICT && _rootDict.empty())
			{
				_curDict = &_rootDict;
			}

			_state = SAX_DICT;

			SAXState preState = SAX_NONE;
			if (!_stateStack.empty())
			{
				preState = _stateStack.top();
			}

			if (SAX_ARRAY == preState)
			{
				// add a new dictionary into the array
				_curArray->push_back(Value(ValueMap()));
				_curDict = &(_curArray->rbegin())->asValueMap();
			}
			else if (SAX_DICT == preState)
			{
				// add a new dictionary into the pre dictionary
				OGASSERT(!_dictStack.empty(), "The state is wrong!");
				ValueMap* preDict = _dictStack.top();
				(*preDict)[_curKey] = Value(ValueMap());
				_curDict = &(*preDict)[_curKey].asValueMap();
			}

			// record the dict state
			_stateStack.push(_state);
			_dictStack.push(_curDict);
		}
		else if (sName == "key")
		{
			_state = SAX_KEY;
		}
		else if (sName == "integer")
		{
			_state = SAX_INT;
		}
		else if (sName == "real")
		{
			_state = SAX_REAL;
		}
		else if (sName == "string")
		{
			_state = SAX_STRING;
		}
		else if (sName == "array")
		{
			_state = SAX_ARRAY;

			if (_resultType == SAX_RESULT_ARRAY && _rootArray.empty())
			{
				_curArray = &_rootArray;
			}
			SAXState preState = SAX_NONE;
			if (!_stateStack.empty())
			{
				preState = _stateStack.top();
			}

			if (preState == SAX_DICT)
			{
				(*_curDict)[_curKey] = Value(ValueVector());
				_curArray = &(*_curDict)[_curKey].asValueVector();
			}
			else if (preState == SAX_ARRAY)
			{
				OGASSERT(!_arrayStack.empty(), "The state is wrong!");
				ValueVector* preArray = _arrayStack.top();
				preArray->push_back(Value(ValueVector()));
				_curArray = &(_curArray->rbegin())->asValueVector();
			}
			// record the array state
			_stateStack.push(_state);
			_arrayStack.push(_curArray);
		}
		else
		{
			_state = SAX_NONE;
		}
	}

	void endElement(void *ctx, const char *name) override
	{
		SAXState curState = _stateStack.empty() ? SAX_DICT : _stateStack.top();
		const std::string sName((char*)name);
		if (sName == "dict")
		{
			_stateStack.pop();
			_dictStack.pop();
			if (!_dictStack.empty())
			{
				_curDict = _dictStack.top();
			}
		}
		else if (sName == "array")
		{
			_stateStack.pop();
			_arrayStack.pop();
			if (!_arrayStack.empty())
			{
				_curArray = _arrayStack.top();
			}
		}
		else if (sName == "true")
		{
			if (SAX_ARRAY == curState)
			{
				_curArray->push_back(Value(true));
			}
			else if (SAX_DICT == curState)
			{
				(*_curDict)[_curKey] = Value(true);
			}
		}
		else if (sName == "false")
		{
			if (SAX_ARRAY == curState)
			{
				_curArray->push_back(Value(false));
			}
			else if (SAX_DICT == curState)
			{
				(*_curDict)[_curKey] = Value(false);
			}
		}
		else if (sName == "string" || sName == "integer" || sName == "real")
		{
			if (SAX_ARRAY == curState)
			{
				if (sName == "string")
					_curArray->push_back(Value(_curValue));
				else if (sName == "integer")
					_curArray->push_back(Value(atoi(_curValue.c_str())));
				else
					_curArray->push_back(Value(std::atof(_curValue.c_str())));
			}
			else if (SAX_DICT == curState)
			{
				if (sName == "string")
					(*_curDict)[_curKey] = Value(_curValue);
				else if (sName == "integer")
					(*_curDict)[_curKey] = Value(atoi(_curValue.c_str()));
				else
					(*_curDict)[_curKey] = Value(std::atof(_curValue.c_str()));
			}

			_curValue.clear();
		}

		_state = SAX_NONE;
	}

	void textHandler(void *ctx, const char *ch, size_t len) override
	{
		if (_state == SAX_NONE)
		{
			return;
		}

		SAXState curState = _stateStack.empty() ? SAX_DICT : _stateStack.top();
		const std::string text = std::string((char*)ch, len);

		switch (_state)
		{
		case SAX_KEY:
			_curKey = text;
			break;
		case SAX_INT:
		case SAX_REAL:
		case SAX_STRING:
		{
			if (curState == SAX_DICT)
			{
				OGASSERT(!_curKey.empty(), "key not found : <integer/real>");
			}

			_curValue.append(text);
		}
		break;
		default:
			break;
		}
	}
};

ValueMap FileUtils::getValueMapFromFile(const std::string& filename) const
{
	const std::string fullPath = fullPathForFilename(filename);
	DictMaker tMaker;
	return tMaker.dictionaryWithContentsOfFile(fullPath);
}

ValueMap FileUtils::getValueMapFromData(const char* filedata, int filesize) const
{
	DictMaker tMaker;
	return tMaker.dictionaryWithDataOfFile(filedata, filesize);
}

ValueVector FileUtils::getValueVectorFromFile(const std::string& filename) const
{
	const std::string fullPath = fullPathForFilename(filename);
	DictMaker tMaker;
	return tMaker.arrayWithContentsOfFile(fullPath);
}

#else

/* The subclass FileUtilsApple should override these two method. */
ValueMap FileUtils::getValueMapFromFile(const std::string& /*filename*/) const { return ValueMap(); }
ValueMap FileUtils::getValueMapFromData(const char* /*filedata*/, int /*filesize*/) const { return ValueMap(); }
ValueVector FileUtils::getValueVectorFromFile(const std::string& /*filename*/) const { return ValueVector(); }
bool FileUtils::writeToFile(const ValueMap& /*dict*/, const std::string &/*fullPath*/) const { return false; }

#endif /* (OG_TARGET_PLATFORM != OG_PLATFORM_IOS) && (OG_TARGET_PLATFORM != OG_PLATFORM_MAC) */
//获取绝对路径
static void _checkPath()
{
#if defined(_WIN32)
	if (s_resourcePath.empty())
	{
		WCHAR utf16Path[OG_MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, utf16Path, OG_MAX_PATH - 1);
		WCHAR *pUtf16ExePath = &(utf16Path[0]);

		// We need only directory part without exe
		WCHAR *pUtf16DirEnd = wcsrchr(pUtf16ExePath, L'\\');

		char utf8ExeDir[OG_MAX_PATH] = { 0 };
		int nNum = WideCharToMultiByte(CP_UTF8, 0, pUtf16ExePath, pUtf16DirEnd - pUtf16ExePath + 1, utf8ExeDir, sizeof(utf8ExeDir), nullptr, nullptr);

		s_resourcePath = convertPathFormatToUnixStyle(utf8ExeDir);
	}
#elif defined(__ANDROID__)
	char fullpath[256] = { 0 };
	ssize_t length = readlink("/proc/self/exe", fullpath, sizeof(fullpath) - 1);

	if (length <= 0) {
		return;
	}

	fullpath[length] = '\0';
	std::string appPath = fullpath;
	s_resourcePath = appPath.substr(0, appPath.find_last_of('/') + 1);

#endif
}


//转UTF8
std::wstring StringUtf8ToWideChar(const std::string& strUtf8)
{
	if (strUtf8.empty()) return {};

	// 1. UTF-8 → UTF-32
	std::u32string u32 = iconv_wrapper_to_utf32le(strUtf8, Utf8ToUtf32le);
	if (u32.empty()) return {};

	std::wstring ret;

#if defined(_WIN32)
	// Windows：wchar_t = UTF-16
	for (char32_t cp : u32)
	{
		if (cp < 0x10000)
		{
			// BMP
			ret.push_back((wchar_t)cp);
		}
		else
		{
			// 补充平面：转成代理对
			cp -= 0x10000;
			ret.push_back((wchar_t)(0xD800 + (cp >> 10)));
			ret.push_back((wchar_t)(0xDC00 + (cp & 0x3FF)));
		}
	}
#else
	// Linux / macOS：wchar_t = UTF-32
	for (char32_t cp : u32)
		ret.push_back((wchar_t)cp);
#endif

	return ret;
}

std::string StringWideCharToUtf8(const std::wstring& strWide)
{
	if (strWide.empty()) return {};

#if defined(_WIN32)
	const char* bytes = reinterpret_cast<const char*>(strWide.data());
	size_t byteLen = strWide.size() * sizeof(wchar_t);
	std::string in(bytes, byteLen);
	return iconv_wrapper(in, Utf16leToUtf8);   // ← 字节流转换
#else
	std::u32string u32(strWide.begin(), strWide.end());
	return iconv_wrapper_from_u32le(u32, Utf32leToUtf8);
#endif
}
std::string UTF8StringToMultiByte(const std::string& strUtf8)
{
#if defined(_WIN32)
	return iconv_wrapper(strUtf8, Utf8ToGbk);
#else
	return strUtf8;
#endif
}


bool FileUtils::init()
{
	_checkPath();
	_defaultResRootPath = convertPathFormatToUnixStyle(s_resourcePath) + "Resources/";
	_searchPathArray.push_back(_defaultResRootPath);
	//加入路径 
	_searchResolutionsOrderArray.push_back("");

	return true;
}

//判断是否是绝对路径
bool FileUtils::isAbsolutePath(const std::string& strPath) const
{
	if (strPath.empty()) return false;

#if defined(_WIN32)
	// 盘符 C:\ 或 C:/
	if (strPath.size() >= 2 &&
		((strPath[0] >= 'a' && strPath[0] <= 'z') ||
		(strPath[0] >= 'A' && strPath[0] <= 'Z')) &&
		strPath[1] == ':')
	{
		return true;
	}
	// Windows UNC \\server
	if (strPath.size() >= 2 && strPath[0] == '\\' && strPath[1] == '\\')
	{
		return true;
	}
#endif

	// 类 Unix / Android：以 / 开头就是绝对路径
	if (strPath[0] == '/')
	{
		return true;
	}

	return false;
}

//检测文件状态
bool FileUtils::isFileExistInternal(const std::string& strFilePath)const
{

	if (strFilePath.empty())
	{
		return false;
	}

	std::string strPath = strFilePath;
	if (!isAbsolutePath(strPath))
	{ // Not absolute path, add the default root path at the beginning.
		strPath.insert(0, _defaultResRootPath);
	}
	// 尝试以二进制只读方式打开
	//FILE *fp = fopen(strPath.c_str(), "rb");
	FILE* fp = fopen(getSuitableFOpen(strPath).c_str(), "rb");
	if (fp)
	{
		fclose(fp);
		return true;
	}
	// 	DWORD attr = GetFileAttributesW(StringUtf8ToWideChar(strPath).c_str());
	// 	if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
	// 		return false;   //  not a file

	return false;
}

bool FileUtils::isFileExist(const std::string& filename) const
{
// 	// 1. 查 ZIP
// 	std::string key = convertPathFormatToUnixStyle(filename);
// 	if (ZipTableManager::getInstance()->has(key))
// 		return true;

	if (isAbsolutePath(filename))
	{
		return isFileExistInternal(filename);
	}
	else
	{
		std::string fullpath = fullPathForFilename(filename);
		if (fullpath.empty())
			return false;
		else
			return true;
	}
}

std::string FileUtils::getFullPathForFilenameWithinDirectory(const std::string& directory, const std::string& filename)const
{
	// get directory+filename, safely adding '/' as necessary
	std::string ret = directory;
	if (directory.size() && directory[directory.size() - 1] != '/') {
		ret += '/';
	}
	ret += filename;
	// if the file doesn't exist, return an empty string
	if (!isFileExistInternal(ret)) {
		ret = "";
	}
	return ret;
}

std::string FileUtils::getPathForFilename(const std::string& filename, const std::string & resource_path)const
{
	std::string file = filename;
	std::string file_path = "";
	size_t pos = filename.find_last_of('/');
	if (pos != std::string::npos)
	{
		file_path = filename.substr(0, pos + 1);
		file = filename.substr(pos + 1);
	}

	// searchPath + file_path + resourceDirectory
	std::string path = resource_path;
	path += file_path;


	path = getFullPathForFilenameWithinDirectory(path, file);

	return path;
}

std::string FileUtils::fullPathForFilename(const std::string &filename)const
{


	if (filename.empty())
	{
		return "";
	}

// 	// ★ 先查 ZIP
// 	std::string key = convertPathFormatToUnixStyle(filename);
// 	if (ZipTableManager::getInstance()->has(key))
// 		return key;   // ZIP 里，返回逻辑路径
// 

	if (isAbsolutePath(filename))
	{
		return filename;
	}

	// Already Cached ?
	auto cacheIter = _fullPathCache.find(filename);
	if (cacheIter != _fullPathCache.end())
	{
		return cacheIter->second;
	}
	// Get the new file name.
	const std::string newFilename(getNewFilename(filename));

	std::string fullpath;

	for (const auto& searchIt : _searchPathArray)
	{
		for (const auto& resolutionIt : _searchResolutionsOrderArray) {
			std::string fullpath = this->getPathForFilename(filename, searchIt);
			if (!fullpath.empty())
			{
				_fullPathCache.emplace(std::make_pair(filename, fullpath));
				return fullpath;
			}
		}
	}

	return "";
}

std::string FileUtils::getNewFilename(const std::string &filename) const
{
	std::string newFileName;


	// in Lookup Filename dictionary ?
	auto iter = _filenameLookupDict.find(filename);

	if (iter == _filenameLookupDict.end())
	{
		newFileName = filename;
	}
	else
	{
		newFileName = iter->second.asString();
	}
	return newFileName;
}
std::string FileUtils::fullPathFromRelativeFile(const std::string &filename, const std::string &relativeFile) const
{
	return relativeFile.substr(0, relativeFile.rfind('/') + 1) + getNewFilename(filename);
}


Data FileUtils::getDataFromFile(const std::string& filename) const
{
	Data d;
	getContents(filename, &d);
	return d;
}
std::string FileUtils::getStringFromFile(const std::string& filename) const
{
	std::string s;
	getContents(filename, &s);
	return s;
}


FileUtils::Status FileUtils::getContents(const std::string& filename, ResizableBuffer* buffer) const
{
	if (filename.empty())
		return Status::NotExists;

	// ===== 1. 先查 ZIP =====
	std::string key = convertPathFormatToUnixStyle(filename);
// 	Data zipData;
// 	if (ZipTableManager::getInstance()->read(key, zipData))
// 	{
// 		buffer->resize(zipData.getSize());
// 		memcpy(buffer->buffer(), zipData.getBytes(), zipData.getSize());
// 		return Status::OK;
// 	}


	auto fs = FileUtils::getInstance();

	std::string fullPath = fs->fullPathForFilename(filename);
	if (fullPath.empty())
		return Status::NotExists;

	std::string suitableFullPath = fs->getSuitableFOpen(fullPath);

	struct stat statBuf;
	if (stat(suitableFullPath.c_str(), &statBuf) == -1) {
		return Status::ReadFailed;
	}

	if (!(statBuf.st_mode & S_IFREG)) {
		return Status::NotRegularFileType;
	}

	FILE *fp = fopen(suitableFullPath.c_str(), "rb");
	if (!fp)
		return Status::OpenFailed;

	size_t size = statBuf.st_size;

	buffer->resize(size);
	size_t readsize = fread(buffer->buffer(), 1, size, fp);
	fclose(fp);

	if (readsize < size) {
		buffer->resize(readsize);
		return Status::ReadFailed;
	}

	return Status::OK;
}

std::string FileUtils::fullPathForDirectory(const std::string &dir) const
{

	if (dir.empty())
	{
		return "";
	}

	if (isAbsolutePath(dir))
	{
		return dir;
	}

	// Already Cached ?
	auto cacheIter = _fullPathCacheDir.find(dir);
	if (cacheIter != _fullPathCacheDir.end())
	{
		return cacheIter->second;
	}
	std::string longdir = dir;
	std::string fullpath;

	if (longdir[longdir.length() - 1] != '/')
	{
		longdir += "/";
	}

	const std::string newdirname(getNewFilename(longdir));

	for (const auto& searchIt : _searchPathArray)
	{
		for (const auto& resolutionIt : _searchResolutionsOrderArray)
		{
			fullpath = this->getPathForDirectory(newdirname, resolutionIt, searchIt);
			if (!fullpath.empty() && isDirectoryExistInternal(fullpath))
			{
				// Using the filename passed in as key.
				_fullPathCacheDir.emplace(dir, fullpath);
				return fullpath;
			}

		}
	}

	if (isPopupNotify()) {
		OGLOG("orange: fullPathForDirectory: No directory found at %s. Possible missing directory.", dir.c_str());
	}

	// The file wasn't found, return empty string.
	return "";
}

std::string FileUtils::getFileExtension(const std::string& filePath) const
{
	std::string fileExtension;
	size_t pos = filePath.find_last_of('.');
	if (pos != std::string::npos)
	{
		fileExtension = filePath.substr(pos, filePath.length());

		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
	}

	return fileExtension;
}

void FileUtils::setWritablePath(const std::string& writablePath)
{
	_writablePath = writablePath;
}



//---------------回调部分----------------


const std::vector<std::string> FileUtils::getSearchPaths() const
{
	return _searchPathArray;
}
std::string FileUtils::getPathForDirectory(const std::string &dir, const std::string &resolutionDiretory, const std::string &searchPath) const
{
	return searchPath + resolutionDiretory + dir;
}

static bool s_popupNotify = true;

void FileUtils::setPopupNotify(bool notify)
{
	s_popupNotify = notify;
}

bool FileUtils::isPopupNotify() const
{
	return s_popupNotify;
}

bool FileUtils::isDirectoryExist(const std::string& dirPath) const
{
	OGASSERT(!dirPath.empty(), "Invalid path");


	if (isAbsolutePath(dirPath))
	{
		return isDirectoryExistInternal(dirPath);
	}
	else {
		auto fullPath = fullPathForDirectory(dirPath);
		return !fullPath.empty();
	}
}



void FileUtils::addSearchPath(const std::string &searchpath, const bool front)
{

	std::string prefix;
	if (!isAbsolutePath(searchpath))
		prefix = _defaultResRootPath;

	std::string path = prefix + searchpath;
	if (!path.empty() && path[path.length() - 1] != '/')
	{
		path += "/";
	}

	if (front) {
		_originalSearchPaths.insert(_originalSearchPaths.begin(), searchpath);
		_searchPathArray.insert(_searchPathArray.begin(), path);
	}
	else {
		_originalSearchPaths.push_back(searchpath);
		_searchPathArray.push_back(path);
	}
}






//调用线程


void FileUtils::isDirectoryExist(const std::string& fullPath, std::function<void(bool)> callback) const
{
	OGASSERT(isAbsolutePath(fullPath), "Async isDirectoryExist only accepts absolute file paths");
	performOperationOffthread([fullPath]() -> bool {
		return FileUtils::getInstance()->isDirectoryExist(fullPath);
	}, std::move(callback));
}

void FileUtils::createDirectory(const std::string& dirPath, std::function<void(bool)> callback) const
{
	performOperationOffthread([dirPath]() -> bool {
		return FileUtils::getInstance()->createDirectory(dirPath);
	}, std::move(callback));
}

void FileUtils::removeDirectory(const std::string& dirPath, std::function<void(bool)> callback) const
{
	performOperationOffthread([dirPath]() -> bool {
		return FileUtils::getInstance()->removeDirectory(dirPath);
	}, std::move(callback));
}

void FileUtils::removeFile(const std::string &filepath, std::function<void(bool)> callback) const
{
	auto fullPath = fullPathForFilename(filepath);
	performOperationOffthread([fullPath]() -> bool {
		return FileUtils::getInstance()->removeFile(fullPath);
	}, std::move(callback));
}

void FileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name, std::function<void(bool)> callback) const
{
	performOperationOffthread([path, oldname, name]() -> bool {
		return FileUtils::getInstance()->renameFile(path, oldname, name);
	}, std::move(callback));

}

void FileUtils::renameFile(const std::string &oldfullpath, const std::string &newfullpath, std::function<void(bool)> callback) const
{
	performOperationOffthread([oldfullpath, newfullpath]() {
		return FileUtils::getInstance()->renameFile(oldfullpath, newfullpath);
	}, std::move(callback));
}

void FileUtils::getFileSize(const std::string &filepath, std::function<void(long)> callback) const
{
	auto fullPath = fullPathForFilename(filepath);
	performOperationOffthread([fullPath]() {
		return FileUtils::getInstance()->getFileSize(fullPath);
	}, std::move(callback));
}

void FileUtils::listFilesAsync(const std::string& dirPath, std::function<void(std::vector<std::string>)> callback) const
{
	auto fullPath = fullPathForDirectory(dirPath);
	performOperationOffthread([fullPath]() {
		return FileUtils::getInstance()->listFiles(fullPath);
	}, std::move(callback));
}

void FileUtils::listFilesRecursivelyAsync(const std::string& dirPath, std::function<void(std::vector<std::string>)> callback) const
{
	auto fullPath = fullPathForDirectory(dirPath);
	performOperationOffthread([fullPath]() {
		std::vector<std::string> retval;
		FileUtils::getInstance()->listFilesRecursively(fullPath, &retval);
		return retval;
	}, std::move(callback));
}

void FileUtils::isFileExist(const std::string& filename, std::function<void(bool)> callback) const
{
	auto fullPath = fullPathForFilename(filename);
	performOperationOffthread([fullPath]() -> bool {
		return FileUtils::getInstance()->isFileExist(fullPath);
	}, std::move(callback));
}



static tinyxml2::XMLElement* generateElementForArray(const ValueVector& array, tinyxml2::XMLDocument *doc);
static tinyxml2::XMLElement* generateElementForDict(const ValueMap& dict, tinyxml2::XMLDocument *doc);


bool FileUtils::writeToFile(const ValueMap& dict, const std::string &fullPath) const
{
	return writeValueMapToFile(dict, fullPath);
}


/*
 * Generate tinyxml2::XMLElement for Object through a tinyxml2::XMLDocument
 */
static tinyxml2::XMLElement* generateElementForObject(const Value& value, tinyxml2::XMLDocument *doc)
{
	// object is String
	if (value.getType() == Value::Type::STRING)
	{
		tinyxml2::XMLElement* node = doc->NewElement("string");
		tinyxml2::XMLText* content = doc->NewText(value.asString().c_str());
		node->LinkEndChild(content);
		return node;
	}

	// object is integer
	if (value.getType() == Value::Type::INTEGER)
	{
		tinyxml2::XMLElement* node = doc->NewElement("integer");
		tinyxml2::XMLText* content = doc->NewText(value.asString().c_str());
		node->LinkEndChild(content);
		return node;
	}

	// object is real
	if (value.getType() == Value::Type::FLOAT || value.getType() == Value::Type::DOUBLE)
	{
		tinyxml2::XMLElement* node = doc->NewElement("real");
		tinyxml2::XMLText* content = doc->NewText(value.asString().c_str());
		node->LinkEndChild(content);
		return node;
	}

	//object is bool
	if (value.getType() == Value::Type::BOOLEAN) {
		tinyxml2::XMLElement* node = doc->NewElement(value.asString().c_str());
		return node;
	}

	// object is Array
	if (value.getType() == Value::Type::VECTOR)
		return generateElementForArray(value.asValueVector(), doc);

	// object is Dictionary
	if (value.getType() == Value::Type::MAP)
		return generateElementForDict(value.asValueMap(), doc);

	OGLOG("This type cannot appear in property list");
	return nullptr;
}

/*
 * Generate tinyxml2::XMLElement for Dictionary through a tinyxml2::XMLDocument
 */
static tinyxml2::XMLElement* generateElementForDict(const ValueMap& dict, tinyxml2::XMLDocument *doc)
{
	tinyxml2::XMLElement* rootNode = doc->NewElement("dict");

	for (const auto &iter : dict)
	{
		tinyxml2::XMLElement* tmpNode = doc->NewElement("key");
		rootNode->LinkEndChild(tmpNode);
		tinyxml2::XMLText* content = doc->NewText(iter.first.c_str());
		tmpNode->LinkEndChild(content);

		tinyxml2::XMLElement *element = generateElementForObject(iter.second, doc);
		if (element)
			rootNode->LinkEndChild(element);
	}
	return rootNode;
}

/*
 * Generate tinyxml2::XMLElement for Array through a tinyxml2::XMLDocument
 */
static tinyxml2::XMLElement* generateElementForArray(const ValueVector& array, tinyxml2::XMLDocument *pDoc)
{
	tinyxml2::XMLElement* rootNode = pDoc->NewElement("array");

	for (const auto &value : array) {
		tinyxml2::XMLElement *element = generateElementForObject(value, pDoc);
		if (element)
			rootNode->LinkEndChild(element);
	}
	return rootNode;
}


bool FileUtils::writeValueMapToFile(const ValueMap& dict, const std::string& fullPath) const
{
	tinyxml2::XMLDocument *doc = new (std::nothrow)tinyxml2::XMLDocument();
	if (nullptr == doc)
		return false;

	tinyxml2::XMLDeclaration *declaration = doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
	if (nullptr == declaration)
	{
		delete doc;
		return false;
	}

	doc->LinkEndChild(declaration);
	tinyxml2::XMLElement *docType = doc->NewElement("!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
	doc->LinkEndChild(docType);

	tinyxml2::XMLElement *rootEle = doc->NewElement("plist");
	if (nullptr == rootEle)
	{
		delete doc;
		return false;
	}
	rootEle->SetAttribute("version", "1.0");
	doc->LinkEndChild(rootEle);

	tinyxml2::XMLElement *innerDict = generateElementForDict(dict, doc);
	if (nullptr == innerDict)
	{
		delete doc;
		return false;
	}
	rootEle->LinkEndChild(innerDict);

	bool ret = tinyxml2::XML_SUCCESS == doc->SaveFile(getSuitableFOpen(fullPath).c_str());

	delete doc;
	return ret;
}
bool FileUtils::writeStringToFile(const std::string& dataStr, const std::string& fullPath) const
{
	Data data;
	data.fastSet((unsigned char*)dataStr.c_str(), dataStr.size());

	bool rv = writeDataToFile(data, fullPath);

	data.fastSet(nullptr, 0);
	return rv;
}
bool FileUtils::writeDataToFile(const Data& data, const std::string& fullPath) const
{
	size_t size = 0;
	const char* mode = "wb";

	OGASSERT(!fullPath.empty() && data.getSize() != 0, "Invalid parameters.");

	auto fileutils = FileUtils::getInstance();
	do
	{
		// Read the file from hardware
		FILE *fp = fopen(fileutils->getSuitableFOpen(fullPath).c_str(), mode);
		OG_BREAK_IF(!fp);
		size = data.getSize();

		fwrite(data.getBytes(), size, 1, fp);

		fclose(fp);

		return true;
	} while (0);

	return false;
}
void FileUtils::purgeCachedEntries()
{

	_fullPathCache.clear();
	_fullPathCacheDir.clear();
}

void FileUtils::setSearchPaths(const std::vector<std::string>& searchPaths)
{
	DECLARE_GUARD;
	bool existDefaultRootPath = false;
	_originalSearchPaths = searchPaths;

	_fullPathCache.clear();
	_fullPathCacheDir.clear();
	_searchPathArray.clear();

	for (const auto& path : _originalSearchPaths)
	{
		std::string prefix;
		std::string fullPath;

		if (!isAbsolutePath(path))
		{ // Not an absolute path
			prefix = _defaultResRootPath;
		}
		fullPath = prefix + path;
		if (!path.empty() && path[path.length() - 1] != '/')
		{
			fullPath += "/";
		}
		if (!existDefaultRootPath && path == _defaultResRootPath)
		{
			existDefaultRootPath = true;
		}
		_searchPathArray.push_back(fullPath);
	}

	if (!existDefaultRootPath)
	{
		//CCLOG("Default root path doesn't exist, adding it.");
		_searchPathArray.push_back(_defaultResRootPath);
	}
}

//-------------平台分类API-------------
#if defined(_WIN32)||defined(_WIN64)


bool FileUtils::isDirectoryExistInternal(const std::string& dirPath) const
{
	unsigned long fAttrib = GetFileAttributes(StringUtf8ToWideChar(dirPath).c_str());
	if (fAttrib != INVALID_FILE_ATTRIBUTES &&
		(fAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		return true;
	}
	return false;
}

std::string FileUtils::getWritablePath() const
{

	if (_writablePath.length())
	{
		return _writablePath;
	}

	// Get full path of executable, e.g. c:\Program Files (x86)\My Game Folder\MyGame.exe
	WCHAR full_path[512 + 1] = { 0 };
	::GetModuleFileName(nullptr, full_path, 512 + 1);

	// Debug app uses executable directory; Non-debug app uses local app data directory
//#ifndef _DEBUG
	// Get filename of executable only, e.g. MyGame.exe
	WCHAR *base_name = wcsrchr(full_path, '\\');
	std::wstring retPath;
	if (base_name)
	{
		WCHAR app_data_path[512 + 1];

		// Get local app data directory, e.g. C:\Documents and Settings\username\Local Settings\Application Data
		if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, app_data_path)))
		{
			std::wstring ret(app_data_path);

			// Adding executable filename, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame.exe
			ret += base_name;

			// Remove ".exe" extension, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame
			ret = ret.substr(0, ret.rfind(L"."));

			ret += L"\\";

			// Create directory
			if (SUCCEEDED(SHCreateDirectoryEx(nullptr, ret.c_str(), nullptr)))
			{
				retPath = ret;
			}
		}
	}
	if (retPath.empty())
		//#endif // not defined _DEBUG
	{
		// If fetching of local app data directory fails, use the executable one
		retPath = full_path;

		// remove xxx.exe
		retPath = retPath.substr(0, retPath.rfind(L"\\") + 1);
	}

	return convertPathFormatToUnixStyle(StringWideCharToUtf8(retPath));
}


bool FileUtils::createDirectory(const std::string& dirPath) const
{
	OGASSERT(!dirPath.empty(), "Invalid path");

	if (isDirectoryExist(dirPath))
		return true;

	std::wstring path = StringUtf8ToWideChar(dirPath);

	// Split the path
	size_t start = 0;
	size_t found = path.find_first_of(L"/\\", start);
	std::wstring subpath;
	std::vector<std::wstring> dirs;

	if (found != std::wstring::npos)
	{
		while (true)
		{
			subpath = path.substr(start, found - start + 1);
			if (!subpath.empty())
				dirs.push_back(subpath);
			start = found + 1;
			found = path.find_first_of(L"/\\", start);
			if (found == std::wstring::npos)
			{
				if (start < path.length())
				{
					dirs.push_back(path.substr(start));
				}
				break;
			}
		}
	}

	if ((GetFileAttributes(path.c_str())) == INVALID_FILE_ATTRIBUTES)
	{
		subpath = L"";
		for (unsigned int i = 0, size = dirs.size(); i < size; ++i)
		{
			subpath += dirs[i];

			std::string utf8Path = StringWideCharToUtf8(subpath);
			if (!isDirectoryExist(utf8Path))
			{
				BOOL ret = CreateDirectory(subpath.c_str(), NULL);
				if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
				{
					OGLOGERROR("Fail create directory %s !Error code is 0x%x", utf8Path.c_str(), GetLastError());
					return false;
				}
			}
		}
	}
	return true;
}
bool FileUtils::removeDirectory(const std::string& dirPath) const
{
	std::string dirPathCopy = dirPath;
	if (dirPath.length() > 0 && dirPath[dirPath.length() - 1] != '/' && dirPath[dirPath.length() - 1] != '\\')
	{
		dirPathCopy.append("/");
	}
	std::wstring wpath = StringUtf8ToWideChar(dirPathCopy);
	std::wstring files = wpath + L"*.*";
	WIN32_FIND_DATA wfd;
	HANDLE  search = FindFirstFileEx(files.c_str(), FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
	bool ret = true;
	if (search != INVALID_HANDLE_VALUE)
	{
		BOOL find = true;
		while (find)
		{
			// Need check string . and .. for delete folders and files begin name.
			std::wstring fileName = wfd.cFileName;
			if (fileName != L"." && fileName != L"..")
			{
				std::wstring temp = wpath + wfd.cFileName;
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					temp += '/';
					ret = ret && this->removeDirectory(StringWideCharToUtf8(temp));
				}
				else
				{
					SetFileAttributes(temp.c_str(), FILE_ATTRIBUTE_NORMAL);
					ret = ret && DeleteFile(temp.c_str());
				}
			}
			find = FindNextFile(search, &wfd);
		}
		FindClose(search);
	}
	if (ret && RemoveDirectory(wpath.c_str()))
	{
		return true;
	}
	return false;
}
bool FileUtils::removeFile(const std::string& filepath) const
{
	OGASSERT(!filepath.empty(), "Invalid path");
	std::string newPath = convertPathFormatToUnixStyle(filepath);
	if (std::remove(newPath.c_str()) == 0)
	{
		return true;
	}
	else
	{
		OGLOGERROR("Fail to remove file %s, errno=%d",
			newPath.c_str(), errno);
		return false;
	}
}
bool FileUtils::renameFile(const std::string& oldfullpath, const std::string& newfullpath) const
{
	OGASSERT(!oldfullpath.empty(), "Invalid path");
	OGASSERT(!newfullpath.empty(), "Invalid path");


	std::string oldPath = convertPathFormatToUnixStyle(oldfullpath);
	std::string newPath = convertPathFormatToUnixStyle(newfullpath);
	// 如果新路径已存在，先删掉
	if (isFileExist(newPath))
	{
		if (std::remove(newPath.c_str()) != 0)
		{
			OGLOGERROR("Fail to remove existing file %s, errno=%d",
				newPath.c_str(), errno);
		}
	}

	if (std::rename(oldPath.c_str(), newPath.c_str()) == 0)
		return true;

	OGLOGERROR("Fail to rename file %s to %s, errno=%d",
		oldPath.c_str(), newPath.c_str(), errno);
	return false;
}
bool FileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name) const
{
	OGASSERT(!path.empty(), "Invalid path");
	std::string oldPath = path + oldname;
	std::string newPath = path + name;

	std::regex pat("\\/");
	std::string _old = std::regex_replace(oldPath, pat, "\\");
	std::string _new = std::regex_replace(newPath, pat, "\\");

	return renameFile(_old, _new);
}

std::string FileUtils::getSuitableFOpen(const std::string& filenameUtf8) const
{
	return UTF8StringToMultiByte(filenameUtf8);
}

std::vector<std::string> FileUtils::listFiles(const std::string& dirPath) const
{
	std::vector<std::string> files;
	std::string fullpath = fullPathForDirectory(dirPath);
	if (!isDirectoryExist(fullpath)) return files;


	std::wstring wpath = StringUtf8ToWideChar(fullpath);
	if (!wpath.empty() && wpath.back() != L'\\' && wpath.back() != L'/')
		wpath += L'\\';
	wpath += L'*';

	WIN32_FIND_DATAW fd;
	HANDLE h = FindFirstFileW(wpath.c_str(), &fd);
	if (h == INVALID_HANDLE_VALUE) return files;

	do {
		if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0)
			continue;

		std::wstring wname = fd.cFileName;
		std::string name = StringWideCharToUtf8(wname);

		std::string filepath = fullpath + name;
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			filepath += "/";

		files.push_back(filepath);
	} while (FindNextFileW(h, &fd));

	FindClose(h);



	return files;
}

// long FileUtils::getFileSize(const std::string &filepath)
// {
// 	WIN32_FILE_ATTRIBUTE_DATA fad;
// 	if (!GetFileAttributesEx(StringUtf8ToWideChar(filepath).c_str(), GetFileExInfoStandard, &fad))
// 	{
// 		return 0; // error condition, could call GetLastError to find out more
// 	}
// 	LARGE_INTEGER size;
// 	size.HighPart = fad.nFileSizeHigh;
// 	size.LowPart = fad.nFileSizeLow;
// 	return (long)size.QuadPart;
// }

long FileUtils::getFileSize(const std::string &filepath) const
{
	struct _stat tmp;
	if (_stat(filepath.c_str(), &tmp) == 0)
	{
		return (long)tmp.st_size;
	}
	return 0;
}

void FileUtils::listFilesRecursively(const std::string& dirPath,
	std::vector<std::string> *files) const
{
	std::string fullpath = fullPathForDirectory(dirPath);
	if (!isDirectoryExist(fullpath)) return;

	std::wstring wpath = StringUtf8ToWideChar(fullpath);
	if (wpath.empty()) return;
	if (wpath.back() != L'\\' && wpath.back() != L'/')
		wpath += L'\\';
	wpath += L'*';

	WIN32_FIND_DATAW fd;
	HANDLE h = FindFirstFileW(wpath.c_str(), &fd);
	if (h == INVALID_HANDLE_VALUE) return;

	do {
		if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0)
			continue;

		std::string name = StringWideCharToUtf8(fd.cFileName);

		std::string filepath = fullpath;
		if (!filepath.empty() && filepath.back() != '/')
			filepath += '/';
		filepath += name;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			filepath += "/";
			files->push_back(filepath);
			listFilesRecursively(filepath, files);   // 递归
		}
		else
		{
			files->push_back(filepath);
		}
	} while (FindNextFileW(h, &fd));

	FindClose(h);
}
#else
bool FileUtils::isDirectoryExistInternal(const std::string& dirPath) const
{
	struct stat st;
	if (stat(dirPath.c_str(), &st) == 0)
	{
		return S_ISDIR(st.st_mode);
	}
	return false;
}
std::string FileUtils::getWritablePath() const
{
	if (_writablePath.empty()) return "";

	struct stat st;
	if (stat(_writablePath.c_str(), &st) != 0 || !S_ISDIR(st.st_mode)) {
		createDirectory(_writablePath);   // ← 递归建
	}
	return _writablePath;
}

bool FileUtils::createDirectory(const std::string& path) const
{
	OGASSERT(!path.empty(), "Invalid path");

	if (isDirectoryExist(path))
		return true;

	// Split the path
	size_t start = 0;
	size_t found = path.find_first_of("/\\", start);
	std::string subpath;
	std::vector<std::string> dirs;

	if (found != std::string::npos)
	{
		while (true)
		{
			subpath = path.substr(start, found - start + 1);
			if (!subpath.empty())
				dirs.push_back(subpath);
			start = found + 1;
			found = path.find_first_of("/\\", start);
			if (found == std::string::npos)
			{
				if (start < path.length())
				{
					dirs.push_back(path.substr(start));
				}
				break;
			}
		}
	}

	DIR *dir = NULL;

	// Create path recursively
	subpath = "";
	for (const auto& iter : dirs)
	{
		subpath += iter;
		dir = opendir(subpath.c_str());

		if (!dir)
		{
			// directory doesn't exist, should create a new one

			int ret = mkdir(subpath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
			if (ret != 0 && (errno != EEXIST))
			{
				// current directory can not be created, sub directories can not be created too
				// should return
				return false;
			}
		}
		else
		{
			// directory exists, should close opened dir
			closedir(dir);
		}
	}
	return true;
}

namespace
{
#if (OG_TARGET_PLATFORM != OG_PLATFORM_ANDROID)
	int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
	{
		int rv = remove(fpath);

		if (rv)
			perror(fpath);

		return rv;
	}
#endif
}
bool FileUtils::removeDirectory(const std::string& path) const
{
#if !defined(OG_TARGET_OS_TVOS)

#if (OG_TARGET_PLATFORM != OG_PLATFORM_ANDROID)
	if (nftw(path.c_str(), unlink_cb, 64, FTW_DEPTH | FTW_PHYS) == -1)
		return false;
	else
		return true;
#else
	std::string command = "rm -r ";
	// Path may include space.
	command += "\"" + path + "\"";
	if (system(command.c_str()) >= 0)
		return true;
	else
		return false;
#endif // (OG_TARGET_PLATFORM != OG_PLATFORM_ANDROID)

#else
	return false;
#endif // !defined(OG_TARGET_OS_TVOS)
}

bool FileUtils::removeFile(const std::string &path) const
{
	if (remove(path.c_str())) {
		return false;
	}
	else {
		return true;
	}
}

bool FileUtils::renameFile(const std::string &oldfullpath, const std::string &newfullpath) const
{
	OGASSERT(!oldfullpath.empty(), "Invalid path");
	OGASSERT(!newfullpath.empty(), "Invalid path");

	int errorCode = rename(oldfullpath.c_str(), newfullpath.c_str());

	if (0 != errorCode)
	{
		OGLOGERROR("Fail to rename file %s to %s !Error code is %d", oldfullpath.c_str(), newfullpath.c_str(), errorCode);
		return false;
	}
	return true;
}

bool FileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name) const
{
	OGASSERT(!path.empty(), "Invalid path");
	std::string oldPath = path + oldname;
	std::string newPath = path + name;

	return this->renameFile(oldPath, newPath);
}

std::string FileUtils::getSuitableFOpen(const std::string& filenameUtf8) const
{
	return filenameUtf8;
}

long FileUtils::getFileSize(const std::string &filepath) const
{
	OGASSERT(!filepath.empty(), "Invalid path");

	std::string fullpath = filepath;
	if (!isAbsolutePath(filepath))
	{
		fullpath = fullPathForFilename(filepath);
		if (fullpath.empty())
			return 0;
	}

	struct stat info;
	// Get data associated with "crt_stat.c":
	int result = stat(fullpath.c_str(), &info);

	// Check if statistics are valid:
	if (result != 0)
	{
		// Failed
		return -1;
	}
	else
	{
		return (long)(info.st_size);
	}
}

std::vector<std::string> FileUtils::listFiles(const std::string& dirPath) const
{
	std::vector<std::string> files;
	std::string fullpath = fullPathForDirectory(dirPath);
	DIR* dir = opendir(fullpath.c_str());
	if (!dir) return files;

	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		std::string name = entry->d_name;
		if (name == "." || name == "..") continue;

		std::string filepath = fullpath + name;
		struct stat st;
		if (stat(filepath.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
			filepath += "/";

		files.push_back(filepath);
	}
	closedir(dir);
	return files;
}

void FileUtils::listFilesRecursively(const std::string& dirPath,
	std::vector<std::string> *files) const
{
	std::string fullpath = fullPathForDirectory(dirPath);
	if (!isDirectoryExist(fullpath)) return;

	DIR* dir = opendir(fullpath.c_str());
	if (!dir) return;

	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr)
	{
		std::string name = entry->d_name;
		if (name == "." || name == "..") continue;

		std::string filepath = fullpath;
		if (!filepath.empty() && filepath.back() != '/')
			filepath += '/';
		filepath += name;

		struct stat st;
		if (stat(filepath.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
		{
			filepath += "/";
			files->push_back(filepath);
			listFilesRecursively(filepath, files);   // 递归
		}
		else
		{
			files->push_back(filepath);
		}
	}

	closedir(dir);
}

#endif

OG_END
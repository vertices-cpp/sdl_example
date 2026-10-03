#include "OGFileUtils.h" 
 
OG_BEGIN

FileUtils* FileUtils::s_sharedFileUtils = nullptr;



 
//-------------------------------------FileUtils--------------------------

//判断是否是绝对路径
bool FileUtils::isAbsolutePath(const std::string& strPath) const
{
	if (
		(strPath.length() > 2 &&
		((strPath[0] >= 'a' && strPath[0] <= 'z') ||
			(strPath[0] >= 'A' && strPath[0] <= 'Z'))
			&& strPath[1] == ':')

		||
		(strPath[0] == '/' && strPath[1] == '/'))
	{
		return true;
	}
	return false;
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
	FILE *fp = fopen(strPath.c_str(), "rb");
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
	for (const auto& searchIt : _searchPathArray)
	{
		std::string fullpath = this->getPathForFilename(filename, searchIt);
		if (!fullpath.empty())
		{
			_fullPathCache.emplace(std::make_pair(filename, fullpath));
			return fullpath;
		}
	}

	return "";
}


std::string FileUtils::getSuitableFOpen(const std::string & filenameUtf8) const
{
	//CCASSERT(false, "getSuitableFOpen should be override by platform FileUtils");
	return filenameUtf8;
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

bool FileUtils::isFileExist(const std::string& filename) const
{
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


OG_END
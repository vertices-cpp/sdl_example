#ifndef _FILE_UTILS_H_
#define _FILE_UTILS_H_

#include "base/OGValue.h"
#include "base/OGData.h"
#include "base/OGAsyncTaskPool.h"
#include "base/OGScheduler.h"
#include "base/OGDirector.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <mutex>

// #pragma warning(disable:4996)



OG_BEGIN

#define MAX_FILENAME 512
#define READ_SIZE 8192

#define dir_delimter '/'
#define OG_MAX_PATH  512



static std::string s_resourcePath = "";


// \\替换为/
static inline std::string convertPathFormatToUnixStyle(const std::string& path)
{
	std::string ret = path;
	int len = ret.length();
	for (int i = 0; i < len; ++i)
	{
		if (ret[i] == '\\')
		{
			ret[i] = '/';
		}
	}
	return ret;
}


//-------------------------------------AssetManager--------------------------
// #include <vector>
// class AssetManager {
// 	std::string _FilePath;
// 	std::vector<Data> file;
// public:
// 	vector<Data> get_file() { return file; }
// 	void addFile(const string &filePath, const Data &d) {
// 		_FilePath = filePath;
// 		file.push_back(move(d));
// 	}
// 
// };

//-------------------------------------FileUtils--------------------------




class ResizableBuffer {
public:
	virtual ~ResizableBuffer() {}
	virtual void resize(size_t size) = 0;
	virtual void* buffer() const = 0;
};

template<typename T>
class ResizableBufferAdapter { };


template<typename CharT, typename Traits, typename Allocator>
class ResizableBufferAdapter< std::basic_string<CharT, Traits, Allocator> > : public ResizableBuffer {
	typedef std::basic_string<CharT, Traits, Allocator> BufferType;
	BufferType* _buffer;
public:
	explicit ResizableBufferAdapter(BufferType* buffer) : _buffer(buffer) {}
	virtual void resize(size_t size) override {
		_buffer->resize((size + sizeof(CharT) - 1) / sizeof(CharT));
	}
	virtual void* buffer() const override {
		// can not invoke string::front() if it is empty

		if (_buffer->empty())
			return nullptr;
		else
			return &_buffer->front();
	}
};

template<typename T, typename Allocator>
class ResizableBufferAdapter< std::vector<T, Allocator> > : public ResizableBuffer {
	typedef std::vector<T, Allocator> BufferType;
	BufferType* _buffer;
public:
	explicit ResizableBufferAdapter(BufferType* buffer) : _buffer(buffer) {}
	virtual void resize(size_t size) override {
		_buffer->resize((size + sizeof(T) - 1) / sizeof(T));
	}
	virtual void* buffer() const override {
		// can not invoke vector::front() if it is empty

		if (_buffer->empty())
			return nullptr;
		else
			return &_buffer->front();
	}
};


template<>
class ResizableBufferAdapter<Data> : public ResizableBuffer {
	typedef Data BufferType;
	BufferType* _buffer;
public:
	explicit ResizableBufferAdapter(BufferType* buffer) : _buffer(buffer) {}
	virtual void resize(size_t size) override {
		size_t oldSize = static_cast<size_t>(_buffer->getSize());
		if (oldSize != size) {
			auto old = _buffer->getBytes();
			void* buffer = realloc(old, size);
			if (buffer)
				_buffer->fastSet((unsigned char*)buffer, size);
		}
	}
	virtual void* buffer() const override {
		return _buffer->getBytes();
	}
};

class FileUtils {
	enum class Status
	{
		OK = 0,
		NotExists = 1, // File not exists
		OpenFailed = 2, // Open file failed.
		ReadFailed = 3, // Read failed
		NotInitialized = 4, // FileUtils is not initializes
		TooLarge = 5, // The file is too large (great than 2^32-1)
		ObtainSizeFailed = 6, // Failed to obtain the file size.
		NotRegularFileType = 7 // File type is not S_IFREG
	};

	static FileUtils* s_sharedFileUtils;


	std::string _defaultResRootPath;
	mutable std::unordered_map<std::string, std::string> _fullPathCache;
	std::vector<std::string> _searchPathArray;
	ValueMap _filenameLookupDict;

	std::string  _writablePath;
	std::vector<std::string> _searchResolutionsOrderArray;
	mutable std::unordered_map<std::string, std::string> _fullPathCacheDir;
	std::vector<std::string> _originalSearchPaths;

	mutable std::recursive_mutex _mutex;
public:
	static FileUtils*  getInstance()
	{
		if (s_sharedFileUtils == nullptr)
		{
			s_sharedFileUtils = new FileUtils();
			if (!s_sharedFileUtils->init())
			{
				delete s_sharedFileUtils;
				s_sharedFileUtils = nullptr;

			}
		}
		return s_sharedFileUtils;
	}
	FileUtils() : _writablePath("") {}
	~FileUtils() {
		clear();
	}
	void clear() {
		_fullPathCache.clear();

	}

	ValueMap getValueMapFromFile(const std::string & filename) const;

	ValueMap getValueMapFromData(const char * filedata, int filesize) const;

	ValueVector getValueVectorFromFile(const std::string & filename) const;

	bool init();
	bool isAbsolutePath(const std::string & path) const;
	bool isFileExistInternal(const std::string & strFilePath)const;
	bool isFileExist(const std::string & filename) const;


	std::string getFullPathForFilenameWithinDirectory(const std::string & directory, const std::string & filename)const;
	std::string getPathForFilename(const std::string & filename, const std::string & resource_path)const;


	std::string fullPathForFilename(const std::string & filename)const;
	std::string getNewFilename(const std::string & filename) const;
	std::string fullPathFromRelativeFile(const std::string & filename, const std::string & relativeFile) const;

	Data getDataFromFile(const std::string & filename) const;
	std::string getStringFromFile(const std::string & filename) const;

	virtual std::string getSuitableFOpen(const std::string& filenameUtf8) const;
	std::vector<std::string> listFiles(const std::string & dirPath) const;



	FileUtils::Status getContents(const std::string & filename, ResizableBuffer * buffer) const;
	std::string fullPathForDirectory(const std::string & dir) const;

	std::string getFileExtension(const std::string & filePath) const;

	void setWritablePath(const std::string & writablePath);

	std::string getWritablePath() const;

	bool isDirectoryExistInternal(const std::string & dirPath) const;

	const std::vector<std::string> getSearchPaths() const;

	std::string getPathForDirectory(const std::string & dir, const std::string & resolutionDiretory, const std::string & searchPath) const;

	void setPopupNotify(bool notify);

	bool isPopupNotify() const;



	bool isDirectoryExist(const std::string & dirPath) const;

	bool createDirectory(const std::string & dirPath) const;

	bool removeDirectory(const std::string & dirPath) const;

	void addSearchPath(const std::string & searchpath, const bool front);

	void isDirectoryExist(const std::string & fullPath, std::function<void(bool)> callback) const;

	void createDirectory(const std::string & dirPath, std::function<void(bool)> callback) const;

	void removeDirectory(const std::string & dirPath, std::function<void(bool)> callback) const;

	void removeFile(const std::string & filepath, std::function<void(bool)> callback) const;

	void renameFile(const std::string & path, const std::string & oldname, const std::string & name, std::function<void(bool)> callback) const;

	void renameFile(const std::string & oldfullpath, const std::string & newfullpath, std::function<void(bool)> callback) const;

	//long getFileSize(const std::string & filepath);

	void listFilesRecursively(const std::string & dirPath, std::vector<std::string>* files) const;

	long getFileSize(const std::string & filepath) const;

	void getFileSize(const std::string & filepath, std::function<void(long)> callback) const;

	void listFilesAsync(const std::string & dirPath, std::function<void(std::vector<std::string>)> callback) const;

	void listFilesRecursivelyAsync(const std::string & dirPath, std::function<void(std::vector<std::string>)> callback) const;

	void isFileExist(const std::string & filename, std::function<void(bool)> callback) const;

	bool renameFile(const std::string & oldfullpath, const std::string & newfullpath) const;

	bool renameFile(const std::string & path, const std::string & oldname, const std::string & name) const;

	bool removeFile(const std::string & filepath) const;

	bool writeToFile(const ValueMap & dict, const std::string & fullPath) const;

	bool writeValueMapToFile(const ValueMap & dict, const std::string & fullPath) const;

	bool writeStringToFile(const std::string & dataStr, const std::string & fullPath) const;

	bool writeDataToFile(const Data & data, const std::string & fullPath) const;

	void purgeCachedEntries();

	void setSearchPaths(const std::vector<std::string>& searchPaths);

	template <
		typename T,
		typename Enable = typename std::enable_if<
		std::is_base_of< ResizableBuffer, ResizableBufferAdapter<T> >::value
		>::type
	>
		Status getContents(const std::string& filename, T* buffer) const {
		ResizableBufferAdapter<T> buf(buffer);
		return getContents(filename, &buf);
	}


	template<typename T, typename R, typename ...ARGS>
	static void performOperationOffthread(T&& action, R&& callback, ARGS&& ...args)
	{

		// Visual Studio 2013 does not support using std::bind to forward template parameters into
		// a lambda. To get around this, we will just copy these arguments via lambda capture
#if defined(_MSC_VER) && _MSC_VER  < 1900 
		auto lambda = [action, callback, args...]()
		{
			Director::getInstance()->getScheduler()->performFunctionInCocosThread(std::bind(callback, action(args...)));
		};
#else
		// As orange uses c++11, we will use std::bind to leverage move sematics to
		// move our arguments into our lambda, to potentially avoid copying. 
		auto lambda = std::bind([](const T& actionIn, const R& callbackIn, const ARGS& ...argsIn)
		{
			Director::getInstance()->getScheduler()->performFunctionInOrangeThread(std::bind(callbackIn, actionIn(argsIn...)));
		}, std::forward<T>(action), std::forward<R>(callback), std::forward<ARGS>(args)...);

#endif

		AsyncTaskPool::getInstance()->enqueue(AsyncTaskPool::TaskType::TASK_IO, [](void*) {}, nullptr, std::move(lambda));
	}
};


OG_END

#endif



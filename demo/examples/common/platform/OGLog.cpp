#include "platform/OGStdC.h"
#include "base/ogMacros.h"
#include "utils/iconv_wrapper.h"
#include "platform/OGPlatformMacros.h"

#include <stdarg.h>
#include <chrono>

OG_BEGIN


void log(const char * format, ...)
{
	int bufferSize = 16 * 1024;
	char* buf = nullptr;
	int nret = 0;
	va_list args;
	do
	{
		buf = new (std::nothrow) char[bufferSize];
		if (buf == nullptr)
			return;
		va_start(args, format);
		nret = vsnprintf(buf, bufferSize - 3, format, args);
		va_end(args);

		if (nret >= 0)
		{
			if (nret <= bufferSize - 3)
				break;
			else
			{
				bufferSize = nret + 3;
				delete[] buf;
			}
		}
		else
		{
			bufferSize *= 2;
			delete[] buf;
		}
	} while (true);
	buf[nret] = '\n';
	buf[++nret] = '\0';
 

#if OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "orange debug info", "%s", buf);

#elif OG_TARGET_PLATFORM == OG_PLATFORM_WIN32

	// UTF-8 -> UTF-16LE 用于 OutputDebugStringW
// 	std::string utf16leBuf = iconv_wrapper(std::string(buf, nret), Iconv::Utf8ToUtf16le);
// 	if (!utf16leBuf.empty()) {
// 		std::wstring wstr(reinterpret_cast<const wchar_t*>(utf16leBuf.data()),
// 			utf16leBuf.size() / sizeof(wchar_t));
// 		OutputDebugStringW(wstr.c_str());
// 	}

	// UTF-8 -> GBK 用于 printf
	std::string gbkBuf = iconv_wrapper(std::string(buf, nret), Iconv::Utf8ToGbk);
	printf("%s", gbkBuf.c_str());

	//SendLogToWindow(buf);
	fflush(stdout);
#else
		// Linux, Mac, iOS, etc
	fprintf(stdout, "%s", buf);
	fflush(stdout);
#endif

	//Director::getInstance()->getConsole()->log(buf);
	delete[] buf;
}

namespace StringUtils {
	std::string format(const char* format, ...)
	{
#define OG_VSNPRINTF_BUFFER_LENGTH 512
		va_list args;
		std::string buffer(OG_VSNPRINTF_BUFFER_LENGTH, '\0');

		va_start(args, format);
		int nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
		va_end(args);

		if (nret >= 0) {
			if ((unsigned int)nret < buffer.length()) {
				buffer.resize(nret);
			}
			else if ((unsigned int)nret > buffer.length()) { // VS2015/2017 or later Visual Studio Version
				buffer.resize(nret);

				va_start(args, format);
				nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
				va_end(args);

				assert(nret == buffer.length());
			}
			// else equals, do nothing.
		}
		else { // less or equal VS2013 and Unix System glibc implement.
			do {
				buffer.resize(buffer.length() * 3 / 2);

				va_start(args, format);
				nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
				va_end(args);

			} while (nret < 0);

			buffer.resize(nret);
		}

		return buffer;

	}

	long getCharacterCountInUTF8String(const std::string& utf8)
	{
		const unsigned char* s = (const unsigned char*)utf8.c_str();
		size_t n = utf8.length();
		long count = 0;

		while (n > 0)
		{
			unsigned char c = s[0];
			int len;
			if (c < 0x80) len = 1;
			else if (c < 0xC2) return count;   // 非法首字节
			else if (c < 0xE0) len = 2;
			else if (c < 0xF0) len = 3;
			else if (c < 0xF8) len = 4;
			else                return count;   // 非法

			if ((size_t)len > n) return count;  // 截断
			s += len;
			n -= len;
			++count;
		}
		return count;
	}
}



int gettimeofday(struct my_timeval *val, void* /*tz*/)
{
	if (!val) return 0;

	using namespace std::chrono;
	auto now = system_clock::now().time_since_epoch();
	auto secs = duration_cast<seconds>(now);
	auto usecs = duration_cast<microseconds>(now - secs);

	val->tv_sec = static_cast<long>(secs.count());
	val->tv_usec = static_cast<long>(usecs.count());
	return 0;
}
OG_END
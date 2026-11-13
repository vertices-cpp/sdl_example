 

#include "base/OGConsole.h"
#include "utils/iconv_wrapper.h"

#include <thread>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
// #include <io.h>
// #include <WS2tcpip.h>
// #include <Winsock2.h>
#if defined(__MINGW32__)
#include "platform/win32/inet_pton_mingw.h"
#endif
#define bzero(a, b) memset(a, 0, b);
#else
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#endif

#include "base/OGDirector.h"
#include "base/OGScheduler.h"
//#include "platform/OGPlatformConfig.h"
//#include "base/OGConfiguration.h"
#include "2d/OGScene.h"
#include "platform/OGFileUtils.h"
// #include "renderer/OGTextureCache.h"
// #include "base/base64.h"
// #include "base/ogUtils.h"

OG_BEGIN

extern const char* orangeVersion(void);

#define PROMPT  "> "
#define DEFAULT_COMMAND_SEPARATOR '|'

static const size_t SEND_BUFSIZ = 512;
 

OG_END

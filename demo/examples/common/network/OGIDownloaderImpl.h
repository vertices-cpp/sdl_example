

#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "base/OGConsole.h"

//#define OG_DOWNLOADER_DEBUG
#ifdef  OG_DOWNLOADER_DEBUG
#define DLLOG(format, ...)      orange::log(format, ##__VA_ARGS__)
#else
#define DLLOG(...)       do {} while (0)
#endif

namespace orange { namespace network
{
    class DownloadTask;

    class IDownloadTask
    {
    public:
        virtual ~IDownloadTask(){}
    };

    class IDownloaderImpl
    {
    public:
        virtual ~IDownloaderImpl(){}

        std::function<void(const DownloadTask& task,
                           int64_t bytesReceived,
                           int64_t totalBytesReceived,
                           int64_t totalBytesExpected,
                           std::function<int64_t(void *buffer, int64_t len)>& transferDataToBuffer)> onTaskProgress;

        std::function<void(const DownloadTask& task,
                           int errorCode,
                           int errorCodeInternal,
                           const std::string& errorStr,
                           std::vector<unsigned char>& data)> onTaskFinish;

        virtual IDownloadTask *createCoTask(std::shared_ptr<const DownloadTask>& task) = 0;
    };

}}  // namespace orange::network


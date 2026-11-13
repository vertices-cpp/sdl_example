
#include "base/OGAsyncTaskPool.h"

OG_BEGIN

AsyncTaskPool* AsyncTaskPool::s_asyncTaskPool = nullptr;

AsyncTaskPool* AsyncTaskPool::getInstance()
{
    if (s_asyncTaskPool == nullptr)
    {
        s_asyncTaskPool = new (std::nothrow) AsyncTaskPool();
    }
    return s_asyncTaskPool;
}

void AsyncTaskPool::destroyInstance()
{
    delete s_asyncTaskPool;
    s_asyncTaskPool = nullptr;
}

AsyncTaskPool::AsyncTaskPool()
{
}

AsyncTaskPool::~AsyncTaskPool()
{
}

OG_END

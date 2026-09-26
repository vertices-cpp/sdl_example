#pragma once

#include <string> 
#include <SDL.h>
namespace og {
 

inline std::string convertPathFormatToUnixStyle(const std::string& path)
{
    std::string ret = path;
    for (size_t i = 0; i < ret.size(); ++i)
        if (ret[i] == '\\') ret[i] = '/';
    return ret;
}

inline std::string checkPath(const std::string& path)
{
    return convertPathFormatToUnixStyle(SDL_GetBasePath()) + "Resources/" + path;
} 
} // namespace og
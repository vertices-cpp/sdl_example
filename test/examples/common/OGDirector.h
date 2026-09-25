#ifndef _OG_DIRECTOR_H_
#define _OG_DIRECTOR_H_

#include "OGTexture2D.h"
#include "OGFileUtils.h"

#include <string>
#include <unordered_map>

OG_BEGIN

// ==================== TextureCache ====================
class TextureCache
{
public:
    TextureCache() = default;
    ~TextureCache() { removeAllTextures(); }

    // 命中缓存直接返回，否则 loadFrom 加载
    Texture2D* addImage(const std::string& filepath)
    {
        // 1. 先查缓存
        auto it = _textures.find(filepath);
        if (it != _textures.end())
            return it->second;

        // 2. 解析真实路径
        std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filepath);
        if (fullPath.empty())
        {
            printf("[TextureCache] file not found: %s\n", filepath.c_str());
            return nullptr;
        }

        // 3. 创建并加载
        Texture2D* tex = new (std::nothrow) Texture2D();
        if (!tex) return nullptr;

        tex->loadFrom(fullPath.c_str());

        if (!tex->getTexture())
        {
            printf("[TextureCache] load failed: %s\n", fullPath.c_str());
            delete tex;
            return nullptr;
        }

        tex->retain();                  // 由 cache 持有
        _textures[filepath] = tex;
        return tex;
    }

    // 从内存加载（zip / 打包资源用）
    Texture2D* addImageFromMemory(const std::string& key,
                                  unsigned char* data, int len)
    {
        auto it = _textures.find(key);
        if (it != _textures.end())
            return it->second;

        Texture2D* tex = new (std::nothrow) Texture2D();
        if (!tex) return nullptr;

        if (!tex->loadMemData(data, len))
        {
            delete tex;
            return nullptr;
        }

        tex->retain();
        _textures[key] = tex;
        return tex;
    }

    // 主动移除
    void removeTexture(const std::string& key)
    {
        auto it = _textures.find(key);
        if (it == _textures.end()) return;
        it->second->release();
        _textures.erase(it);
    }

    void removeAllTextures()
    {
        for (auto& kv : _textures)
            kv.second->release();
        _textures.clear();
    }

    void removeUnusedTextures()
    {
        for (auto it = _textures.begin(); it != _textures.end(); )
        {
            if (it->second->getReferenceCount() == 1)
            {
                it->second->release();
                it = _textures.erase(it);
            }
            else ++it;
        }
    }

private:
    std::unordered_map<std::string, Texture2D*> _textures;
};

// ==================== Director ====================
class Director
{
public:
    static Director* getInstance()
    {
        static Director inst;
        return &inst;
    }

    TextureCache* getTextureCache() { return &_textureCache; }

    // 数据目录（你项目里可能已有，这里只是占位）
    void setResourceRoot(const std::string& path) { _resourceRoot = path; }
    const std::string& getResourceRoot() const { return _resourceRoot; }

private:
    Director() = default;
    ~Director() = default;
    Director(const Director&) = delete;
    Director& operator=(const Director&) = delete;

    TextureCache _textureCache;
    std::string  _resourceRoot;
};

OG_END

#endif
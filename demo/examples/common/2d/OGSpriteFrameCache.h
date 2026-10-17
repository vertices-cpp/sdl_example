 

#ifndef __SPRITE_OGSPRITE_FRAME_CACHE_H__
#define __SPRITE_OGSPRITE_FRAME_CACHE_H__

#include <set>
#include <unordered_map>
#include <string>
#include "2d/OGSpriteFrame.h"
#include "base/OGRef.h"
#include "base/OGValue.h"
#include "base/OGMap.h"

OG_BEGIN

class Sprite;
class Texture2D;
class PolygonInfo;
 
class SpriteFrameCache : public Ref
{
protected:
    /**
    * used to wrap plist & frame names & SpriteFrames
    */
    class PlistFramesCache {
    public:
        PlistFramesCache() { }
        void init() {
            _spriteFrames.reserve(20); clear();
        }
        /**  Record SpriteFrame with plist and frame name, add frame name 
        *    and plist to index
        */
        void insertFrame(const std::string &plist, const std::string &frame, SpriteFrame *frameObj);
        /** Delete frame from cache, rebuild index
        */
        bool eraseFrame(const std::string &frame);
        /** Delete a list of frames from cache, rebuild index
        */
        bool eraseFrames(const std::vector<std::string> &frame);
        /** Delete frame from index and SpriteFrame is kept.
        */
        bool erasePlistIndex(const std::string &frame);
        /** Clear index and all SpriteFrames.
        */
        void clear();

        inline bool hasFrame(const std::string &frame) const;
        inline bool isPlistUsed(const std::string &plist) const;

        inline SpriteFrame *at(const std::string &frame);
        inline Map<std::string, SpriteFrame*>& getSpriteFrames();

        void markPlistFull(const std::string &plist, bool full) { _isPlistFull[plist] = full; }
        bool isPlistFull(const std::string &plist) const
        {
            auto it = _isPlistFull.find(plist);
            return it == _isPlistFull.end() ? false : it->second;
        }
    private:
        Map<std::string, SpriteFrame*> _spriteFrames;
        std::unordered_map<std::string, std::set<std::string>> _indexPlist2Frames;
        std::unordered_map<std::string, std::string> _indexFrame2plist;
        std::unordered_map<std::string, bool> _isPlistFull;
    };

public:
    /** Returns the shared instance of the Sprite Frame cache.
     *
     * @return The instance of the Sprite Frame Cache.
     * @js NA
     */
    static SpriteFrameCache* getInstance();

    /** Destroys the cache. It releases all the Sprite Frames and the retained instance.
	 * @js NA
     */
    static void destroyInstance();

    /** Destructor.
     * @js NA
     * @lua NA
     */
    virtual ~SpriteFrameCache();
    
    /** Initialize method.
     *
     * @return if success return true.
     */
    bool init();
	 
//     void addSpriteFramesWithFile(const std::string& plist);
// 	 
//     void addSpriteFramesWithFile(const std::string& plist, const std::string& textureFileName);
//  
//     void addSpriteFramesWithFile(const std::string&plist, Texture2D *texture); 
//     void addSpriteFramesWithFileContent(const std::string& plist_content, Texture2D *texture); 
//     void addSpriteFrame(SpriteFrame *frame, const std::string& frameName); 
//     bool isSpriteFramesWithFileLoaded(const std::string& plist) const; 
//     void removeSpriteFrames(); 
//     void removeUnusedSpriteFrames(); 
//     void removeSpriteFrameByName(const std::string& name); 
//     void removeSpriteFramesFromFile(const std::string& plist); 
//     void removeSpriteFramesFromFileContent(const std::string& plist_content); 
//     void removeSpriteFramesFromTexture(Texture2D* texture); 
     SpriteFrame* getSpriteFrameByName(const std::string& name);
// 
//     bool reloadTexture(const std::string& plist);

protected:   
	SpriteFrameCache(){} 
//     void addSpriteFramesWithDictionary(ValueMap& dictionary, Texture2D *texture, const std::string &plist);
//     
//     void addSpriteFramesWithDictionary(ValueMap& dictionary, const std::string &texturePath, const std::string &plist);
//     void removeSpriteFramesFromDictionary(ValueMap& dictionary);
// 	void reloadSpriteFramesWithDictionary(ValueMap& dictionary, Texture2D *texture, const std::string &plist);

	void initializePolygonInfo(const Size &textureSize,
                               const Size &spriteSize,
                               const std::vector<int> &vertices,
                               const std::vector<int> &verticesUV,
                               const std::vector<int> &triangleIndices,
                               PolygonInfo &polygonInfo);
	
	ValueMap _spriteFramesAliases;
    PlistFramesCache _spriteFramesCache;
};

// end of _2d group
/// @}

OG_END

#endif // __SPRITE_OGSPRITE_FRAME_CACHE_H__

 
#ifndef __SUPPORT_OG_UTILS_H__
#define __SUPPORT_OG_UTILS_H__

#include <vector>
#include <string>
#include "2d/OGNode.h"
#include "base/ogMacros.h"
#include "base/OGData.h"
//#include "renderer/backend/Types.h"
#include "math/Mat3.h"
 
/** @file ccUtils.h
Misc free functions
*/

OG_BEGIN
/*
ccNextPOT function is licensed under the same license that is used in Texture2D.m.
*/

/** Returns the Next Power of Two value.

Examples:
- If "value" is 15, it will return 16.
- If "value" is 16, it will return 16.
- If "value" is 17, it will return 32.
@param value The value to get next power of two.
@return Returns the next power of two value.
@since v0.99.5
*/

int ccNextPOT(int value);

class Sprite;
class Image;

namespace utils
{
    /** Capture the entire screen.
     * To ensure the snapshot is applied after everything is updated and rendered in the current frame,
     * we need to wrap the operation with a custom command which is then inserted into the tail of the render queue.
     * @param afterCaptured specify the callback function which will be invoked after the snapshot is done.
     * @param filename specify a filename where the snapshot is stored. This parameter can be either an absolute path or a simple
     * base filename ("hello.png" etc.), don't use a relative path containing directory names.("mydir/hello.png" etc.).
     * @since v3.2
     */
	  void  captureScreen(const std::function<void(bool, const std::string&)>& afterCaptured, const std::string& filename);

    /** Capture a specific Node.
    * @param startNode specify the snapshot Node. It should be orange::Scene
    * @param scale
    * @returns: return a Image, then can call saveToFile to save the image as "xxx.png or xxx.jpg".
    * @since v3.11
    * !!! remark: Caller is responsible for releasing it by calling delete.
    */
	  void captureNode(Node* startNode, std::function<void(Image*)> imageCallback, float scale = 1.0f);
    
    /** Find children by name, it will return all child that has the same name.
     * It supports c++ 11 regular expression. It is  a helper function of `Node::enumerateChildren()`.
     * You can refer to `Node::enumerateChildren()` for detail information.
     *
     * @param node The node to find
     * @param name The name to search for, it supports c++ 11 expression
     * @return Array of Nodes that matches the name
     * @since v3.2
     */
	  std::vector<Node*>  findChildren(const Node &node, const std::string &name);
    
    /** Same to ::atof, but strip the string, remain 7 numbers after '.' before call atof.
     * Why we need this? Because in android c++_static, atof ( and std::atof ) is unsupported for numbers have long decimal part and contain
     * several numbers can approximate to 1 ( like 90.099998474121094 ), it will return inf. This function is used to fix this bug.
     * @param str The string be to converted to double.
     * @return Returns converted value of a string.
     */
      double  atof(const char* str);

    /** Get current exact time, accurate to nanoseconds.
     * @return Returns the time in seconds since the Epoch.
     */
      double  gettime();

    /**
     * Get current time in milliseconds, accurate to nanoseconds
     *
     * @return  Returns the time in milliseconds since the Epoch.
     */
      long long  getTimeInMilliseconds();

    /**
     * Calculate unionof bounding box of a node and its children.
     * @return Returns unionof bounding box of a node and its children.
     */
      Rect getCascadeBoundingBox(Node *node);

    /**
     * Create a sprite instance from base64 encoded image and adds the texture to the Texture Cache.

     * @return Returns an instance of sprite
     */
      Sprite* createSpriteFromBase64Cached(const char* base64String, const char* key);

    /**
    * Create a sprite instance from base64 encoded image.

    * @return Returns an instance of sprite
    */
      Sprite* createSpriteFromBase64(const char* base64String);


    /**
     * Find a child by name recursively

     * @return  Returns found node or nullptr
     */
     Node*  findChild(Node* levelRoot, const std::string& name);

    /**
     * Find a child by tag recursively

     * @return Returns found node or nullptr
     */
     Node*  findChild(Node* levelRoot, int tag);

    /**
     * Find a child by name recursively

     * @return  Returns found node or nullptr with specified type 'T'
     */
    template<typename T> inline
    T findChild(Node* levelRoot, const std::string& name)
    {
        return dynamic_cast<T>(findChild(levelRoot, name));
    }

    /**
     * Find a child by tag recursively

     * @return  Returns found node or nullptr with specified type 'T'
     */
    template<typename T> inline
    T findChild(Node* levelRoot, int tag)
    {
        return dynamic_cast<T>(findChild(levelRoot, tag));
    }

    /**
     *  Gets the md5 hash for the given file.
     *  @param filename The file to calculate md5 hash.
     *  @return The md5 hash for the file
     */
     std::string getFileMD5Hash(const std::string &filename);


    /**
    *  Gets the md5 hash for the given buffer.
    *  @param data The buffer to calculate md5 hash.
    *  @return The md5 hash for the data
    */
     std::string getDataMD5Hash(const Data &data);

    /**
    @brief Converts language iso 639-1 code to LanguageType enum.
    @return LanguageType enum.
    * @js NA
    * @lua NA
    */
//      LanguageType getLanguageTypeByISO2(const char* code);
//     
//      backend::BlendFactor toBackendBlendFactor(int factor);
// 
//      int toGLBlendFactor(backend::BlendFactor blendFactor);
// 
//      backend::SamplerFilter toBackendSamplerFilter(int mode);
// 
//      backend::SamplerAddressMode toBackendAddressMode(int mode);

    // Adjust matrix for metal.
     const Mat3& getAdjustMatrix();

    /**
    Get the Normal Matrix of matrixMV
    */
     std::vector<float> getNormalMat3OfMat3(const Mat3 &mat);

    /**
    @brief Parses a list of space-separated integers.
    @return Vector of ints.
    * @js NA
    * @lua NA
    */
     std::vector<int> parseIntegerList(const std::string &intsString);
}

OG_END

#endif // __SUPPORT_OG_UTILS_H__

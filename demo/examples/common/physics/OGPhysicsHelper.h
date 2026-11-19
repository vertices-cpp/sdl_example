

#ifndef _OG_PHYSICS_HELPER_H_
#define _OG_PHYSICS_HELPER_H_

#include "base/ogConfig.h"
#if OG_USE_PHYSICS

#include "chipmunk/chipmunk.h"
#include "platform/OGPlatformMacros.h"
#include "math/OGGeometry.h"

OG_BEGIN

/**
 * @addtogroup physics
 * @{
 * @addtogroup physics_2d
 * @{
 */

/**
 * A physics helper class.
 *
 * Support for conversion between the chipmunk types and cocos types, eg: cpVect to Vec2, cpVect to Size, cpFloat to float.
 */
class PhysicsHelper
{
public:
    /** Make cpVect type convert to Vec2 type. */
    static Vec2 cpv2point(const cpVect& vec) { return Vec2(vec.x, vec.y); }

    /** Make Vec2 type convert to cpVect type. */
    static cpVect point2cpv(const Vec2& point) { return cpv(point.x, point.y); }
    
    /** Make cpVect type convert to Size type. */
    static Size cpv2size(const cpVect& vec) { return Size(vec.x, vec.y); }
    
    /** Make Size type convert to cpVect type. */
    static cpVect size2cpv(const Size& size) { return cpv(size.width, size.height); }
    
    /** Make cpFloat type convert to float type. */
    static float cpfloat2float(cpFloat f) { return f; }
    
    /** Make Rect type convert to cpBB type. */
    static cpBB rect2cpbb(const Rect& rect) { return cpBBNew(rect.origin.x, rect.origin.y, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height); }
    
    /** Make cpBB type convert to Rect type. */
    static Rect cpbb2rect(const cpBB& bb) { return Rect(bb.l, bb.b, bb.r -  bb.l, bb.t - bb.b); }
    
    /** 
     Make cpVect array convert to Vec2 array.
     
     @param cpvs The be converted object, it's a cpVect array.
     @param out The converted object, it's a Vec2 array.
     @param count It's cpvs array length.
     @return The out object's pointer.
     */
    static Vec2* cpvs2points(const cpVect* cpvs, Vec2* out, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            out[i] = cpv2point(cpvs[i]);
        }
        
        return out;
    }
    
    /**
     Make Vec2 array convert to cpVect array.
     
     @param points The be converted object, it's a Vec2 array.
     @param out The converted object, it's a cpVect array.
     @param count It's points array length.
     @return The out object's pointer.
     */
    static cpVect* points2cpvs(const Vec2* points, cpVect* out, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            out[i] = point2cpv(points[i]);
        }
        
        return out;
    }
};

/** @} */
/** @} */

OG_END

#endif // OG_USE_PHYSICS
#endif // _OG_PHYSICS_HELPER_H_

#ifndef _TYPES_H_
#define _TYPES_H_

#include <string> 
#include <map>
#include "OGPlatformMacros.h"
// #include "math/CCMath.h"
#include "OGRef.h"
#include "OGGeometry.h"
//#include <SDL.h>
// #include "base/CCEnumClass.h"
// #include "renderer/backend/Types.h"

/**
 * @addtogroup base
 * @{
 */

OG_BEGIN

struct Color4B;
struct Color4F;

/**
 * RGB color composed of bytes 3 bytes.
 * @since v3.0
 */
struct  Color3B
{
	Color3B();
	Color3B(uint8_t _r, uint8_t _g, uint8_t _b);
	explicit Color3B(const Color4B& color);
	explicit Color3B(const Color4F& color);

	bool operator==(const Color3B& right) const;
	bool operator==(const Color4B& right) const;
	bool operator==(const Color4F& right) const;
	bool operator!=(const Color3B& right) const;
	bool operator!=(const Color4B& right) const;
	bool operator!=(const Color4F& right) const;

	bool equals(const Color3B& other) const
	{
		return (*this == other);
	}

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

	static const Color3B WHITE;
	static const Color3B YELLOW;
	static const Color3B BLUE;
	static const Color3B GREEN;
	static const Color3B RED;
	static const Color3B MAGENTA;
	static const Color3B BLACK;
	static const Color3B ORANGE;
	static const Color3B GRAY;
};

/**
 * RGBA color composed of 4 bytes.
 * @since v3.0
 */
struct  Color4B
{
	Color4B();
	// 	Color4B(const Color4B& color):r(color.r),g(color.g),b(color.b),a(color.a){}
	// 	Color4B& operator = (const Color4B& color) {
	// 		if (&color != this)
	// 		{
	// 			r = color.r, g = color.g, b = color.b, a = color.a;
	// 		}
	// 		return *this;
	// 	}
	Color4B(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a);
	explicit Color4B(const Color3B& color);
	Color4B& operator=(const Color3B &color)
	{
		r = color.r, g = color.g, b = color.b;
		return *this;
	}
	explicit Color4B(const Color3B& color, uint8_t _a = 255);
	explicit Color4B(const Color4F& color);

	inline void set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
	// 	void set(Color4B rhs)
	// 	{
	// 		r = rhs.r;
	// 		g = rhs.g;
	// 		b = rhs.b;
	// 		a = rhs.a;
	// 	}

	bool operator==(const Color4B& right) const;
	bool operator==(const Color3B& right) const;
	bool operator==(const Color4F& right) const;
	bool operator!=(const Color4B& right) const;
	bool operator!=(const Color3B& right) const;
	bool operator!=(const Color4F& right) const;

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;

	static const Color4B WHITE;
	static const Color4B YELLOW;
	static const Color4B BLUE;
	static const Color4B GREEN;
	static const Color4B RED;
	static const Color4B MAGENTA;
	static const Color4B BLACK;
	static const Color4B ORANGE;
	static const Color4B GRAY;
};


/**
 * RGBA color composed of 4 floats.
 * @since v3.0
 */
struct  Color4F
{
	Color4F();
	Color4F(float _r, float _g, float _b, float _a);
	explicit Color4F(const Color3B& color, float _a = 1.0f);
	explicit Color4F(const Color4B& color);

	bool operator==(const Color4F& right) const;
	bool operator==(const Color3B& right) const;
	bool operator==(const Color4B& right) const;
	bool operator!=(const Color4F& right) const;
	bool operator!=(const Color3B& right) const;
	bool operator!=(const Color4B& right) const;

	bool equals(const Color4F &other) const
	{
		return (*this == other);
	}

	float r = 0.f;
	float g = 0.f;
	float b = 0.f;
	float a = 0.f;

	static const Color4F WHITE;
	static const Color4F YELLOW;
	static const Color4F BLUE;
	static const Color4F GREEN;
	static const Color4F RED;
	static const Color4F MAGENTA;
	static const Color4F BLACK;
	static const Color4F ORANGE;
	static const Color4F GRAY;
};

Color4F& operator+=(Color4F& lhs, const Color4F& rhs);
Color4F operator+(Color4F lhs, const Color4F& rhs);

Color4F& operator-=(Color4F& lhs, const Color4F& rhs);
Color4F operator-(Color4F lhs, const Color4F& rhs);

Color4F& operator*=(Color4F& lhs, const Color4F& rhs);
Color4F operator*(Color4F lhs, const Color4F& rhs);
Color4F& operator*=(Color4F& lhs, float rhs);
Color4F operator*(Color4F lhs, float rhs);

Color4F& operator/=(Color4F& lhs, const Color4F& rhs);
Color4F operator/(Color4F lhs, const Color4F& rhs);
Color4F& operator/=(Color4F& lhs, float rhs);
Color4F operator/(Color4F lhs, float rhs);


/** A vertex composed of 2 floats: x, y
 @since v3.0
 */
 // struct Vertex2F
 // {
 //     Vertex2F(float _x, float _y) :x(_x), y(_y) {}

 //     Vertex2F(): x(0.f), y(0.f) {}

 //     GLfloat x;
 //     GLfloat y;
 // };


 /** A vertex composed of 2 floats: x, y
  @since v3.0
  */
  // struct Vertex3F
  // {
  //     Vertex3F(float _x, float _y, float _z)
  //         : x(_x)
  //         , y(_y)
  //         , z(_z)
  //     {}

  //     Vertex3F(): x(0.f), y(0.f), z(0.f) {}

  //     GLfloat x;
  //     GLfloat y;
  //     GLfloat z;
  // };

  /** @struct Tex2F
   * A TEXCOORD composed of 2 floats: u, y
   * @since v3.0
   */
struct  Tex2F {
	Tex2F(float _u, float _v) : u(_u), v(_v) {}

	Tex2F() {}

	float u = 0.f;
	float v = 0.f;
};

/** @struct PointSprite
 * Vec2 Sprite component.
 */
struct  PointSprite
{
	Vec2       pos;        // 8 bytes
	Color4B    color;      // 4 bytes
	float      size = 0.f;       // 4 bytes
};

/** @struct Quad2
 * A 2D Quad. 4 * 2 floats.
 */
struct  Quad2
{
	Vec2        tl;
	Vec2        tr;
	Vec2        bl;
	Vec2        br;
};

/** @struct Quad3
 * A 3D Quad. 4 * 3 floats.
 */
 // struct  Quad3 {
 //     Vec3        bl;
 //     Vec3        br;
 //     Vec3        tl;
 //     Vec3        tr;
 // };

 /** @struct V2F_C4B_T2F
  * A Vec2 with a vertex point, a tex coord point and a color 4B.
  */
struct V2F_C4B_T2F
{
	/// vertices (2F)
	Vec2       vertices;
	/// colors (4B)
	Color4B        colors;
	/// tex coords (2F)
	Tex2F          texCoords;
};
// 
// /** @struct V2F_C4B_PF
//  *
//  */
// struct V2F_C4B_PF
// {
//     /// vertices (2F)
//     Vec2       vertices;
//     /// colors (4B)
//     Color4B        colors;
//     /// pointsize
//     float      pointSize = 0.f;
// };
// 
// /** @struct V2F_C4F_T2F
//  * A Vec2 with a vertex point, a tex coord point and a color 4F.
//  */
// struct  V2F_C4F_T2F
// {
//     /// vertices (2F)
//     Vec2       vertices;
//     /// colors (4F)
//     Color4F        colors;
//     /// tex coords (2F)
//     Tex2F          texCoords;
// };

/** @struct V3F_C4B_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
 // struct  V3F_C4B_T2F
 // {
 //     /// vertices (3F)
 //     Vec3     vertices;            // 12 bytes
 // 
 //     /// colors (4B)
 //     Color4B      colors;              // 4 bytes
 // 
 //     // tex coords (2F)
 //     Tex2F        texCoords;           // 8 bytes
 // };

 /** @struct V3F_T2F
  * A Vec2 with a vertex point, a tex coord point.
  */
  // struct  V3F_T2F
  // {
  //     /// vertices (2F)
  //     Vec3       vertices;
  //     /// tex coords (2F)
  //     Tex2F          texCoords;
  // };
  // 
  // /** @struct V3F_C4F
  // * A Vec3 with a vertex point, a color.
  // */
  // struct  V3F_C4F 
  // {
  //     /// vertices (3F)
  //     Vec3 vertices;
  //     /// vertices (4F)
  //     Color4F colors;
  // };
  // 
  // /** @struct V2F_C4B_T2F_Triangle
  //  * A Triangle of V2F_C4B_T2F.
  //  */
  // struct  V2F_C4B_T2F_Triangle
  // {
  //     V2F_C4B_T2F a;
  //     V2F_C4B_T2F b;
  //     V2F_C4B_T2F c;
  // };
  // 
  // /** @struct V2F_C4B_T2F_Quad
  //  * A Quad of V2F_C4B_T2F.
  //  */
struct  V2F_C4B_T2F_Quad
{
	/// top left
	V2F_C4B_T2F    tl;
	/// top right
	V2F_C4B_T2F    tr;
	/// bottom left
	V2F_C4B_T2F    bl;
	/// bottom right
	V2F_C4B_T2F    br;

};
// 
// /** @struct V3F_C4B_T2F_Quad
//  * 4 Vertex3FTex2FColor4B.
//  */
// struct  V3F_C4B_T2F_Quad
// {
//     /// top left
//     V3F_C4B_T2F    tl;
//     /// bottom left
//     V3F_C4B_T2F    bl;
//     /// top right
//     V3F_C4B_T2F    tr;
//     /// bottom right
//     V3F_C4B_T2F    br;
// };
// 
// /** @struct V2F_C4F_T2F_Quad
//  * 4 Vertex2FTex2FColor4F Quad.
//  */
// struct  V2F_C4F_T2F_Quad
// {
// 	/// top left
// 	V2F_C4F_T2F    tl;
// 	/// top right
// 	V2F_C4F_T2F    tr;
// 
// 	/// bottom left
// 	V2F_C4F_T2F    bl;
// 	
//     /// bottom right
//     V2F_C4F_T2F    br;
//     
// };

// 
// /** @struct V3F_T2F_Quad
//  *
//  */
// struct  V3F_T2F_Quad
// {
//     /// bottom left
//     V3F_T2F    bl;
//     /// bottom right
//     V3F_T2F    br;
//     /// top left
//     V3F_T2F    tl;
//     /// top right
//     V3F_T2F    tr;
// };
// 
// 
// namespace backend {
//     enum class BlendFactor: uint32_t;
// }

/** @struct BlendFunc
 * Blend Function used for textures.
 */
 // struct  BlendFunc
 // {
 //     /** source blend function */
 //     backend::BlendFactor src;
 //     /** destination blend function */
 //     backend::BlendFactor dst;
 // 
 //     /** Blending disabled. Uses {BlendFactor::ONE, BlendFactor::ZERO} */
 //     static const BlendFunc DISABLE;
 //     /** Blending enabled for textures with Alpha premultiplied. Uses {BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA} */
 //     static const BlendFunc ALPHA_PREMULTIPLIED;
 //     /** Blending enabled for textures with Alpha NON premultiplied. Uses {BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA} */
 //     static const BlendFunc ALPHA_NON_PREMULTIPLIED;
 //     /** Enables Additive blending. Uses {BlendFactor::SRC_ALPHA, BlendFactor::ONE} */
 //     static const BlendFunc ADDITIVE;
 // 
 //     bool operator==(const BlendFunc &a) const
 //     {
 //         return src == a.src && dst == a.dst;
 //     }
 // 
 //     bool operator!=(const BlendFunc &a) const
 //     {
 //         return src != a.src || dst != a.dst;
 //     }
 // 
 //     bool operator<(const BlendFunc &a) const
 //     {
 //         return src < a.src || (src == a.src && dst < a.dst);
 //     }
 // };

 /** @enum TextVAlignment
  * Vertical text alignment type.
  *
  * @note If any of these enums are edited and/or reordered, update Texture2D.m.
  */
enum class  TextVAlignment
{
	TOP,
	CENTER,
	BOTTOM
};

/** @enum TextHAlignment
 * Horizontal text alignment type.
 *
 * @note If any of these enums are edited and/or reordered, update Texture2D.m.
 */
enum class  TextHAlignment
{
	LEFT,
	CENTER,
	RIGHT
};

/**
* @brief Possible GlyphCollection used by Label.
*
* Specify a collections of characters to be load when Label created.
* Consider using DYNAMIC.
*/
enum class GlyphCollection {
	DYNAMIC,
	NEHE,
	ASCII,
	CUSTOM
};

// Types for animation in particle systems

/** @struct T2F_Quad
 * Texture coordinates for a quad.
 */
 // struct  T2F_Quad
 // {
 //     /// bottom left
 //     Tex2F    bl;
 //     /// bottom right
 //     Tex2F    br;
 //     /// top left
 //     Tex2F    tl;
 //     /// top right
 //     Tex2F    tr;
 // };

 /** @struct AnimationFrameData
  * Struct that holds the size in pixels, texture coordinates and delays for animated ParticleSystemQuad.
  */
  // struct  AnimationFrameData
  // {
  //     T2F_Quad texCoords;
  //     Size size;
  //     float delay = 0.f;
  // };

  /**
   types used for defining fonts properties (i.e. font name, size, stroke or shadow)
   */

   /** @struct FontShadow
	* Shadow attributes.
	*/

	//自定义类型


struct  FontShadow
{
	/// shadow x and y offset
	Size   _shadowOffset;
	/// shadow blurriness
	float  _shadowBlur = 0.f;
	/// shadow opacity
	float  _shadowOpacity = 0.f;
	/// true if shadow enabled
	bool   _shadowEnabled = false;
};

/** @struct FontStroke
 * Stroke attributes.
 */
struct  FontStroke
{
	/// stroke color
	Color3B   _strokeColor = Color3B::BLACK;
	/// stroke size
	float     _strokeSize = 0.f;
	/// true if stroke enabled
	bool      _strokeEnabled = false;
	/// stroke alpha
	uint8_t   _strokeAlpha = 255;


};

/** @struct FontDefinition
 * Font attributes.
 */
struct  FontDefinition
{
	/// font name
	std::string           _fontName;
	/// font size
	int                   _fontSize = 0;
	/// horizontal alignment
	TextHAlignment        _alignment = TextHAlignment::CENTER;
	/// vertical alignment
	TextVAlignment _vertAlignment = TextVAlignment::TOP;
	/// rendering box
	Size                  _dimensions = Size::ZERO;
	/// font color
	Color3B               _fontFillColor = Color3B::WHITE;
	/// font alpha
	uint8_t               _fontAlpha = 255;
	/// font shadow
	FontShadow            _shadow;
	/// font stroke
	FontStroke            _stroke;
	/// enable text wrap
	bool                  _enableWrap = true;
	/** There are 4 overflows: none, clamp, shrink and resize_height.
	 *  The corresponding integer values are 0, 1, 2, 3 respectively
	 * For more information, please refer to Label::Overflow enum class.
	 */
	int                  _overflow = 0;
};

/**
 * @brief Effects used by `Label`
 *
 */
enum class LabelEffect {
	// FIXME: Covert them to bitwise. More than one effect should be supported
	NORMAL,
	OUTLINE,
	SHADOW,
	GLOW,
	ITALICS,
	BOLD,
	UNDERLINE,
	STRIKETHROUGH,
	ALL
};

/** @struct Acceleration
 * The device accelerometer reports values for each axis in units of g-force.
 */
class  Acceleration : public Ref
{
public:
	double x = 0;
	double y = 0;
	double z = 0;

	double timestamp = 0;
};

extern const std::string  STD_STRING_EMPTY;
extern const long  OG_INVALID_INDEX;

enum class SetIntervalReason : char
{
	BY_GAME = 0,
	BY_ENGINE,
	BY_SYSTEM,
	BY_SCENE_CHANGE,
	BY_DIRECTOR_PAUSE
};
// 
// struct  Viewport
// {
//     int x = 0;
//     int y = 0;
//     unsigned int w = 0;
//     unsigned int h = 0;
// };
//     
// struct  ScissorRect
// {
//     float x = 0;
//     float y = 0;
//     float width = 0;
//     float height = 0;
// };
// 
// enum class ClearFlag : uint8_t
// {
//     NONE = 0,
//     COLOR = 1,
//     DEPTH = 1 << 1,
//     STENCIL = 1 << 2,
//     ALL = COLOR | DEPTH | STENCIL
// };
// ENABLE_BITMASK_OPERATORS(ClearFlag)
// 
// enum class RenderTargetFlag : uint8_t
// {
//     COLOR = 1,
//     DEPTH = 1 << 1,
//     STENCIL = 1 << 2,
//     ALL = COLOR | DEPTH | STENCIL
// };
// ENABLE_BITMASK_OPERATORS(RenderTargetFlag)
// using TextureUsage = backend::TextureUsage;
// using PixelFormat = backend::PixelFormat;

// enum class BlendFunc
// {
// 	DISABLE = SDL_BLENDMODE_BLEND,
// 	ALPHA_PREMULTIPLIED = SDL_BLENDMODE_BLEND,
// 	ALPHA_NON_PREMULTIPLIED = SDL_BLENDMODE_BLEND,
// 	ADDITIVE = SDL_BLENDMODE_ADD
// 
// };




OG_END
// end group
/// @}
#endif
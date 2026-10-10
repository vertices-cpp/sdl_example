
#pragma once

#include "OGNode.h"
#include "OGProtocols.h"

#include "OGCustomCommand.h"

#include "OGTouch.h"
#include "OGEventKeyboard.h"
#include "OGEventListener.h"

#include <vector>

OG_BEGIN

/**
 * @addtogroup _2d
 * @{
 */

class __Set;
class TouchScriptHandlerEntry;

class EventListenerTouch;
class EventListenerKeyboard;
class EventListenerAcceleration;

class Touch;

//
// Layer
//
/** @class Layer
 * @brief Layer is a subclass of Node that implements the TouchEventsDelegate protocol.

All features from Node are valid, plus the following new features:
- It can receive iPhone Touches
- It can receive Accelerometer input
*/

class Acceleration;

class   Layer : public Node
{
public:
	static Layer *create();
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	virtual void onTouchCancelled(Touch *touch, Event *unused_event);
	virtual void onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event);
	virtual void onTouchesMoved(const std::vector<Touch*>& touches, Event *unused_event);
	virtual void onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event);
	virtual void onTouchesCancelled(const std::vector<Touch*>&touches, Event *unused_event);
	virtual void onAcceleration(Acceleration* acc, Event* unused_event) {};
	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);


public:
	Layer();
	virtual ~Layer();

	virtual bool init() override;

protected:

// 	int executeScriptTouchHandler(EventTouch::EventCode eventType, Touch* touch, Event* event);
// 	int executeScriptTouchesHandler(EventTouch::EventCode eventType, const std::vector<Touch*>& touches, Event* event);

	bool _touchEnabled;
	bool _accelerometerEnabled;
	bool _keyboardEnabled;
	EventListener* _touchListener;
	EventListenerKeyboard* _keyboardListener;
	EventListenerAcceleration* _accelerationListener;

	Touch::DispatchMode _touchMode;
	bool _swallowsTouches;

private:
	OG_DISALLOW_COPY_AND_ASSIGN(Layer);

};

//
// LayerColor
//
/** @class LayerColor
 * @brief LayerColor is a subclass of Layer that implements the RGBAProtocol protocol.

All features from Layer are valid, plus the following new features:
- opacity
- RGB colors
*/
class  LayerColor : public Layer, public BlendProtocol
{
public:
	static LayerColor* create();
	static LayerColor * create(const Color4B& color, float width, float height);
	static LayerColor * create(const Color4B& color);
	void changeWidth(float w);
	void changeHeight(float h);
	void changeWidthAndHeight(float w, float h);
	virtual void draw(Renderer *renderer, const Mat3 &transform, uint32_t flags) override;

	virtual void setContentSize(const Size & var) override;
	virtual const BlendFunc& getBlendFunc() const override;
	virtual void setBlendFunc(const BlendFunc& blendFunc) override;

public:
	LayerColor();
	virtual ~LayerColor();

	bool init() override;
	bool initWithColor(const Color4B& color, float width, float height);
	bool initWithColor(const Color4B& color);

protected:

	virtual void updateColor() override;
	//void updateVertexBuffer();

	BlendFunc _blendFunc; 

	CustomCommand _customCommand;

	int _indices[6];
	V2F_C4B_T2F _vertexData[4];

private:
	OG_DISALLOW_COPY_AND_ASSIGN(LayerColor);

};

class  LayerGradient : public LayerColor
{
public:
	static LayerGradient* create();
	static LayerGradient* create(const Color4B& start, const Color4B& end);
	static LayerGradient* create(const Color4B& start, const Color4B& end, const Vec2& v);
	void setCompressedInterpolation(bool compressedInterpolation);
	bool isCompressedInterpolation() const;
	void setStartColor(const Color3B& startColor);
	const Color3B& getStartColor() const;

	void setEndColor(const Color3B& endColor);
	const Color3B& getEndColor() const;
	void setStartOpacity(uint8_t startOpacity);
	uint8_t getStartOpacity() const;
	void setEndOpacity(uint8_t endOpacity);
	uint8_t getEndOpacity() const;
	void setVector(const Vec2& alongVector);
	const Vec2& getVector() const;
public:
	LayerGradient();
	virtual ~LayerGradient();

	virtual bool init() override;
	bool initWithColor(const Color4B& start, const Color4B& end);
	bool initWithColor(const Color4B& start, const Color4B& end, const Vec2& v);

protected:
	virtual void updateColor() override;

	Color3B _startColor = Color3B::BLACK;
	Color3B _endColor = Color3B::BLACK;
	uint8_t _startOpacity = 255;
	uint8_t _endOpacity = 255;
	Vec2   _alongVector = { 0.0f, -1.0f };
	bool    _compressedInterpolation = true;
};


class  LayerRadialGradient : public Layer
{
public:
	static LayerRadialGradient* create(const Color4B& startColor, const Color4B& endColor, float radius, const Vec2& center, float expand);
	static LayerRadialGradient* create();

	//
	// overrides
	//
	virtual void draw(Renderer *renderer, const Mat3 &transform, uint32_t flags) override;
	virtual void setContentSize(const Size& size) override;

	void setStartOpacity(uint8_t opacity);
	uint8_t getStartOpacity() const;

	void setEndOpacity(uint8_t opacity);
	uint8_t getEndOpacity() const;

	void setRadius(float radius);
	float getRadius() const;

	void setCenter(const Vec2& center);
	Vec2 getCenter() const;

	void setExpand(float expand);
	float getExpand() const;

	void setStartColor(const Color3B& color);
	void setStartColor(const Color4B& color);
	Color4B getStartColor() const;
	Color3B getStartColor3B() const;

	void setEndColor(const Color3B& color);
	void setEndColor(const Color4B& color);
	Color4B getEndColor() const;
	Color3B getEndColor3B() const;

	void setBlendFunc(const BlendFunc& blendFunc);
	BlendFunc getBlendFunc() const;

public:
	LayerRadialGradient();
	virtual ~LayerRadialGradient();

	bool initWithColor(const Color4B& startColor, const Color4B& endColor, float radius, const Vec2& center, float expand);

private:
	void convertColor4B24F(Color4F& outColor, const Color4B& inColor);

	Color4B _startColor = Color4B::BLACK;
	Color4F _startColorRend = Color4F::BLACK; // start color used in shader

	Color4B _endColor = Color4B::BLACK;
	Color4F _endColorRend = Color4F::BLACK; // end color used in shader

	//Vec2 _vertices[4];
	Vec2 _center;
	float _radius = 0.f;
	float _expand = 0.f;
	
	
	CustomCommand _customCommand;

	BlendFunc _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;

};

class   LayerMultiplex : public Layer
{
public:
	static LayerMultiplex* create();
	static LayerMultiplex* createWithArray(const Vector<Layer*>& arrayOfLayers);
	static LayerMultiplex * create(Layer* layer, ...);
	static LayerMultiplex * createWithLayer(Layer* layer);
	void addLayer(Layer* layer);
	void switchTo(int n);
	void switchTo(int n, bool cleanup);
	void switchToAndReleaseMe(int n);



public:
	/**
	 * @js ctor
	 */
	LayerMultiplex();
	/**
	 * @js NA
	 * @lua NA
	 */
	virtual ~LayerMultiplex();

	virtual bool init() override;
	bool initWithLayers(Layer* layer, va_list params);
	bool initWithArray(const Vector<Layer*>& arrayOfLayers);

protected:
	unsigned int _enabledLayer;
	Vector<Layer*>    _layers;

private:
	OG_DISALLOW_COPY_AND_ASSIGN(LayerMultiplex);
};


// end of _2d group
/// @}

OG_END

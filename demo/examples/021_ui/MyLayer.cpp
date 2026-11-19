#include "MyLayer.h"
 

MyLayer::MyLayer() : _displayValueLabel(nullptr), _uiLayer(nullptr), _button(nullptr) {}

bool MyLayer::init() {
	if (!Layer::init()) return false;
	Size widgetSize = Director::getInstance()->getWinSize();
	_uiLayer = Layer::create();
	_uiLayer->setName("_uiLayer");
	_uiLayer->setContentSize(widgetSize);
	addChild(_uiLayer);
	return true;
} 
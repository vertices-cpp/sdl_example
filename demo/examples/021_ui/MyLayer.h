#ifndef _MY_LAYER_H_
#define _MY_LAYER_H_

#include "orange.h"
#include "ui/UIWidget.h"
#include <vector>
#include <functional>

USING_OG;
using namespace ui;

class MyLayer : public Layer {
public:
	MyLayer();
	virtual bool init();

	orange::Layer* _uiLayer;
	orange::ui::Text* _displayValueLabel;
	orange::ui::Button* _button;

	//orange::ui::Layout* _widget;
};
#endif
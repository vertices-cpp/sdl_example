#ifndef _TEST_H_
#define _TEST_H_

#include "2d/OGNode.h"
#include "2d/OGScene.h"
#include "2d/OGLabel.h"
USING_OG;

class Test :public Scene{
public:
	CREATE_FUNC(Test);
	bool init();

	void testDeleteCrash();

	void update(float dt);
	void draw(Renderer * renderer, const Mat3 & transform, uint32_t flags);
	 Label *label;
};

#endif
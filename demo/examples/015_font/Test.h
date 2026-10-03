#ifndef _TEST_H_
#define _TEST_H_

#include "OGNode.h"
#include "OGLabel.h"
USING_OG;

class Test :public Node{
public:
	CREATE_FUNC(Test);
	bool init();

	void update(float dt);
	void draw(Renderer * renderer, const Mat3 & transform, uint32_t flags);
	 Label *label;
};

#endif
#ifndef _HELLO_WORLD_H_
#define _HELLO_WORLD_H_

#include "2d/OGNode.h" 
#include "2d/OGScene.h"
#include "2d/OGDrawNode.h"
USING_OG;

class HelloWorld :public Scene{
public:
	CREATE_FUNC(HelloWorld);
    
	HelloWorld(){}
 
	static Scene * createScene();

	bool init();

	void draw(Renderer * renderer, const Mat3 & transform, uint32_t flags);
	 
	void uiTest();

};

#endif
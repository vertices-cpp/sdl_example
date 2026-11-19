#include "base/OGDirector.h"
#include "HelloWorld.h"
 
USING_OG;

int main(int argc, char *argv[])
{

	auto d = Director::getInstance();
	d->init();
	auto t = HelloWorld::createScene();
	d->runWithScene(t);
	d->mainLoop();
	return 0;
}
#include "base/OGDirector.h"
#include "Test.h"
#include <SDL.h>
USING_OG;

int main(int argc, char *argv[])
{
	auto d = Director::getInstance();
	d->init();
	auto t = Test::create();
	d->runWithScene(t);
	d->mainLoop();
	return 0;
}
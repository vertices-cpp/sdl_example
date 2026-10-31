#include "base/OGDirector.h"
#include "Test.h"
#include <SDL.h>
USING_OG;

int main(int argc, char *argv[])
{
	auto d = Director::getInstance();
	d->init();
	auto t = Test::createScene();
	d->setScene(t);
	d->MainLoop();
	return 0;
}
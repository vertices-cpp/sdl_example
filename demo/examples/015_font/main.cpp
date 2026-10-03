#include "OGDirector.h"
#include "Test.h"
#include <SDL.h>
USING_OG;

int main(int argc, char *argv[])
{
	auto d = Director::getInstance();
	auto t = Test::create();
	d->setScene(t);
	d->MainLoop();
	return 0;
}
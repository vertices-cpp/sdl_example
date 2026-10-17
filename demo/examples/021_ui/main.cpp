#include "base/OGDirector.h"
#include "Test.h"
 
USING_OG;

int main(int argc, char *argv[])
{
	auto d = Director::getInstance();
	d->init();
	auto t = Test::createScene();
	d->setScene(t);
	d->mainLoop();
	return 0;
}
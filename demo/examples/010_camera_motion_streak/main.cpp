  
#include "AppTest.h" 
#include <SDL.h>
USING_OG;

int main(int, char **)
{
	static AppTest* appTest = new AppTest();
	appTest->init();
	delete appTest;

	return 0;
}

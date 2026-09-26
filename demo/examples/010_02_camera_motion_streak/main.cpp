  
#include "AppTest.h" 
USING_OG;

int main(int, char **)
{
	AppTest* appTest = new AppTest();
	appTest->init();
	delete appTest;

	return 0;
}

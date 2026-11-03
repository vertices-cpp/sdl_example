 
#include "platform/OGApplication.h"
#include "base/OGDirector.h"
#include <algorithm>
#include "platform/OGFileUtils.h"
 
/**
@brief    This function change the PVRFrame show/hide setting in register.
@param  bEnable If true show the PVRFrame window, otherwise hide.
*/
 

OG_BEGIN

// sharedApplication pointer
Application * Application::sm_pSharedApplication = nullptr;

Application::Application()
{ 
	sm_pSharedApplication = this;
}

Application::~Application()
{
	OG_ASSERT(this == sm_pSharedApplication);
	sm_pSharedApplication = nullptr;
}

int Application::run()
{
	 


	// Initialize instance and cocos2d.
	if (!init())
	{
		return 1;
	}

	auto director = Director::getInstance();
	auto sdlview = director->getSDLView();

	// Retain glview to avoid glview being released in the while loop
	sdlview->retain();

 
	while (!director->EventHandle())
	{

		director->mainLoop();

	}

	// Director should still do a cleanup if the window was closed manually.
// 	if (sdlview->isOpenGLReady())
// 	{
// 		director->end();
// 		director->mainLoop();
// 		director = nullptr;
// 	}
	sdlview->release(); 
	return 0;
}
bool Application::openURL(const std::string &url)
{
	return 0 == SDL_OpenURL(url.c_str());
}
// void Application::setAnimationInterval(float interval)
// {
// 	 
// }

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::getInstance()
{
	OG_ASSERT(sm_pSharedApplication);
	return sm_pSharedApplication;
} 
 
OG_END
 

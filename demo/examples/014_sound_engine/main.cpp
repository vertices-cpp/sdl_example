
#include "AudioEngine.h"

#define PATH_RES
#include "path_head.h"

USING_OG;

int main(int argc, char *argv[])
{
	static SDLmanager* s = SDLmanager::getInstance();
	 
		OG::AudioEngine::getInstace()->play2d(og::checkPath("013_sound_pitching_example/0013.wav"));
 
	while (!s->event())
	{
		OG::AudioEngine::cleanupFinished();
	}

	return 0;
}
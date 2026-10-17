#ifndef _CALLBACK_COMMAND_H_
#define _CALLBACK_COMMAND_H_

 

#include "renderer/OGRenderCommand.h"

/**
 * @addtogroup renderer
 * @{
 */

OG_BEGIN
 
class CallbackCommand : public RenderCommand
{  
public:
    CallbackCommand();

    void init(float globalZOrder);
	 
    /**
     Execute the render command and call callback functions.
     */
    void execute();
    /**Callback function.*/
    std::function<void()> func;
};

/**
 * CaptureScreenCallbackCommand is used to capture the screen.
 */
class CaptureScreenCallbackCommand : public RenderCommand
{
	SDL_Texture* _texture = nullptr;
	SDL_Vertex*  _verts = nullptr;
	int*         _indices = nullptr;
	int _vertCount = 0;
	int _indexCount = 0;

	friend class Renderer;
public:
    CaptureScreenCallbackCommand();
    
    void init(float globalZOrder);
    
    /**
     * A callback function to do with the image after capture from the color buffer.
     */
    std::function<void(const unsigned char*, int, int)> func;
};

OG_END
/**
 end of support group
 @}
 */

#endif _CALLBACK_COMMAND_H_
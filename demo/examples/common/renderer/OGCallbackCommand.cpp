 
#include "renderer/OGCallbackCommand.h"
//#include "renderer/backend/Device.h"

OG_BEGIN

CallbackCommand::CallbackCommand()
{
    _type = RenderCommand::Type::CALLBACK_COMMAND;
}

void CallbackCommand::init(float globalOrder)
{
	_globalOrder = globalOrder;
}
 

void CallbackCommand::execute()
{
   if(func)
   {
       func();
   }
}

CaptureScreenCallbackCommand::CaptureScreenCallbackCommand()
{
    _type = RenderCommand::Type::CAPTURE_SCREEN_COMMAND;
}

void CaptureScreenCallbackCommand::init(float globalOrder)
{
   // backend::Device::getInstance()->setFrameBufferOnly(false);
    _globalOrder = globalOrder;
}

OG_END

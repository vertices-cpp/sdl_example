 
#include "OGRenderCommand.h"
#include "OGCamera.h"
#include "OGNode.h"


OG_BEGIN

RenderCommand::RenderCommand()
{
}

RenderCommand::~RenderCommand()
{
}

void RenderCommand::init(float globalZOrder)
{
    _globalOrder = globalZOrder;
//     if (flags & Node::FLAGS_RENDER_AS_3D)
//     {
//         if (Camera::getVisitingCamera())
//             _depth = Camera::getVisitingCamera()->getDepthInView(transform);
//         
//         set3D(true);
//     }
//     else
    {
       // set3D(false);
        _depth = 0;
    }
}

void RenderCommand::printID()
{
    printf("Command Depth: %f\n", _globalOrder);
}

OG_END

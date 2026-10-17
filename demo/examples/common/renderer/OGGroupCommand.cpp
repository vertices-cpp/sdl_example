 


#include "renderer/OGGroupCommand.h"
#include "renderer/OGRenderer.h"
#include "base/OGDirector.h"

OG_BEGIN
 

GroupCommand::GroupCommand()
{
    _type = RenderCommand::Type::GROUP_COMMAND;
}

void GroupCommand::init(float globalOrder)
{
    _globalOrder = globalOrder;
}

GroupCommand::~GroupCommand()
{
	_groupCommand.clear();
}

OG_END

 
#include "OGCustomCommand.h"
// #include "CCTextureAtlas.h"
// #include "backend/Buffer.h"
// #include "backend/Device.h"
//#include "ogUtils.h"

OG_BEGIN

CustomCommand::CustomCommand()
{
    _type = RenderCommand::Type::CUSTOM_COMMAND;
}

CustomCommand::~CustomCommand()
{ 
}

void CustomCommand::init(float globalOrder, SDL_Texture* texture,
	V2F_C4B_T2F* vertex, int* indices,
	int vertCount, int indexCount)
{
	_globalOrder = globalOrder;

	_texture = texture;
	_verts = reinterpret_cast<SDL_Vertex*>(vertex);
	_indices = indices;
	_vertCount = vertCount;
	_indexCount = indexCount;
}


OG_END

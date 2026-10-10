 
#include "OGTrianglesCommand.h"
//#include "xxhash.h"
#include "OGRenderer.h"
#include "OGTexture2D.h"
//#include "/ccUtils.h"

OG_BEGIN

TrianglesCommand::TrianglesCommand()
{
    _type = RenderCommand::Type::TRIANGLES_COMMAND;
}

void TrianglesCommand::init(float globalOrder, SDL_Texture* texture,const Triangles& triangles)
{
    RenderCommand::init(globalOrder);
    
    _triangles = triangles;
    if(_triangles._indexCount % 3 != 0)
    {
        unsigned int count = _triangles._indexCount;
        _triangles._indexCount = count / 3 * 3;
		OGLOGERROR("Resize indexCount from %d to %d, size must be multiple times of 3", count, _triangles._indexCount);
    }
	_texture = texture;
}
TrianglesCommand::~TrianglesCommand()
{
}


OG_END

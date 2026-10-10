 
#include "OGQuadCommand.h"
#include "OGTexture2D.h"

OG_BEGIN 

int QuadCommand::__indexCapacity = -1;
int* QuadCommand::__indices = nullptr;

QuadCommand::QuadCommand() :
	_indexSize(-1),
	_ownedIndices()
{
}

QuadCommand::~QuadCommand()
{
	for (auto& indices : _ownedIndices)
	{
		OG_SAFE_DELETE_ARRAY(indices);
	}
}


void QuadCommand::reIndex(int indicesCount)
{
	// first time init: create a decent buffer size for indices to prevent too much resizing
	if (__indexCapacity == -1)
	{
		indicesCount = std::max(indicesCount, 2048);
	}

	if (indicesCount > __indexCapacity)
	{
		// if resizing is needed, get needed size plus 25%, but not bigger that max size
		indicesCount *= 1.25;
		indicesCount = std::min(indicesCount, 65536);

		OGLOG("orange: QuadCommand: resizing index size from [%d] to [%d]", __indexCapacity, indicesCount);

		_ownedIndices.push_back(__indices);
		__indices = new (std::nothrow) int[indicesCount];
		__indexCapacity = indicesCount;
	}

	for (int i = 0; i < __indexCapacity / 6; i++)
	{
		__indices[i * 6 + 0] = (int)(i * 4 + 0);
		__indices[i * 6 + 1] = (int)(i * 4 + 1);
		__indices[i * 6 + 2] = (int)(i * 4 + 2);
		__indices[i * 6 + 3] = (int)(i * 4 + 1);
		__indices[i * 6 + 4] = (int)(i * 4 + 3);
		__indices[i * 6 + 5] = (int)(i * 4 + 2);
	}

	_indexSize = indicesCount;
}
 
void QuadCommand::init(float globalOrder, 
	SDL_Texture * texture, 
	V2F_C4B_T2F * vertex,
	int quadCount)
{

	if (quadCount * 6 > _indexSize)
		reIndex((int)quadCount * 6);

	Triangles triangles;
	triangles._verts = reinterpret_cast<SDL_Vertex*>(vertex);// &quads->tl;
	triangles._vertCount = (int)quadCount * 4;
	triangles._indices = __indices;
	triangles._indexCount = (int)quadCount * 6;
	TrianglesCommand::init(globalOrder, texture,triangles);

	//_alphaTextureID = texture->getAlphaTextureName();
}
OG_END
 

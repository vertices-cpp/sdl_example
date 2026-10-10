#ifndef _OG_CUSTOM_COMMAND_H_
#define _OG_CUSTOM_COMMAND_H_

#include "OGRenderCommand.h"

/**
 * @addtogroup renderer
 * @{
 */

OG_BEGIN
 
 
class CustomCommand : public RenderCommand
{
public:
	enum class DrawType { ARRAY, ELEMENT };

	CustomCommand();

	~CustomCommand();

	void init(float globalOrder,
		SDL_Texture* texture,
		V2F_C4B_T2F* vertex,
		int* indices,
		int vertCount,
		int indexCount); 

	SDL_Texture* getTexture() const { return _texture; }
	SDL_Vertex*  getVertices() const { return _verts; }
	int* getIndices() const { return _indices; }
	int getVertexCount() const { return _vertCount; }
	int getIndexCount() const { return _indexCount; }

protected:
	SDL_Texture* _texture = nullptr;
	SDL_Vertex*  _verts = nullptr;
	int*         _indices = nullptr;
	int _vertCount = 0;
	int _indexCount = 0;

	friend class Renderer;
};

OG_END
/**
 end of support group
 @}
 */

#endif
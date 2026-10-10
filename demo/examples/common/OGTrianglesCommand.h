#ifndef _TRIANGLES_COMMAND_H_
#define _TRIANGLES_COMMAND_H_

#include "OGRenderCommand.h" 

OG_BEGIN 

// namespace backend {
//     class TextureBackend;
//     class Program;
// }

class Texture2D; 

class TrianglesCommand : public RenderCommand
{
public: 
    struct Triangles
    {
        Triangles(SDL_Vertex* verts,int* indices,int vertCount,int indexCount)
        : _verts(verts)
        , _indices(indices)
        , _vertCount(vertCount)
        , _indexCount(indexCount)
        {}

        Triangles() {} 
		SDL_Vertex  * _verts = nullptr;
        int* _indices = nullptr; //SDL要用int
        int _vertCount = 0;
        int _indexCount = 0;
    }; 
    TrianglesCommand(); 
    ~TrianglesCommand(); 
	void init(float globalOrder, SDL_Texture* texture, const Triangles& triangles);
    uint32_t getMaterialID() const { return _materialID; } 
    const Triangles& getTriangles() const { return _triangles; } 
    size_t getVertexCount() const { return _triangles._vertCount; }
    size_t getIndexCount() const { return _triangles._indexCount; }
    const SDL_Vertex* getVertices() const { return _triangles._verts; }
    const int* getIndices() const { return _triangles._indices; }

  
protected:  
    uint32_t _materialID = 0; 
    Triangles _triangles; 
    uint8_t _alphaTextureID = 0;  
  
	SDL_Texture* _texture = nullptr;

	friend class Renderer;
};

OG_END
/**
 end of support group
 @}
 */

#endif
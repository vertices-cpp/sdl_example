#ifndef _MY_OG_QUAD_COMMAND_H_
#define _MY_OG_QUAD_COMMAND_H_

#include <vector>
#include <algorithm> 

#include "OGTrianglesCommand.h"

OG_BEGIN 

struct QuadCommand : public TrianglesCommand
{  
	int *_indices = nullptr;
	int _vertCount = 0;
	int _indexCount = 0;
	float _globalOrder = 0.f;
public:
	QuadCommand();

	~QuadCommand();

	void init(float globalOrder, SDL_Texture * texture, V2F_C4B_T2F * vertex,
		int quadCount);
protected:
	void reIndex(int indicesCount);

	int _indexSize;
	std::vector<int*> _ownedIndices;

	// shared across all instances
	static int __indexCapacity;
	static int* __indices;

};
 

OG_END
#endif

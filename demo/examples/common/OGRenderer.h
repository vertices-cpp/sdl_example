#ifndef _MY_OG_RENDERER_H_
#define _MY_OG_RENDERER_H_

#include <vector>
#include <algorithm>
#include "SDLView.h"
#include "OGTexture2D.h"

OG_BEGIN 

struct QuadCommand
{
	SDL_Texture *_texture = nullptr;
	SDL_Vertex *_vertex = nullptr;
	int *_indices = nullptr;
	int _vertCount = 0;
	int _indexCount = 0;
	float _globalOrder = 0.f;
public:
	QuadCommand() = default;

	QuadCommand(float globalOrder, SDL_Texture * texture, V2F_C4B_T2F * vertex, int *indices,
		int vertCount, int indexCount)
	{
		init(globalOrder, texture, vertex, indices, vertCount, indexCount);
	}

	void init(float globalOrder, SDL_Texture * texture, V2F_C4B_T2F * vertex, int *indices,
		int vertCount, int indexCount)
	{
		_globalOrder = globalOrder;
		_texture = texture;
		_vertex = (SDL_Vertex*)vertex;
		_indices = indices;
		_vertCount = vertCount;
		_indexCount = indexCount;
	}
	float getGlobalOrder() { return _globalOrder; }
	QuadCommand(const QuadCommand & rhs)
	{
		_globalOrder = rhs._globalOrder;
		_texture = rhs._texture, _vertex = rhs._vertex, _indices = rhs._indices,
			_vertCount = rhs._vertCount, _indexCount = rhs._indexCount;
	}
	QuadCommand & operator=(const QuadCommand & rhs)
	{
		if (this != &rhs)
		{
			_globalOrder = rhs._globalOrder;
			_texture = rhs._texture, _vertex = rhs._vertex, _indices = rhs._indices,
				_vertCount = rhs._vertCount, _indexCount = rhs._indexCount;
		}
		return *this;
	}

};

class Renderer
{
	enum QUEUE_GROUP
	{
		GLOBALZ_NEG = 0,		// _commands[0]
		GLOBALZ_ZERO,			// _commands[1]
		GLOBALZ_POS,			// _commands[2]
		QUEUE_COUNT				// = 3
	};

	  std::vector < QuadCommand * >_commands[QUEUE_COUNT];	// ← 3 个
 
	SDL_Renderer *_render;
	size_t _TotalCalls, _TotalVertex = 0, _TotalIndex = 0;
  public:
	  Instance(Renderer);
	  Renderer() = default;
	void clearStats()
	{
		_TotalCalls = _TotalVertex = _TotalIndex = 0;
	}
	size_t getTotalCalls() const
	{
		return _TotalCalls;
	}
	size_t getTotalVertex() const
	{
		return _TotalVertex;
	}
	void init(SDL_Renderer * ren)
	{
		_render = ren;
	}
	void addCommand(QuadCommand * cmd)
	{
		float z = cmd->getGlobalOrder();
		int g = (z < 0) ? 0 : (z > 0 ? 2 : 1);
		_commands[g].push_back(cmd);

	}
	static bool cmp(QuadCommand * a, QuadCommand * b)
	{
		return a->_globalOrder < b->_globalOrder;

	}
	void draw(QuadCommand * cmd)
	{
		// 一次 SDL_RenderGeometry 提交一个 quad
		SDL_RenderGeometry(_render,
			cmd->_texture,
			cmd->_vertex, cmd->_vertCount, cmd->_indices, cmd->_indexCount);
		// 统计
		++_TotalCalls;
		_TotalVertex += cmd->_vertCount;
		_TotalIndex += cmd->_indexCount;
	}
	void render()
	{


		std::stable_sort(_commands[0].begin(), _commands[0].end(), cmp);
		std::stable_sort(_commands[2].begin(), _commands[2].end(), cmp);
		// _commands[1] 不排

		for (int g = 0; g < 3; ++g)
		  for (auto * c:_commands[g])
				draw(c);

		for (int g = 0; g < 3; ++g)
			_commands[g].clear();

	}

};



OG_END
#endif

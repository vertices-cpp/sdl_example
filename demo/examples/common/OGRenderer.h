#ifndef _MY_OG_RENDERER_H_
#define _MY_OG_RENDERER_H_

#include <vector>
#include <algorithm>

#include "OGRenderCommand.h"


OG_BEGIN

class TrianglesCommand;
class CustomCommand;

class Renderer
{
	enum QUEUE_GROUP
	{
		GLOBALZ_NEG = 0,		// _commands[0]
		GLOBALZ_ZERO,			// _commands[1]
		GLOBALZ_POS,			// _commands[2]
		QUEUE_COUNT				// = 3
	};

	  std::vector < RenderCommand * >_commands[QUEUE_COUNT];	// ← 3 个
 
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
	void addCommand(RenderCommand * cmd)
	{
		float z = cmd->getGlobalOrder();
		int g = (z < 0) ? 0 : (z > 0 ? 2 : 1);
		_commands[g].push_back(cmd);

	}
	static bool cmp(RenderCommand * a, RenderCommand * b)
	{
		return a->getGlobalOrder() < b->getGlobalOrder();

	}
	void drawTriangles(TrianglesCommand * cmd);
	void drawCustom(CustomCommand * cmd);
	void processRenderCommand(RenderCommand * command);
	void render();

};



OG_END
#endif

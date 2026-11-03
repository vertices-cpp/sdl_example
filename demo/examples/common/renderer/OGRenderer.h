#ifndef _MY_OG_RENDERER_H_
#define _MY_OG_RENDERER_H_

#include <vector>
#include <algorithm>
#include <stack>

#include "renderer/OGRenderCommand.h"
#include "platform/SDLView.h"

OG_BEGIN

class TrianglesCommand;
class CustomCommand;
class CallbackCommand;
class GroupCommand;
class GroupCommandManager;

class RenderQueue
{
public:
	enum QUEUE_GROUP
	{
		GLOBALZ_NEG = 0,		// _commands[0]
		GLOBALZ_ZERO,			// _commands[1]
		GLOBALZ_POS,			// _commands[2]
		QUEUE_COUNT				// = 3
	};
	std::vector < RenderCommand * >_commands[QUEUE_COUNT];	// ← 3 个
public:
 
	RenderQueue() {} 
	void addCommand(RenderCommand * cmd)
	{
		float z = cmd->getGlobalOrder();
		int g = (z < 0) ? 0 : (z > 0 ? 2 : 1);
		_commands[g].push_back(cmd);

	}
	void push_back(RenderCommand* command);
 
	size_t size() const;
 
	void sort();
 
	RenderCommand* operator[](size_t index) const;
 
	void clear();
 
	void realloc(size_t reserveSize);
 
	std::vector<RenderCommand*>& getSubQueue(QUEUE_GROUP group) { return _commands[group]; }
 
};

class Renderer
{
	 

	SDL_Renderer *_render;
	size_t _TotalCalls, _TotalVertex = 0, _TotalIndex = 0;
	std::vector<RenderQueue> _renderGroups;

	std::stack<int> _commandGroupStack;
	GroupCommandManager* _groupCommandManager = nullptr;
public:
	Instance(Renderer);
	Renderer();
	~Renderer();

	void init(SDL_Renderer * ren);

	int createRenderQueue();

	inline GroupCommandManager * getGroupCommandManager() const { return _groupCommandManager; }
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

	
	 
	void addCommand(RenderCommand * command);
	void addCommand(RenderCommand * command, int renderQueueID);
	void pushGroup(int renderQueueID);
	void popGroup();

	void drawTriangles(TrianglesCommand * cmd);
	void drawCustom(CustomCommand * cmd);
	void drawCallBack(CallbackCommand * cmd);
	void processGroupCommand(GroupCommand * command);
	void processRenderCommand(RenderCommand * command);

	
	
	void doVisitRenderQueue(const std::vector<RenderCommand*>& renderCommands);
	void visitRenderQueue(RenderQueue & queue);
	void render();

	void clean();

protected:
// 	struct ScissorState
// 	{
// 		ScissorRect rect;
// 		bool isEnabled = false;
// 	};
// 	ScissorState _scissorState;
// public:
// 	void setScissorRect(float x, float y, float w, float h);
// 	const ScissorRect & getScissorRect() const;
// 	bool getScissorTest();
// 	void setScissorTest(bool enabled);
};



OG_END
#endif

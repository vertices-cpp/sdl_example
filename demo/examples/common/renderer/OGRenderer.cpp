
#include "renderer/OGRenderer.h"

#include "base/ogMacros.h"

#include "platform/SDLView.h"
#include "renderer/OGTrianglesCommand.h"
#include "renderer/OGCustomCommand.h"
#include "renderer/OGCallbackCommand.h"
#include "renderer/OGGroupCommand.h"

#include "renderer/OGTexture2D.h"


OG_BEGIN
  

void RenderQueue::push_back(RenderCommand* command)
{
	float z = command->getGlobalOrder();
	if (z < 0)
	{
		_commands[QUEUE_GROUP::GLOBALZ_NEG].push_back(command);
	}
	else if (z > 0)
	{
		_commands[QUEUE_GROUP::GLOBALZ_POS].push_back(command);
	}
	else
	{
		_commands[QUEUE_GROUP::GLOBALZ_ZERO].push_back(command);
	}
}
size_t RenderQueue::size() const
{
	size_t result(0);
	for (int index = 0; index < QUEUE_GROUP::QUEUE_COUNT; ++index)
	{
		result += _commands[index].size();
	}

	return result;
}
RenderCommand* RenderQueue::operator[](size_t index) const
{
	for (int queIndex = 0; queIndex < QUEUE_GROUP::QUEUE_COUNT; ++queIndex)
	{
		if (index < static_cast<ssize_t>(_commands[queIndex].size()))
			return _commands[queIndex][index];
		else
		{
			index -= _commands[queIndex].size();
		}
	}

	OGASSERT(false, "invalid index");
	return nullptr;
}
static bool compareRenderCommand(RenderCommand * a, RenderCommand * b)
{
	return a->getGlobalOrder() < b->getGlobalOrder();

}

void RenderQueue::sort()
{
	// Don't sort _queue0, it already comes sorted
	 std::stable_sort(std::begin(_commands[QUEUE_GROUP::GLOBALZ_NEG]), std::end(_commands[QUEUE_GROUP::GLOBALZ_NEG]), compareRenderCommand);
	std::stable_sort(std::begin(_commands[QUEUE_GROUP::GLOBALZ_POS]), std::end(_commands[QUEUE_GROUP::GLOBALZ_POS]), compareRenderCommand);
}
void RenderQueue::clear()
{
	for (int i = 0; i < QUEUE_GROUP::QUEUE_COUNT; ++i)
	{
		_commands[i].clear();
	}
}
void RenderQueue::realloc(size_t reserveSize)
{
	for (int i = 0; i < QUEUE_GROUP::QUEUE_COUNT; ++i)
	{
		_commands[i] = std::vector<RenderCommand*>();
		_commands[i].reserve(reserveSize);
	}
}


Renderer::Renderer()
{
	_groupCommandManager = new (std::nothrow) GroupCommandManager();
	_commandGroupStack.push(0);
	RenderQueue defaultRenderQueue;
	_renderGroups.push_back(defaultRenderQueue);
}

Renderer::~Renderer()
{
	_renderGroups.clear();
	_groupCommandManager->release();
}
void Renderer::init(SDL_Renderer * ren)
{
	_render = ren;
	_groupCommandManager->init();
}
int Renderer::createRenderQueue()
{
	RenderQueue newRenderQueue;
	_renderGroups.push_back(newRenderQueue);
	return (int)_renderGroups.size() - 1;
}

void Renderer::addCommand(RenderCommand* command)
{
	int renderQueueID = _commandGroupStack.top();
	addCommand(command, renderQueueID);
}

void Renderer::addCommand(RenderCommand* command, int renderQueueID)
{
// 	OGASSERT(!_isRendering, "Cannot add command while rendering");
// 	OGASSERT(renderQueueID >= 0, "Invalid render queue");
// 	OGASSERT(command->getType() != RenderCommand::Type::UNKNOWN_COMMAND, "Invalid Command Type");

	_renderGroups[renderQueueID].push_back(command);
}


void Renderer::pushGroup(int renderQueueID)
{
	//OGASSERT(!_isRendering, "Cannot change render queue while rendering");
	_commandGroupStack.push(renderQueueID);
}

void Renderer::popGroup()
{
	//OGASSERT(!_isRendering, "Cannot change render queue while rendering");
	_commandGroupStack.pop();
}


void Renderer::drawTriangles(TrianglesCommand * cmd)
{
	// 一次 SDL_RenderGeometry 提交一个 quad
	SDL_RenderGeometry(_render,
		cmd->_texture,
		cmd->_triangles._verts,
		cmd->_triangles._vertCount,
		cmd->_triangles._indices,
		cmd->_triangles._indexCount);
	// 统计
	++_TotalCalls;

	_TotalVertex += cmd->_triangles._vertCount;
	_TotalIndex += cmd->_triangles._indexCount;
}
void Renderer::drawCustom(CustomCommand * cmd)
{
	// 一次 SDL_RenderGeometry 提交一个 quad
	SDL_RenderGeometry(_render,
		cmd->_texture,
		cmd->_verts,
		cmd->_vertCount,
		cmd->_indices,
		cmd->_indexCount);
	// 统计
	++_TotalCalls;

	_TotalVertex += cmd->_vertCount;
	_TotalIndex += cmd->_indexCount;
}
void Renderer::drawCallBack(CallbackCommand * cmd)
{
	cmd->execute();
}

void Renderer::processGroupCommand(GroupCommand* command)
{
	OGASSERT(
		(command->beforeDraw == nullptr) == (command->afterDraw == nullptr),
		"GroupCommand: beforeDraw and afterDraw must be set as a pair");

	if (command->beforeDraw != nullptr)
	{
		command->beforeDraw();
	}
	int renderQueueID = ((GroupCommand*)command)->getRenderQueueID();
	visitRenderQueue(_renderGroups[renderQueueID]);
	if (command->afterDraw != nullptr)
	{
		command->afterDraw();
	}
}
void Renderer::processRenderCommand(RenderCommand* command)
{
	auto commandType = command->getType();
	switch (commandType)
	{
	case RenderCommand::Type::TRIANGLES_COMMAND:
	{
		auto cmd = static_cast<TrianglesCommand*>(command);
		drawTriangles(cmd);
	}
	break;
	case RenderCommand::Type::CUSTOM_COMMAND:
	{
		auto cmd = static_cast<CustomCommand*>(command);
		drawCustom(cmd);
	}
	break;
	case RenderCommand::Type::CALLBACK_COMMAND:
	{
		auto cmd = static_cast<CallbackCommand*>(command);
		drawCallBack(cmd);
	}
	break;
	case RenderCommand::Type::GROUP_COMMAND:
	{
		auto groupCmd = static_cast<GroupCommand*>(command);
		processGroupCommand(groupCmd);
	}
	break;
	}

}




void Renderer::doVisitRenderQueue(const std::vector<RenderCommand*>& renderCommands)
{
	for (const auto& command : renderCommands)
	{
		processRenderCommand(command);
	}
 
}

void Renderer::visitRenderQueue(RenderQueue& queue)
{ 
	//Process Global-Z < 0 Objects 
	doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_NEG)); 
  //Process Global-Z = 0 Queue 
	doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_ZERO)); 
	//Process Global-Z > 0 Queue 
	doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_POS)); 
}
void Renderer::render()
{
	// _commands[1] 不排

	for (auto &renderqueue : _renderGroups)
	{
		renderqueue.sort();
	}
	visitRenderQueue(_renderGroups[0]);

	clean();
	//	_isRendering = false;
}

void Renderer::clean()
{
	// Clear render group
	for (size_t j = 0, size = _renderGroups.size(); j < size; j++)
	{ 
		_renderGroups[j].clear();
	}
	 
}


// 
// 
// const ScissorRect& Renderer::getScissorRect() const
// {
// 	return _scissorState.rect;
// }
// bool Renderer::getScissorTest() {
// 	return _scissorState.isEnabled;
// }
// void Renderer::setScissorTest(bool enabled)
// {
// 	_scissorState.isEnabled = enabled;
// }
// void Renderer::setScissorRect(float x, float y, float w, float h)
//{
//	_scissorState.rect.x = x;
//	_scissorState.rect.y = y;
//	_scissorState.rect.width = w;
//	_scissorState.rect.height = h;
//}


OG_END


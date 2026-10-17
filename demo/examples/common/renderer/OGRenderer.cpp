
#include "renderer/OGRenderer.h"

#include "platform/SDLView.h"
#include "renderer/OGTrianglesCommand.h"
#include "renderer/OGCustomCommand.h"
#include "renderer/OGCallbackCommand.h"
#include "renderer/OGGroupCommand.h"

#include "renderer/OGTexture2D.h"


OG_BEGIN


static bool cmp(TrianglesCommand * a, TrianglesCommand * b)
{
	return a->getGlobalOrder() < b->getGlobalOrder();

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
		const auto& cmds = groupCmd->getGroupCommand();
		for (auto cmd : cmds)
			processRenderCommand(cmd);
	}
	break;
	}

}
void Renderer::render()
{


	std::stable_sort(_commands[0].begin(), _commands[0].end(), cmp);
	std::stable_sort(_commands[2].begin(), _commands[2].end(), cmp);
	// _commands[1] 不排

	for (int g = 0; g < 3; ++g)
		for (auto * c : _commands[g])
			processRenderCommand(c);

	for (int g = 0; g < 3; ++g)
		_commands[g].clear();

}






const ScissorRect& Renderer::getScissorRect() const
{
	return _scissorState.rect;
}
bool Renderer::getScissorTest() {
	return _scissorState.isEnabled;
}
void Renderer::setScissorTest(bool enabled)
{
	_scissorState.isEnabled = enabled;
}
void Renderer::setScissorInPoints(float x, float y, float w, float h)
{
	_scissorState.rect.x = x;
	_scissorState.rect.y = y;
	_scissorState.rect.width = w;
	_scissorState.rect.height = h;
}


OG_END


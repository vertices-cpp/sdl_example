 
#include "OGRenderer.h"

#include "SDLView.h"
#include "OGTrianglesCommand.h"
#include "OGCustomCommand.h"
#include "OGTexture2D.h"


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
		auto cmd = static_cast<CustomCommand*>(command);
		drawCustom(cmd);
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
 
OG_END 


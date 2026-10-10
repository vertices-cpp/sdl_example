#ifndef _RENDER_COMMAND_H_
#define _RENDER_COMMAND_H_

#include "OGPlatformMacros.h"
#include "ogTypes.h" 

OG_BEGIN

#ifdef ORANGE_DEBUG
class Node;
#endif

class RenderCommand
{
public:
	enum class Type
	{
		UNKNOWN_COMMAND,
		QUAD_COMMAND,
		CUSTOM_COMMAND,
		GROUP_COMMAND,
		MESH_COMMAND,
		TRIANGLES_COMMAND,
		CALLBACK_COMMAND,
		CAPTURE_SCREEN_COMMAND
	};
	void init(float globalZOrder);
	float getGlobalOrder() const { return _globalOrder; }
	Type getType() const { return _type; }
	float getDepth() const { return _depth; }

#ifdef ORANGE_DEBUG
protected:
	Node* _owner = nullptr;
public:
	void setOwner(Node* n) { _owner = n; }
	Node* getOwner() const { return _owner; }
#endif 

protected:
	RenderCommand();
	virtual ~RenderCommand();
	void printID();
	Type _type = RenderCommand::Type::UNKNOWN_COMMAND;
	float _globalOrder = 0.f;
	float _depth = 0.f;
};

OG_END
#endif
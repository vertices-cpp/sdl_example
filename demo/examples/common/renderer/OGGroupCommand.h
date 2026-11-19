


#ifndef _OG_GROUP_COMMAND_H_
#define _OG_GROUP_COMMAND_H_

#include <vector>
#include <unordered_map>

#include "base/OGRef.h"
#include "renderer/OGRenderCommand.h"

/**
 * @addtogroup renderer
 * @{
 */

OG_BEGIN


//Used for internal
class GroupCommandManager : public Ref
{
public:
	int getGroupID();
	void releaseGroupID(int groupID);

protected:
	friend class Renderer;
	GroupCommandManager();
	~GroupCommandManager();
	bool init();
	std::unordered_map<int, bool> _groupMapping;
	std::vector<int> _unusedIDs;
};

class GroupCommand : public RenderCommand
{
public: 
	GroupCommand();
	~GroupCommand();
 
	void init(float globalOrder);
	 
	int getRenderQueueID() const { return _renderQueueID; }
	std::function<void()> beforeDraw;   // 可选，空 = 不做
	std::function<void()> afterDraw;    // 可选，空 = 不做
protected:
	int _renderQueueID;
};

OG_END

/**
 end of support group
 @}
 */
#endif //_OG_GROUPCOMMAND_H_

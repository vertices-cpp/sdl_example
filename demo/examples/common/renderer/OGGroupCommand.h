 


#ifndef _OG_GROUPCOMMAND_H_
#define _OG_GROUPCOMMAND_H_

#include <vector>
#include <unordered_map>

#include "base/OGRef.h"
#include "renderer/OGRenderCommand.h"

/**
 * @addtogroup renderer
 * @{
 */

 OG_BEGIN
	  
class GroupCommand : public RenderCommand
{
	std::vector< RenderCommand*> _groupCommand;
public:
    /**@{
     Constructor and Destructor.
     */
    GroupCommand();
    ~GroupCommand();
    /**@}*/
    //不排序,按顺序处理
	void addCommand(RenderCommand *cmd) {
		_groupCommand.push_back(cmd);
	}
	std::vector< RenderCommand*> &getGroupCommand() { return _groupCommand; }
    /**Init function for group command*/
    void init(float globalOrder);
    
    /**called by renderer, get the group ID.*/
    int getRenderQueueID() const { return _renderQueueID; }
    
protected:
    int _renderQueueID;
};

OG_END

/**
 end of support group
 @}
 */
#endif //_OG_GROUPCOMMAND_H_

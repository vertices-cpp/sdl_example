 

#ifndef _OG_TABLE_VIEWCELL_H_
#define _OG_TABLE_VIEWCELL_H_

//#include "extensions/ExtensionMacros.h"
#include "2d/OGNode.h"
//#include "extensions/ExtensionExport.h"

/**
 * @addtogroup ui
 * @{
 */
OG_BEGIN


/**
 * Abstract class for SWTableView cell node
 */
class TableViewCell: public Node
{
public:
    CREATE_FUNC(TableViewCell);
    
    TableViewCell() {}
    /**
     * The index used internally by SWTableView and its subclasses
     */
    ssize_t getIdx() const;
    void setIdx(ssize_t uIdx);
    /**
     * Cleans up any resources linked to this cell and resets <code>idx</code> property.
     */
    void reset();

private:
    ssize_t _idx;
};

OG_END
// end of ui group
/// @}

#endif /* _OG_TABLE_VIEWCELL_H_ */

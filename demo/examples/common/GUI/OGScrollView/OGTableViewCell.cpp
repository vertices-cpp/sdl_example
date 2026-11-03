 

#include "OGTableViewCell.h"

OG_BEGIN

void TableViewCell::reset()
{
    _idx = -1;
}

ssize_t TableViewCell::getIdx() const
{
    return _idx;
}

void TableViewCell::setIdx(ssize_t idx)
{
    _idx = idx;
}

OG_END

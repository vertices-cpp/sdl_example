

#ifndef __UIPAGEVIEWINDICATOR_H__
#define __UIPAGEVIEWINDICATOR_H__

#include "ui/UIPageView.h"
#include "2d/OGSprite.h"

OG_BEGIN
/**
 * @addtogroup ui
 * @{
 */

namespace ui {

class PageViewIndicator : public ProtectedNode
{

public:
    /**
     * Create a page view indicator with its parent page view.
     * @return A page view indicator instance.
     */
    static PageViewIndicator* create();

    PageViewIndicator();
    virtual ~PageViewIndicator();

    void setDirection(PageView::Direction direction);
    void reset(ssize_t numberOfTotalPages);
    void indicate(ssize_t index);
    void clear();
    void setSpaceBetweenIndexNodes(float spaceBetweenIndexNodes);
    float getSpaceBetweenIndexNodes() const { return _spaceBetweenIndexNodes; }
    void setSelectedIndexColor(const Color3B& color) { _currentIndexNode->setColor(color); }
    const Color3B& getSelectedIndexColor() const { return _currentIndexNode->getColor(); }
    void setIndexNodesColor(const Color3B& indexNodesColor);
    const Color3B& getIndexNodesColor() const { return _indexNodesColor; }
    void setIndexNodesScale(float indexNodesScale);
    float getIndexNodesScale() const { return _indexNodesScale; }
    void setSelectedIndexOpacity(uint8_t opacity) { _currentIndexNode->setOpacity(opacity); }
    uint8_t getSelectedIndexOpacity() const { return _currentIndexNode->getOpacity(); }
    void setIndexNodesOpacity(uint8_t opacity);
    uint8_t getIndexNodesOpacity() const { return _indexNodesOpacity; }
    
    /**
     * Sets texture for index nodes.
     *
     * @param fileName   File name of texture.
     * @param resType    @see TextureResType .
     */
    void setIndexNodesTexture(const std::string& texName,Widget::TextureResType texType = Widget::TextureResType::LOCAL);
    
protected:
    bool init() override;
    void increaseNumberOfPages();
    void decreaseNumberOfPages();
    void rearrange();

    PageView::Direction _direction;
    Vector<Sprite*> _indexNodes;
    Sprite* _currentIndexNode;
    Sprite* _currentOverlappingIndexNode;
    float _spaceBetweenIndexNodes;
    float _indexNodesScale;
    Color3B _indexNodesColor;
    uint8_t _indexNodesOpacity;
    
    bool _useDefaultTexture;
    std::string _indexNodesTextureFile;
    Widget::TextureResType _indexNodesTexType;
};

}
// end of ui group
/// @}
OG_END

#endif /* defined(__UIPAGEVIEWINDICATOR_H__) */

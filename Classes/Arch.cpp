#include <vector>
#include "Arch.h"
#include "CoordAdaptor.h"

USING_NS_CC;

Arch* Arch::create(const ArchData& data, BaseMap* base_map)
{
    Arch* pRet = new(std::nothrow) Arch(data, base_map);
    if (pRet && pRet->initWithFile(kArchInfo.at(data.no_)[data.level_ - 1].image_)) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool Arch::initWithFile(const std::string& filename)
{
    if (!Sprite::initWithFile(filename)) {
        return false;
    }
    setAnchorPoint(Vec2(0.5f, 0.4f));
    unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
    float scale = 1.5f * CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(size, 0)).x / this->getContentSize().width;
    setScale(scale);
    setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(x_ + size / 2.0f, y_ + size / 2.0f)));
    base_map_->sprites_.push_back(this);
    base_map_->addChild(base_map_->sprites_.back(), 2);
    return true;
}
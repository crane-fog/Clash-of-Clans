#ifndef __COORD_ADAPTOR_H__
#define __COORD_ADAPTOR_H__

#include "cocos2d.h"

// 坐标转换
class CoordAdaptor {
public:
    // 格子坐标转像素坐标
    inline static cocos2d::Vec2 cellToPixel(const cocos2d::Node* const base_map, const cocos2d::Vec2& original)
    {
        float cell_width_zero = base_map->getContentSize().width * 0.17813765f;
        float cell_height_zero = base_map->getContentSize().height * 0.53831041f;
        float cell_width = base_map->getContentSize().width * 0.00757575f;
        float cell_height = base_map->getContentSize().height * 0.00826040f;
        return cocos2d::Vec2((original.x + original.y) * cell_width + cell_width_zero, (original.y - original.x) * cell_height + cell_height_zero);
    }

    // 格子坐标移动量转像素坐标移动量
    inline static cocos2d::Vec2 cellDeltaToPixelDelta(const cocos2d::Node* const base_map, const cocos2d::Vec2& delta)
    {
        float cell_width = base_map->getContentSize().width * 0.00757575f;
        float cell_height = base_map->getContentSize().height * 0.00826040f;
        return cocos2d::Vec2((delta.x + delta.y) * cell_width, (delta.y - delta.x) * cell_height);
    }
};

#endif // __COORD_ADAPTOR_H__
#ifndef __COORD_ADAPTOR_H__
#define __COORD_ADAPTOR_H__

#include "cocos2d.h"

// 坐标转换
class CoordAdaptor {
public:
    // 像素坐标转格子坐标
    inline static cocos2d::Vec2 pixelToCell(const cocos2d::Node* const base_map, const cocos2d::Vec2& original)
    {
        float cell_width_zero = base_map->getContentSize().width * 0.17813765f;
        float cell_height_zero = base_map->getContentSize().height * 0.53831041f;
        float cell_width = base_map->getContentSize().width * 0.00757575f;
        float cell_height = base_map->getContentSize().height * 0.00826040f;
        return cocos2d::Vec2((original.x - cell_width_zero) / cell_width, (original.y - cell_height_zero) / cell_height);
    }

    // 格子坐标转像素坐标
    inline static cocos2d::Vec2 cellToPixel(const cocos2d::Node* const base_map, const cocos2d::Vec2& original)
    {
        float cell_width_zero = base_map->getContentSize().width * 0.17813765f;
        float cell_height_zero = base_map->getContentSize().height * 0.53831041f;
        float cell_width = base_map->getContentSize().width * 0.00757575f;
        float cell_height = base_map->getContentSize().height * 0.00826040f;
        return cocos2d::Vec2(original.x * cell_width + cell_width_zero, original.y * cell_height + cell_height_zero);
    }

    // 像素坐标移动量转格子坐标移动量
    inline static cocos2d::Vec2 pixelDeltaToCellDelta(const cocos2d::Node* const base_map, const cocos2d::Vec2& delta)
    {
        float cell_width = base_map->getContentSize().width * 0.00757575f;
        float cell_height = base_map->getContentSize().height * 0.00826040f;
        return cocos2d::Vec2(delta.x / cell_width, delta.y / cell_height);
    }

    // 格子坐标移动量转像素坐标移动量
    inline static cocos2d::Vec2 cellDeltaToPixelDelta(const cocos2d::Node* const base_map, const cocos2d::Vec2& delta)
    {
        float cell_width = base_map->getContentSize().width * 0.00757575f;
        float cell_height = base_map->getContentSize().height * 0.00826040f;
        return cocos2d::Vec2(delta.x * cell_width, delta.y * cell_height);
    }
};

#endif // __COORD_ADAPTOR_H__
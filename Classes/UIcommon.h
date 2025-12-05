#ifndef __UICOMMON_H__
#define __UICOMMON_H__

#include "ShopPopup.h"
#include"UIparts.h"
#include "cocos/ui/CocosGUI.h"
//边框绘制，bg：当前框；borderWidth = 3.0f:边框宽度；Color4F colorBorder:边框颜色
inline void draw_border(cocos2d::LayerColor * bg, float borderWidth = 3.0f, cocos2d::Color4F colorBorder = cocos2d::Color4F(1.0f, 0.8f, 0.0f, 1.0f),int gap=0) {
    // 添加边框
    auto border = cocos2d::DrawNode::create();

    // 基于bg的实际尺寸
    cocos2d::Size bgSize = bg->getContentSize();

    // 绘制四条边（使用bg的尺寸）
    // 上边
    border->drawSegment(cocos2d::Vec2(0, bgSize.height),
        cocos2d::Vec2(bgSize.width, bgSize.height),
        borderWidth, colorBorder);
    // 右边
    border->drawSegment(cocos2d::Vec2(bgSize.width, bgSize.height),
        cocos2d::Vec2(bgSize.width, 0),
        borderWidth, colorBorder);
    // 下边
    border->drawSegment(cocos2d::Vec2(bgSize.width, 0),
        cocos2d::Vec2(0, 0),
        borderWidth, colorBorder);
    // 左边
    border->drawSegment(cocos2d::Vec2(0, 0),
        cocos2d::Vec2(0, bgSize.height),
        borderWidth, colorBorder);

    border->setPosition(cocos2d::Vec2::ZERO);  // 相对于bg的本地坐标系
    bg->addChild(border, 100);
}
#endif // __UICOMMON_H__
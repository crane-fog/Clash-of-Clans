#include "UIcommon.h"

void drawBorder(cocos2d::LayerColor* bg, float borderWidth, cocos2d::Color4F colorBorder, int gap)
{
    // 添加边框
    auto border = cocos2d::DrawNode::create();
    border->setName("border");
    // 基于bg的实际尺寸
    cocos2d::Size bg_size = bg->getContentSize();

    // 绘制四条边（使用bg的尺寸）
    // 上边
    border->drawSegment(cocos2d::Vec2(0, bg_size.height), cocos2d::Vec2(bg_size.width, bg_size.height), borderWidth,
                        colorBorder);
    // 右边
    border->drawSegment(cocos2d::Vec2(bg_size.width, bg_size.height), cocos2d::Vec2(bg_size.width, 0), borderWidth,
                        colorBorder);
    // 下边
    border->drawSegment(cocos2d::Vec2(bg_size.width, 0), cocos2d::Vec2(0, 0), borderWidth, colorBorder);
    // 左边
    border->drawSegment(cocos2d::Vec2(0, 0), cocos2d::Vec2(0, bg_size.height), borderWidth, colorBorder);

    border->setPosition(cocos2d::Vec2::ZERO);  // 相对于bg的本地坐标系
    bg->addChild(border, 100);
}

void removeBorder(cocos2d::LayerColor* bg)
{
    // 通过子节点移除已添加的边框
    auto child = bg->getChildByName("border");
    bg->removeChild(child);  // 移除边框（DrawNode）
}
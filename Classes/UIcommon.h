#ifndef __UICOMMON_H__
#define __UICOMMON_H__

#include "cocos/ui/CocosGUI.h"
#include "ShopPopup.h"
#include "UIparts.h"

using namespace cocos2d::ui;

// 为指定节点绘制边框
//
// @param bg 需要绘制边框的目标节点
// @param borderWidth 边框的宽度
// @param colorBorder 边框的颜色，使用Color4F格式
void drawBorder(cocos2d::LayerColor* bg, float borderWidth = 3.0f,
                cocos2d::Color4F colorBorder = cocos2d::Color4F(1.0f, 0.8f, 0.0f, 1.0f), int gap = 0);

// 为指定节点去除边框
void removeBorder(cocos2d::LayerColor* bg);

#endif  // __UICOMMON_H__
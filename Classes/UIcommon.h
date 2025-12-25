#ifndef __UICOMMON_H__
#define __UICOMMON_H__

#include "cocos/ui/CocosGUI.h"
#include "ShopPopup.h"
#include "UIparts.h"

using namespace cocos2d::ui;

// 边框绘制，bg：当前框；borderWidth = 3.0f:边框宽度；Color4F colorBorder:边框颜色
void drawBorder(cocos2d::LayerColor* bg, float borderWidth = 3.0f,
                cocos2d::Color4F colorBorder = cocos2d::Color4F(1.0f, 0.8f, 0.0f, 1.0f), int gap = 0);

// 去除边框
void removeBorder(cocos2d::LayerColor* bg);

#endif  // __UICOMMON_H__
#ifndef __UICOMMON_H__
#define __UICOMMON_H__

#include "cocos/ui/CocosGUI.h"
#include "ShopPopup.h"
#include "UIparts.h"
USING_NS_CC;
using namespace ui;
// 边框绘制，bg：当前框；borderWidth = 3.0f:边框宽度；Color4F colorBorder:边框颜色
inline void drawBorder(cocos2d::LayerColor* bg, float borderWidth = 3.0f,
                        cocos2d::Color4F colorBorder = cocos2d::Color4F(1.0f, 0.8f, 0.0f, 1.0f), int gap = 0)
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

// 去除边框
inline void removeBorder(cocos2d::LayerColor* bg)
{
    // 通过子节点移除已添加的边框
    auto child = bg->getChildByName("border");
    bg->removeChild(child);  // 移除边框（DrawNode）
}
// 在当前场景上添加加载层
inline void addLoadingLayerToCurrentScene(cocos2d::Node* parent, float duration = 5.0f)
{
    if (!parent) return;

    auto visible_size = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 创建遮罩层
    auto mask_layer = LayerColor::create(Color4B(0, 0, 0, 255));
    mask_layer->setContentSize(visible_size);
    mask_layer->setPosition(origin);
    mask_layer->setName("loading_mask");
    parent->addChild(mask_layer, 9999);  // 最高层级
    // maskLayer->setGlobalZOrder(9999);

    // 创建加载文本
    auto label = Label::createWithSystemFont("正在进入村庄", "Arial", 40);
    label->setColor(Color3B::WHITE);
    label->setPosition(Vec2(visible_size.width / 2, visible_size.height / 2 + 100));
    mask_layer->addChild(label, 1);

    // 创建进度条前景
    // 创建进度条
    auto progress_bar = cocos2d::ui::LoadingBar::create("LoadingBarFile.png");
    progress_bar->setName("progress_bar");
    progress_bar->setPosition(Vec2(visible_size.width / 2, visible_size.height / 2 - 5));
    progress_bar->setScale(2.0f);
    mask_layer->addChild(progress_bar, 1);

    // 百分比标签
    auto percent_label = Label::createWithSystemFont("0%", "Arial", 24);
    percent_label->setColor(Color3B::WHITE);
    percent_label->setPosition(Vec2(visible_size.width / 2, visible_size.height / 2 - 50));
    percent_label->setName("percent_label");
    mask_layer->addChild(percent_label);

    // 启动加载进度
    float d = duration;
    bool is_reset = false;
    mask_layer->schedule(
        [d, progress_bar, percent_label, mask_layer](float dt) {
            static float elapsed_time = 0.0f;
            elapsed_time += dt;

            float progress = (elapsed_time * 100.0f / d);
            progress = std::min(100.0f, progress);
            if (elapsed_time > d) elapsed_time = 0.0f;
            // 更新进度条绘制
            if (progress_bar) {
                progress_bar->setPercent(progress);
            }

            // 更新百分比
            if (percent_label) {
                percent_label->setString(StringUtils::format("%.0f%%", progress));
            }

            // 加载完成
            if (progress >= 100.0f) {
                mask_layer->unschedule("loading_update");

                // 淡出动画
                auto fade_out = FadeOut::create(0.5f);
                auto remove = CallFunc::create([mask_layer]() { mask_layer->removeFromParent(); });

                mask_layer->runAction(Sequence::create(fade_out, remove, nullptr));
            }
        },
        0.12f, "loading_update");
}

/* 调用示例
void returnToVillage() {
    auto currentScene = Director::getInstance()->getRunningScene();
    addLoadingLayerToCurrentScene(currentScene, 5.0f);
}
*/

#endif  // __UICOMMON_H__
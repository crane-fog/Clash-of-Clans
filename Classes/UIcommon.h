#ifndef __UICOMMON_H__
#define __UICOMMON_H__

#include "cocos/ui/CocosGUI.h"
#include "ShopPopup.h"
#include "UIparts.h"
USING_NS_CC;
using namespace ui;
// 边框绘制，bg：当前框；borderWidth = 3.0f:边框宽度；Color4F colorBorder:边框颜色
inline void draw_border(cocos2d::LayerColor* bg, float borderWidth = 3.0f,
                        cocos2d::Color4F colorBorder = cocos2d::Color4F(1.0f, 0.8f, 0.0f, 1.0f), int gap = 0)
{
    // 添加边框
    auto border = cocos2d::DrawNode::create();
    border->setName("border");
    // 基于bg的实际尺寸
    cocos2d::Size bgSize = bg->getContentSize();

    // 绘制四条边（使用bg的尺寸）
    // 上边
    border->drawSegment(cocos2d::Vec2(0, bgSize.height), cocos2d::Vec2(bgSize.width, bgSize.height), borderWidth,
                        colorBorder);
    // 右边
    border->drawSegment(cocos2d::Vec2(bgSize.width, bgSize.height), cocos2d::Vec2(bgSize.width, 0), borderWidth,
                        colorBorder);
    // 下边
    border->drawSegment(cocos2d::Vec2(bgSize.width, 0), cocos2d::Vec2(0, 0), borderWidth, colorBorder);
    // 左边
    border->drawSegment(cocos2d::Vec2(0, 0), cocos2d::Vec2(0, bgSize.height), borderWidth, colorBorder);

    border->setPosition(cocos2d::Vec2::ZERO);  // 相对于bg的本地坐标系
    bg->addChild(border, 100);
}

// 去除边框
inline void remove_border(cocos2d::LayerColor* bg)
{
    // 通过子节点移除已添加的边框
    auto child = bg->getChildByName("border");
    bg->removeChild(child);  // 移除边框（DrawNode）
}
// 在当前场景上添加加载层
inline void addLoadingLayerToCurrentScene(cocos2d::Node* parent, float duration = 5.0f)
{
    if (!parent) return;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 创建遮罩层
    auto maskLayer = LayerColor::create(Color4B(0, 0, 0, 255));
    maskLayer->setContentSize(visibleSize);
    maskLayer->setPosition(origin);
    maskLayer->setName("loading_mask");
    parent->addChild(maskLayer, 9999);  // 最高层级
    // maskLayer->setGlobalZOrder(9999);

    // 创建加载文本
    auto label = Label::createWithSystemFont("正在进入村庄", "Arial", 40);
    label->setColor(Color3B::WHITE);
    label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 100));
    maskLayer->addChild(label, 1);

    // 创建进度条前景
    // 创建进度条
    auto progressBar = cocos2d::ui::LoadingBar::create("LoadingBarFile.png");
    progressBar->setName("progress_bar");
    progressBar->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 5));
    progressBar->setScale(2.0f);
    maskLayer->addChild(progressBar, 1);

    // 百分比标签
    auto percentLabel = Label::createWithSystemFont("0%", "Arial", 24);
    percentLabel->setColor(Color3B::WHITE);
    percentLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 50));
    percentLabel->setName("percent_label");
    maskLayer->addChild(percentLabel);

    // 启动加载进度
    float d = duration;
    bool isReset = false;
    maskLayer->schedule(
        [d, progressBar, percentLabel, maskLayer](float dt) {
            static float elapsedTime = 0.0f;
            elapsedTime += dt;

            float progress = (elapsedTime * 100.0f / d);
            progress = std::min(100.0f, progress);
            if (elapsedTime > d) elapsedTime = 0.0f;
            // 更新进度条绘制
            if (progressBar) {
                progressBar->setPercent(progress);
            }

            // 更新百分比
            if (percentLabel) {
                percentLabel->setString(StringUtils::format("%.0f%%", progress));
            }

            // 加载完成
            if (progress >= 100.0f) {
                maskLayer->unschedule("loading_update");

                // 淡出动画
                auto fadeOut = FadeOut::create(0.5f);
                auto remove = CallFunc::create([maskLayer]() { maskLayer->removeFromParent(); });

                maskLayer->runAction(Sequence::create(fadeOut, remove, nullptr));
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
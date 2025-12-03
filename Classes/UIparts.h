#pragma once
#pragma once
#ifndef __UI_PARTS_H__
#define __UI_PARTS_H__

#include "cocos2d.h"
#include "BaseMap.h"
#include "cocos/ui/CocosGUI.h"

// 进度条结构体
struct ProgressBarData {
    cocos2d::ui::LoadingBar* loadingBar;
    cocos2d::LayerColor* background;
    cocos2d::Label* percentLabel;
    cocos2d::Sprite* icon;
    std::string title;
};

// UI部分基类
class UI : public cocos2d::Scene {
private:
    std::vector<ProgressBarData> progressBars_;  // 存储多个进度条

public:
    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;

    // 创建带背景的进度条
    void createProgressBarWithBackground(const std::string& title, const cocos2d::Color3B& barColor, const std::string& iconPath, float percent, float x, float y, int UpperLimit);

    // 更新指定进度条
    void updateProgressBar(int index, float percent);
    void updateProgressBar(const std::string& title, float percent);

    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(UI);
};
class ShopPopup : public cocos2d::Layer
{
public:
    CREATE_FUNC(ShopPopup);
    virtual bool init();

    void show(cocos2d::Node* parent);
    void close();
    void onShopButtonClick(cocos2d::Ref* sender);
    void setupBackground();
private:
    void onClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

};
struct ShopItem {
    int id;
    std::string name;
    int price;
    bool isAvailable;
    std::string unavailableReason;
};
#endif // __UI_PARTS_H__
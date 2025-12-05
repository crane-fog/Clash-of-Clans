#pragma once
#pragma once
#ifndef __UI_PARTS_H__
#define __UI_PARTS_H__

#include "cocos2d.h"
#include "BaseMap.h"
#include "cocos/ui/CocosGUI.h"
#include"Arch.h"
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
struct ShopItem {
    int id;
    std::string name;
    int price;
    bool isAvailable;
    std::string unavailableReason;
    std::string imagePath;
    int rarity; // 添加：0-N, 1-R, 2-SR, 3-SSR
};
class ShopPopup : public cocos2d::Layer
{
public:
    CREATE_FUNC(ShopPopup);
    virtual bool init();
    //展示商店面板
    void show(cocos2d::Node* parent);
    void close();
    void onShopButtonClick(cocos2d::Ref* sender);
    void setupBackground();
    // 添加显示气泡提示的函数
    void showUnavailableBubble(const ShopItem& item, cocos2d::LayerColor* targetNode, cocos2d::ui::ScrollView* scrollView);

private:
    void onClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    void switchToTab(int tabIndex);  // 切换标签函数
    void showItemsInScrollView(const std::vector<ShopItem>& items, cocos2d::ui::ScrollView* scrollView, int tabIndex=1);  // 显示商品函数

    // 成员变量
    int currentTab_;  // 当前选中的标签：1-建筑，2-士兵，3-抽卡
    std::vector<ShopItem> buildingItems_;  // 建筑商品
    std::vector<ShopItem> soldierItems_;   // 士兵商品
    std::vector<ShopItem> gachaItems_;     // 抽卡商品
    cocos2d::ui::ScrollView* scrollView_;  // 滚动容器引用


    void performGacha();                          // 执行抽卡
    void showGachaAnimation();                    // 显示抽卡动画
    void showGachaResult(const ShopItem& item);   // 显示抽卡结果
    void createGachaItem();                       // 创建抽卡商品界面
    void initGachaPool();
    // 稀有度枚举
    enum Rarity {
        RARITY_N = 0,     // 普通
        RARITY_R = 1,     // 稀有
        RARITY_SR = 2,    // 超级稀有
        RARITY_SSR = 3    // 特级稀有
    };

    // 添加抽卡相关变量
    std::vector<ShopItem> gachaPool_;             // 抽卡池
    Node* gachaResultNode_ = nullptr;             // 抽卡结果节点
};

#endif // __UI_PARTS_H__
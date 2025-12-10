#pragma once
#pragma once
#ifndef __UI_PARTS_H__
#define __UI_PARTS_H__

#include "cocos2d.h"
#include "BaseMap.h"
#include "cocos/ui/CocosGUI.h"
#include"Arch.h"
#include<string.h>
// 进度条结构体
struct ProgressBarData {
    cocos2d::ui::LoadingBar* loadingBar;
    cocos2d::LayerColor* background;
    cocos2d::Label* percentLabel;
    cocos2d::Sprite* icon;
    std::string title;
};
//进度条上限数量
enum UpperLimit :unsigned long long {
    GoldLimit = 50000,
    ElixirLimit = 50000
};

// UI部分基类
class UIBars : public cocos2d::Node {
private:
    std::vector<ProgressBarData> progressBars_;  // 存储多个进度条

    cocos2d::EventListenerCustom* goldUpdateListener;  // 存储监听器
public:

    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;

    // 创建带背景的进度条
    //title:进度条左边文字标签， barcolor:进度条颜色， iconPath：图标文件路径，nowAmount：当前数量，x,y位置，UpperLimit:上限
    void createProgressBarWithBackground(const std::string& title, const cocos2d::Color3B& barColor, const std::string& iconPath, unsigned long long nowAmount, float x, float y, unsigned long long UpperLimit);
    // 更新指定进度条
    void updateProgressBar(const std::string& title, unsigned long long nowAmount);

    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(UIBars);



};

enum ShopType :int {
    buildingItems = 1,
    soldierItems=2,
    gachaItems=3
};

struct ShopItem {
    //标签
    int id;
    //建筑名称
    std::string name;
    //所需资源
    unsigned int price;
    //是否可购
    bool isAvailable;
    //不可购原因
    std::string unavailableReason;
    //图片路径
    std::string imagePath;
    //珍稀度
    int rarity; // 添加：0-N, 1-R, 2-SR, 3-SSR

    // 构造函数
    ShopItem(int i, const std::string& n,unsigned int p, bool available, const std::string& reason, const std::string& path, int r=0)
        : id(i), name(n), price(p), isAvailable(available), unavailableReason(reason), imagePath(path), rarity(r) {
    }

};
// 定义三个板块的商品数据
const std::map<int, std::vector<ShopItem>> kShopItemsInfo = {
        {buildingItems, {
        {1, "兵营", kArchInfo.at(ARMY_CAMP)[0].upgrade_cost_amount_, true, "", "arch/Army_Camp1.webp"},
        {2, "城墙", kArchInfo.at(WALL)[0].upgrade_cost_amount_, false, "等级不足", "arch/Wall1.webp"},
        {3, "金库", kArchInfo.at(GOLD_STORAGE)[0].upgrade_cost_amount_, true, "", "arch/Gold_Storage1.webp"},
        {4, "圣水罐", kArchInfo.at(ELIXIR_STORAGE)[0].upgrade_cost_amount_, false, "等级不足", "arch/Elixir_Storage1.webp"},
        {5, "金矿", kArchInfo.at(GOLD_MINE)[0].upgrade_cost_amount_, true, "", "arch/Gold_Mine1.webp"},
        {6, "圣水收集器", kArchInfo.at(ELIXIR_COLLECTOR)[0].upgrade_cost_amount_, false, "需要完成前置任务", "arch/Elixir_Collector1.webp"},
        {7, "箭塔", kArchInfo.at(ARCHER_TOWER)[0].upgrade_cost_amount_, false, "VIP only", "arch/Archer_Tower1.webp"},
        {8, "加农炮", kArchInfo.at(CANNON)[0].upgrade_cost_amount_, false, "VIP only", "arch/Cannon1.webp"},
        {9, "训练营", kArchInfo.at(BARRACKS)[0].upgrade_cost_amount_, false, "VIP only", "arch/Barracks1.webp"}}},

        {soldierItems, {
        {101, "野蛮人", 30, true, "", "Barbarian.png"},
        {102, "弓箭手", 45, true, "", "Barbarian.png"},
        {103, "巨人", 150, false, "需要2级兵营", "Barbarian.png"},
        {104, "哥布林", 25, true, "", "Barbarian.png"},
        {105, "炸弹人", 50, false, "需要完成训练", "Barbarian.png"}}},

        {gachaItems, {
        {201, "神秘宝箱", 1000, true, "有机会获得稀有物品！", "lucky.png"}}},
};


class ShopPopup : public cocos2d::Layer
{
public:
    CREATE_FUNC(ShopPopup);
    virtual bool init();
    //展示商店面板
    void show(cocos2d::Node* parent);
    //关闭商店面板
    void close();
    void onShopButtonClick(cocos2d::Ref* sender);
    //面板背景遮盖
    void setupBackground();
    // 添加显示气泡提示的函数
    void showUnavailableBubble(const ShopItem& item, cocos2d::LayerColor* targetNode, cocos2d::ui::ScrollView* scrollView);

private:
    //关闭动画
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
    void showGachaAnimation(int rarity );         // 显示抽卡动画
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

//购买函数相关
private:
    Arch* pendingArch_ = nullptr;
    cocos2d::EventListener* mapTouchListener_ = nullptr;
    bool isPlacingArch_ = false;

    
public:

    friend class Arch;
};

//倒计时
class CountdownTimer : public cocos2d::Node {
public:
    CREATE_FUNC(CountdownTimer);

    void start(unsigned int seconds,
        std::function<void(int remaining)> onTick = nullptr,
        std::function<void()> onComplete = nullptr);

private:
    unsigned int remainingTime_;
    unsigned int totalTime_;
    bool isRunning_;
    std::function<void(int)> onTick_;
    std::function<void()> onComplete_;

    void updateTimer(float dt);
};

enum Buidlingtype : bool{
    NEW_BUIDING = 0,
    UPGRADING = 1
};



#endif // __UI_PARTS_H__
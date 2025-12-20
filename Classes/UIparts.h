#pragma once
#pragma once
#ifndef __UI_PARTS_H__
#define __UI_PARTS_H__

#include "cocos2d.h"
#include "BaseMap.h"
#include "cocos/ui/CocosGUI.h"
#include"Arch.h"
#include<string.h>
#include "CocController.h"
#include"UIcommon.h"
// 进度条结构体
struct ProgressBarData {
    cocos2d::ui::LoadingBar* loadingBar;
    cocos2d::LayerColor* background;
    cocos2d::Label* percentLabel;
    cocos2d::Sprite* icon;
    std::string title;
};

// UI部分基类
class UIBars : public cocos2d::Node {
private:


    std::vector<ProgressBarData> progressBars_;  // 存储多个进度条
    cocos2d::EventListenerCustom* goldUpdateListener;  // 存储监听器
    cocos2d::EventListenerCustom* elixirUpdateListener;  // 存储监听器
    cocos2d::EventListenerCustom* maxGoldUpdateListener;  // 存储监听器
    cocos2d::EventListenerCustom* maxElixirUpdateListener;  // 存储监听器

public:



    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;

    // 创建带背景的进度条
    //title:进度条左边文字标签， barcolor:进度条颜色， iconPath：图标文件路径，nowAmount：当前数量，x,y位置，UpperLimit:上限
    void createProgressBarWithBackground(const std::string& title, const cocos2d::Color3B& barColor, const std::string& iconPath, unsigned long long nowAmount, float x, float y, unsigned long long UpperLimit);
    // 更新指定进度条
    void updateProgressBar(const std::string& title, unsigned long long nowAmount, unsigned long long maxAmount);

    // 更新金币进度条的回调函数
    void onGoldUpdated(cocos2d::EventCustom* event);
    // 更新圣水进度条的回调函数
    void onElixirUpdated(cocos2d::EventCustom* event);
    // 更新最大金币的回调函数
    void onMaxGoldUpdated(cocos2d::EventCustom* event);
    // 更新最大圣水的回调函数
    void onMaxElixirUpdated(cocos2d::EventCustom* event);
    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(UIBars);
};

enum ShopType :int {
    buildingItems = 1,
    soldierItems=2,
    gachaItems=3
};
enum Rarity :int {
    RARITY_N = 0,     // 普通
    RARITY_R = 1,     // 稀有
    RARITY_SR = 2,    // 超级稀有
    RARITY_SSR = 3    // 特级稀有
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
    //所需资源类型
    bool p_type;
    // 构造函数
    ShopItem(int i, const std::string& n,unsigned int p, bool available, const std::string& reason, const std::string& path, int r=0, bool type =GOLD)
        : id(i), name(n), price(p), isAvailable(available), unavailableReason(reason), imagePath(path), rarity(r) , p_type(type){
    }

};
// 定义三个板块的商品数据
const std::map<int, std::vector<ShopItem>> kShopItemsInfo = {
        {buildingItems, {
        {1, "兵营", kArchInfo.at(ARMY_CAMP)[0].upgrade_cost_amount_, true, "", "arch/Army_Camp1.webp",0,kArchInfo.at(ARMY_CAMP)[0].upgrade_cost_type_},
        {2, "城墙", kArchInfo.at(WALL)[0].upgrade_cost_amount_, true, "等级不足", "arch/Wall1.webp",0,kArchInfo.at(WALL)[0].upgrade_cost_type_},
        {3, "金库", kArchInfo.at(GOLD_STORAGE)[0].upgrade_cost_amount_, true, "", "arch/Gold_Storage1.webp",0,kArchInfo.at(GOLD_STORAGE)[0].upgrade_cost_type_},
        {4, "圣水罐", kArchInfo.at(ELIXIR_STORAGE)[0].upgrade_cost_amount_, false, "等级不足", "arch/Elixir_Storage1.webp",0,kArchInfo.at(ELIXIR_STORAGE)[0].upgrade_cost_type_},
        {5, "金矿", kArchInfo.at(GOLD_MINE)[0].upgrade_cost_amount_, true, "", "arch/Gold_Mine1.webp",0,kArchInfo.at(GOLD_MINE)[0].upgrade_cost_type_},
        {6, "圣水收集器", kArchInfo.at(ELIXIR_COLLECTOR)[0].upgrade_cost_amount_, false, "需要完成前置任务", "arch/Elixir_Collector1.webp",0,kArchInfo.at(ELIXIR_COLLECTOR)[0].upgrade_cost_type_},
        {7, "箭塔", kArchInfo.at(ARCHER_TOWER)[0].upgrade_cost_amount_, true, "VIP only", "arch/Archer_Tower1.webp",0,kArchInfo.at(ARCHER_TOWER)[0].upgrade_cost_type_},
        {8, "加农炮", kArchInfo.at(CANNON)[0].upgrade_cost_amount_, false, "VIP only", "arch/Cannon1.webp",0,kArchInfo.at(CANNON)[0].upgrade_cost_type_},
        {9, "训练营", kArchInfo.at(BARRACKS)[0].upgrade_cost_amount_, false, "VIP only", "arch/Barracks1.webp",0,kArchInfo.at(BARRACKS)[0].upgrade_cost_type_}}},

        {soldierItems, {
        {1, "野蛮人", 30,false, "", "Barbarian.png"},
        {2, "弓箭手", 45, false, "", "Barbarian.png"},
        {3, "巨人", 150, false, "需要2级兵营", "Barbarian.png"},
        {4, "哥布林", 25, false, "", "Barbarian.png"},
        {5, "炸弹人", 50, false, "需要完成训练", "Barbarian.png"}}},

        {gachaItems, {
        {201, "神秘宝箱", 1000, true, "有机会获得稀有物品！", "lucky.png"}}},
};

// 定义抽卡奖品
const std::map<int, std::vector<ShopItem>> kGachaItemsInfo = {
        {1, {
        // SSR物品 (5%)
{301, "传奇之剑", 0, true, "SSR稀有物品！", "arch/Town_Hall4.webp", 3},
{302, "神圣护甲", 0, true, "SSR稀有物品！", "arch/Town_Hall4.webp", 3},
{303, "龙之宝珠", 0, true, "SSR稀有物品！", "arch/Town_Hall4.webp", 3},

// SR物品 (15%)
{304, "魔法法杖", 0, true, "SR稀有物品！", "arch/Town_Hall3.webp", 2},
{305, "精灵之弓", 0, true, "SR稀有物品！", "arch/Town_Hall3.webp", 2},
{306, "勇士盾牌", 0, true, "SR稀有物品！", "arch/Town_Hall3.webp", 2},
{307, "智慧之书", 0, true, "SR稀有物品！", "arch/Town_Hall3.webp", 2},
{308, "凤凰羽毛", 0, true, "SR稀有物品！", "arch/Town_Hall3.webp", 2},

// R物品 (30%)
{309, "银质长剑", 0, true, "R稀有物品！", "arch/Town_Hall2.webp", RARITY_R},
{310, "钢铁盔甲", 0, true, "R稀有物品！",  "arch/Town_Hall2.webp",RARITY_R},
{311, "治疗药水", 0, true, "R稀有物品！", "arch/Town_Hall2.webp", RARITY_R},
{312, "魔法卷轴", 0, true, "R稀有物品！",  "arch/Town_Hall2.webp", RARITY_R},
{313, "力量戒指", 0, true, "R稀有物品！",  "arch/Town_Hall2.webp", RARITY_R},
{314, "速度之靴", 0, true, "R稀有物品！",  "arch/Town_Hall2.webp", RARITY_R},

// N物品 (50%)
{315, "普通长剑", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
{316, "皮革盔甲", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
{317, "小型药水", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
{318, "火把", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
{319, "面包", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
{320, "钥匙", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
{321, "绳子", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
{322, "箭袋", 0, true, "普通物品","arch/Town_Hall1.webp", 0}}},

        {2, {
        // SSR物品 (5%)
{301, "传奇之剑", 0, true, "SSR稀有物品！", "gacha/3.jpg", 3},

// SR物品 (15%)
{304, "魔法法杖", 0, true, "SR稀有物品！", "gacha/11.jpg", 2},
{305, "精灵之弓", 0, true, "SR稀有物品！", "gacha/22.jpg", 2},

// R物品 (30%)
{309, "银质长剑", 0, true, "R稀有物品！", "gacha/1.jpg", 1},
{310, "钢铁盔甲", 0, true, "R稀有物品！",  "gacha/2.jpg", 1},
{311, "治疗药水", 0, true, "R稀有物品！", "gacha/4.jpg", 1},
{312, "魔法卷轴", 0, true, "R稀有物品！",  "gacha/5.jpg", 1},

// N物品 (50%)
{315, "普通长剑", 0, true, "普通物品", "gacha/6.jpg", 0},
{316, "皮革盔甲", 0, true, "普通物品", "gacha/7.jpg", 0},
{317, "小型药水", 0, true, "普通物品", "gacha/8.jpg", 0},
{318, "火把", 0, true, "普通物品", "gacha/9.jpg", 0},
{319, "面包", 0, true, "普通物品", "gacha/10.jpg", 0}}},

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
    void showUnavailableBubble(const ShopItem& item, cocos2d::LayerColor* targetNode, cocos2d::ui::ScrollView* scrollView, std::string reason);

    // 是否正在十连抽
    bool isTenGachaRunning_ = false;

    // 当前是第几抽（0~9）
    int currentTenIndex_ = 0;

    // 十连抽结果缓存（可选，但推荐）
    std::vector<ShopItem> tenResults_;
    void ShopPopup::startTenGacha();
    void ShopPopup::runNextTenGacha();
    void ShopPopup::performSingleGacha(
        const std::function<void(ShopItem)>& onFinished
    );
private:
    //关闭动画
    void onClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    void switchToTab(int tabIndex);  // 切换标签函数
    void showItemsInScrollView(const std::vector<ShopItem>& items, cocos2d::ui::ScrollView* scrollView, int tabIndex=1);  // 显示商品函数

    // 成员变量
    int currentTab_;  // 当前选中的标签：1-建筑，2-士兵，3-抽卡
    std::vector<ShopItem> buildingItems_;  // 建筑商品
    std::vector<ShopItem> soldierItems_= kShopItemsInfo.at(2);   // 士兵商品
    std::vector<ShopItem> gachaItems_;     // 抽卡商品
    cocos2d::ui::ScrollView* scrollView_;  // 滚动容器引用


    //void performGacha();                          // 执行抽卡
    void showGachaAnimation(int rarity );         // 显示抽卡动画
    void showGachaResult(const ShopItem& item);   // 显示抽卡结果
    void createGachaItem();                       // 创建抽卡商品界面
    void initGachaPool();
    // 稀有度枚举


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

class UICommonHelper {
private:
    int canConfirm_ = -1;
public:
    cocos2d::LayerColor* selectedItemBg = nullptr;  // 当前选中的按钮背景
    int getcanC() {
        return canConfirm_;
    }
    void setcanC(int i) {
        this->canConfirm_ = i;
    }

    // 显示挑战场景选择面板
    static void showChallengeSelectionPanel(cocos2d::Node* parent,  int gold_, int elixir_    ) {
        // 创建一个覆盖全屏的面板
        auto panel = cocos2d::LayerColor::create(cocos2d::Color4B(205, 160, 100, 255));  // 黑色背景
        parent->addChild(panel, 99999);
        bool selectedOptions[4] = {false,false,false,false};
        // 面板标题
        auto titleLabel = cocos2d::Label::createWithSystemFont("选择挑战场景", "Arial", 56);
        titleLabel->setPosition(cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width / 2,
            cocos2d::Director::getInstance()->getVisibleSize().height - 50));
        panel->addChild(titleLabel,1);

        // 创建四个选项
        std::vector<std::string> sceneNames = { "场景1", "场景2", "场景3", "场景4" };
        std::vector<std::string> sceneImages = { "attack_scene/Scenery1.webp", "attack_scene/Scenery2.webp", "attack_scene/Scenery3.webp", "attack_scene/Scenery4.webp" };
        std::vector<std::string> difficultyLevels = { "简单", "中等", "困难", "极难" };


        // 确认按钮
        auto confirmButton = cocos2d::ui::Button::create("attack_scene/yes.png");
        confirmButton->setPosition(cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width - 200, 100));
        confirmButton->setTitleText("确定");
        confirmButton->setTitleColor(cocos2d::Color3B::BLACK);
        confirmButton->setScale(0.8f);
        confirmButton->setEnabled(false);  // 默认不可点击
        confirmButton->setName("confirm_attack");
        panel->addChild(confirmButton);

        confirmButton->addClickEventListener([parent, &selectedOptions, gold_, elixir_,panel](cocos2d::Ref* sender) {
            // 确认后更换场景
            if (selectedOptions[0] != -1) { // 确保已经选择了一个选项
                CocController::getInstance()->changeScene(1, gold_, elixir_);
                // 点击确认按钮后关闭面板
                panel->removeFromParent();
            }
            });

        // 退出按钮
        auto exitButton = cocos2d::ui::Button::create("attack_scene/exit.png");
        exitButton->setPosition(cocos2d::Vec2(200, 100));
        exitButton->setTitleText("退出");
        exitButton->setScale(0.8f);
        exitButton->addClickEventListener([panel](cocos2d::Ref* sender) {
            // 退出面板
            panel->removeFromParent();
            });
        panel->addChild(exitButton);

        float buttonWidth = 350;
        float buttonHeight = 400;
        float padding = 130;
        int canConfirm[1] = { -1 };
        for (size_t i = 0; i < sceneNames.size(); i++) {
            // 选项背景
            auto itemBg = cocos2d::LayerColor::create(cocos2d::Color4B(255, 255, 255, 255), buttonWidth, buttonHeight);
            itemBg->setPosition(cocos2d::Vec2((buttonWidth + padding) * i + 50, 350));
            itemBg->setTag(i);
            // 选项图片
            auto itemPic = cocos2d::Sprite::create(sceneImages[i]);
            float scale = std::min(buttonWidth / itemPic->getContentSize().width, buttonHeight / itemPic->getContentSize().height);
            itemPic->setPosition(cocos2d::Vec2(buttonWidth / 2, buttonHeight / 2 + 20));
            itemPic->setScale(scale);

            // 显示场景名称
            auto nameLabel = cocos2d::Label::createWithSystemFont(sceneNames[i], "Arial", 34);
            nameLabel->setPosition(cocos2d::Vec2(buttonWidth / 2, 25));  // 名字位置
            nameLabel->setColor(cocos2d::Color3B::BLACK);
            itemBg->addChild(nameLabel, 150);

            // 显示难度级别
            auto difficultyLabel = cocos2d::Label::createWithSystemFont(difficultyLevels[i], "Arial", 25);
            difficultyLabel->setColor(cocos2d::Color3B::BLACK);
            difficultyLabel->setPosition(cocos2d::Vec2(buttonWidth / 2, buttonHeight + 20));  // 难度位置
            itemBg->addChild(difficultyLabel, 150);

            // 将按钮添加到背景层
            itemBg->addChild(itemPic);
            panel->addChild(itemBg);

            // 添加触摸事件监听器
            auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
            touchListener->onTouchBegan = [parent, itemBg, i, &selectedOptions,panel, confirmButton,&canConfirm](cocos2d::Touch* touch, cocos2d::Event* event) {
                // 获取触摸点并判断是否点击了按钮
                cocos2d::Rect buttonRect = itemBg->getBoundingBox();
                if (buttonRect.containsPoint(touch->getLocation())) {
                    UICommonHelper::onOptionClick(itemBg, i, selectedOptions, confirmButton,panel); 
                    return true;  // 阻止事件继续传播
                }
                return false;
                };
            parent->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, itemBg);  // 为按钮添加触摸事件
        }

    }
    // 选项点击事件处理
    static void onOptionClick(cocos2d::LayerColor* itemBg, int index, bool selectedOptions[], cocos2d::ui::Button* confirmButton, cocos2d::LayerColor* panel) {
        itemBg->setColor(cocos2d::Color3B::BLUE);




        // 如果有选中项，启用确认按钮并变为橙色

            confirmButton->setEnabled(true);


    }
};

#endif // __UI_PARTS_H__
#ifndef __SHOP_POPUP_H__
#define __SHOP_POPUP_H__

#include <string.h>

#include "Arch.h"
#include "BaseMap.h"
#include "CocManager.h"
#include "TroopTargetManager.h"
#include "ui/CocosGUI.h"
// 商店物品类别，1为建筑，2为法术，3为抽卡
enum ShopType : int { buildingItems = 1, magicItems = 2, gachaItems = 3 };
// 稀有度
enum Rarity : int {
    RARITY_N = 0,   // 普通
    RARITY_R = 1,   // 稀有
    RARITY_SR = 2,  // 超级稀有
    RARITY_SSR = 3  // 特级稀有
};
// 商店物品结构体
struct ShopItem {
    // 标签
    int id_;
    // 建筑名称
    std::string name_;
    // 所需资源
    unsigned int price_;
    // 是否可购
    bool is_available_;
    // 不可购原因
    std::string unavailable_reason_;
    // 图片路径
    std::string image_path_;
    // 珍稀度
    int rarity_;  // 添加：0-N, 1-R, 2-SR, 3-SSR
    // 所需资源类型
    bool p_type_;
    // 构造函数
    ShopItem(int i, const std::string& n, unsigned int p, bool available, const std::string& reason,
             const std::string& path, int r = 0, bool type = GOLD)
        : id_(i),
          name_(n),
          price_(p),
          is_available_(available),
          unavailable_reason_(reason),
          image_path_(path),
          rarity_(r),
          p_type_(type)
    {}
};

// 定义三个板块的商品数据
const std::map<int, std::vector<ShopItem>> kShopItemsInfo = {
    {buildingItems,
     {{1, "兵营", kArchInfo.at(ARMY_CAMP)[0].upgrade_cost_amount_, true, "", kArchInfo.at(ARMY_CAMP)[0].image_, 0,
       kArchInfo.at(ARMY_CAMP)[0].upgrade_cost_type_},
      {2, "城墙", kArchInfo.at(WALL)[0].upgrade_cost_amount_, true, "", kArchInfo.at(WALL)[0].image_, 0,
       kArchInfo.at(WALL)[0].upgrade_cost_type_},
      {3, "金库", kArchInfo.at(GOLD_STORAGE)[0].upgrade_cost_amount_, true, "", kArchInfo.at(GOLD_STORAGE)[0].image_, 0,
       kArchInfo.at(GOLD_STORAGE)[0].upgrade_cost_type_},
      {4, "圣水罐", kArchInfo.at(ELIXIR_STORAGE)[0].upgrade_cost_amount_, false, "",
       kArchInfo.at(ELIXIR_STORAGE)[0].image_, 0, kArchInfo.at(ELIXIR_STORAGE)[0].upgrade_cost_type_},
      {5, "金矿", kArchInfo.at(GOLD_MINE)[0].upgrade_cost_amount_, true, "", kArchInfo.at(GOLD_MINE)[0].image_, 0,
       kArchInfo.at(GOLD_MINE)[0].upgrade_cost_type_},
      {6, "圣水收集器", kArchInfo.at(ELIXIR_COLLECTOR)[0].upgrade_cost_amount_, false, "",
       kArchInfo.at(ELIXIR_COLLECTOR)[0].image_, 0, kArchInfo.at(ELIXIR_COLLECTOR)[0].upgrade_cost_type_},
      {7, "箭塔", kArchInfo.at(ARCHER_TOWER)[0].upgrade_cost_amount_, true, "", kArchInfo.at(ARCHER_TOWER)[0].image_, 0,
       kArchInfo.at(ARCHER_TOWER)[0].upgrade_cost_type_},
      {8, "加农炮", kArchInfo.at(CANNON)[0].upgrade_cost_amount_, false, "", kArchInfo.at(CANNON)[0].image_, 0,
       kArchInfo.at(CANNON)[0].upgrade_cost_type_},
      {9, "训练营", kArchInfo.at(BARRACKS)[0].upgrade_cost_amount_, false, "", kArchInfo.at(BARRACKS)[0].image_, 0,
       kArchInfo.at(BARRACKS)[0].upgrade_cost_type_}}},

    {magicItems,
     {{1, "治疗法术", 30, false, "VIP only", "shop/Healing_Spell.webp"},
      {2, "加速法术", 45, false, "VIP only", "shop/Haste_Spell.webp"},
      {3, "狂暴法术", 150, false, "VIP only", "shop/Rage_Spell.webp"}}},

    {gachaItems, {{201, "神秘宝箱", 1000, true, "有机会获得稀有物品！", "lucky.png"}}},
};

// 定义抽卡奖品
const std::map<int, std::vector<ShopItem>> kGachaItemsInfo = {
    {1,
     {// SSR物品 (5%)
      {301, "超级英雄雕像", 0, true, "SSR稀有物品！", "gacha/301.webp", RARITY_SSR},
      {302, "宝剑雕像", 0, true, "SSR稀有物品！", "gacha/302.webp", RARITY_SSR},
      {303, "皇冠雕像", 0, true, "SSR稀有物品！", "gacha/303.png", RARITY_SSR},

      // SR物品 (15%)
      {304, "海盗旗", 0, true, "SR稀有物品！", "gacha/304.webp", RARITY_SR},
      {305, "万能雕像", 0, true, "SR稀有物品！", "gacha/305.webp", RARITY_SR},
      {306, "死灵墓碑", 0, true, "SR稀有物品！", "gacha/306.webp", RARITY_SR},
      {307, "挑战者雕像", 0, true, "SR稀有物品！", "gacha/307.webp", RARITY_SR},
      {308, "雪人", 0, true, "SR稀有物品！", "gacha/308.png", RARITY_SR},

      // R物品 (30%)
      {309, "欢乐雪橇", 0, true, "R稀有物品！", "gacha/310.webp", RARITY_R},
      {310, "南瓜幽灵", 0, true, "R稀有物品！", "gacha/311.webp", RARITY_R},
      {311, "惊悚温泉", 0, true, "R稀有物品！", "gacha/312.webp", RARITY_R},
      {312, "小鸭浴盆", 0, true, "R稀有物品！", "gacha/313.png", RARITY_R},
      {313, "圣诞铃铛", 0, true, "R稀有物品！", "gacha/314.webp", RARITY_R},
      {314, "哥布林财神", 0, true, "R稀有物品！", "gacha/309.webp", RARITY_R},

      // N物品 (50%)
      {315, "普通旗子", 0, true, "普通物品", "gacha/315.webp", RARITY_N},
      {316, "花丛", 0, true, "普通物品", "gacha/316.webp", RARITY_N},
      {317, "太阳花丛", 0, true, "普通物品", "gacha/317.webp", RARITY_N},
      {318, "下行旗子", 0, true, "普通物品", "gacha/318.webp", RARITY_N},
      {319, "上行旗子", 0, true, "普通物品", "gacha/319.webp", RARITY_N},
      {320, "猪圈", 0, true, "普通物品", "gacha/320.png", RARITY_N},
      {321, "树桩", 0, true, "普通物品", "gacha/321.webp", RARITY_N},
      {322, "长凳", 0, true, "普通物品", "gacha/322.webp", RARITY_N}}},

    {2,
     {// SSR物品 (5%)
      {301, "传奇之剑", 0, true, "SSR稀有物品！", "gacha/301.webp", 3},

      // SR物品 (15%)
      {304, "魔法法杖", 0, true, "SR稀有物品！", "gacha/301.webp", 2},
      {305, "精灵之弓", 0, true, "SR稀有物品！", "gacha/301.webp", 2},

      // R物品 (30%)
      {309, "银质长剑", 0, true, "R稀有物品！", "gacha/301.webp", 1},
      {310, "钢铁盔甲", 0, true, "R稀有物品！", "gacha/301.webp", 1},
      {311, "治疗药水", 0, true, "R稀有物品！", "gacha/301.webp", 1},
      {312, "魔法卷轴", 0, true, "R稀有物品！", "gacha/301.webp", 1},

      // N物品 (50%)
      {315, "普通长剑", 0, true, "普通物品", "gacha/301.webp", 0},
      {316, "皮革盔甲", 0, true, "普通物品", "gacha/301.webp", 0},
      {317, "小型药水", 0, true, "普通物品", "gacha/301.webp", 0},
      {318, "火把", 0, true, "普通物品", "gacha/301.webp", 0},
      {319, "面包", 0, true, "普通物品", "gacha/301.webp", 0}}},

};

// 商店弹窗类，用于显示游戏内商店界面
class ShopPopup : public cocos2d::Layer {
private:
    // 关闭弹窗（带动画效果）
    // @param sender 触发关闭操作的按钮
    // @param type 触摸事件类型
    void onClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

    // 切换商店标签页
    // @param tabIndex 要切换到的标签页索引（1=建筑，2=法术，3=抽卡）
    void switchToTab(int tabIndex);

    // 在滚动视图中显示商品列表
    // @param items 要显示的商品列表
    // @param scrollView 用于显示商品的滚动视图容器
    // @param tabIndex 当前激活的标签页索引（默认为建筑页）
    void showItemsInScrollView(const std::vector<ShopItem>& items, cocos2d::ui::ScrollView* scrollView,
                               int tabIndex = 1);

    // 显示抽卡动画
    // @param rarity 抽卡物品的稀有度
    void showGachaAnimation(int rarity);

    // 显示抽卡结果
    // @param item 从抽卡中获得的商品
    void showGachaResult(const ShopItem& item);

    // 创建抽卡商品界面
    void createGachaItem();

    // 初始化抽卡池
    void initGachaPool();

    // 当前选中的标签页：1=建筑，2=法术，3=抽卡
    int current_tab_;

    // 建筑商品列表
    std::vector<ShopItem> building_items_;

    // 法术商品列表，从配置中初始化
    std::vector<ShopItem> magic_items_ = kShopItemsInfo.at(2);

    // 抽卡商品列表
    std::vector<ShopItem> gacha_items_;

    // 滚动视图容器的引用
    cocos2d::ui::ScrollView* scroll_view_;

    // 抽卡池（包含所有可抽取的物品）
    std::vector<ShopItem> gacha_pool_;

    // 抽卡结果显示节点
    Node* gacha_result_node_ = nullptr;

    // 等待放置的建筑对象
    Arch* pending_arch_ = nullptr;

    // 地图触摸事件监听器
    cocos2d::EventListener* map_touch_listener_ = nullptr;

    // 是否正在放置建筑
    bool is_placing_arch_ = false;

    // 是否正在执行十连抽
    bool is_ten_gacha_running_ = false;

    // 当前十连抽中的第几次抽卡（0~9）
    int current_ten_index_ = 0;

    // 十连抽结果缓存
    std::vector<ShopItem> ten_results_;

public:
    // 创建函数宏
    CREATE_FUNC(ShopPopup);

    // 初始化函数
    virtual bool init();

    // 显示商店面板
    // @param parent 父节点，商店将显示在该节点上
    void show(cocos2d::Node* parent);

    // 关闭商店面板
    void close();

    // 商店按钮点击事件处理
    void onShopButtonClick(cocos2d::Ref* sender);

    // 设置背景遮罩
    void setupBackground();

    // 显示不可用提示气泡
    // @param item 不可用的商品
    // @param targetNode 目标节点，提示将显示在该节点附近
    // @param scrollView 滚动视图容器
    // @param reason 不可用的原因描述
    void showUnavailableBubble(const ShopItem& item, cocos2d::LayerColor* targetNode,
                               cocos2d::ui::ScrollView* scrollView, std::string reason);

    // 开始十连抽
    void startTenGacha();

    // 执行十连抽中的下一次抽卡
    void runNextTenGacha();

    // 执行单次抽卡
    // @param onFinished 抽卡完成后的回调函数
    void performSingleGacha(const std::function<void(ShopItem)>& onFinished);
};
#endif  // __SHOP_POPUP_H__#pragma once
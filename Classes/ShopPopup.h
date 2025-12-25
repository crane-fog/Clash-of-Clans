#ifndef __SHOP_POPUP_H__
#define __SHOP_POPUP_H__

#include <string.h>

#include "Arch.h"
#include "BaseMap.h"
#include "CocController.h"
#include "TroopTargetManager.h"
#include "ui/CocosGUI.h"

enum ShopType : int { buildingItems = 1, magicItems = 2, gachaItems = 3 };

enum Rarity : int {
    RARITY_N = 0,   // 普通
    RARITY_R = 1,   // 稀有
    RARITY_SR = 2,  // 超级稀有
    RARITY_SSR = 3  // 特级稀有
};

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
     {{1, "兵营", kArchInfo.at(ARMY_CAMP)[0].upgrade_cost_amount_, true, "", "arch/Army_Camp1.webp", 0,
       kArchInfo.at(ARMY_CAMP)[0].upgrade_cost_type_},
      {2, "城墙", kArchInfo.at(WALL)[0].upgrade_cost_amount_, true, "等级不足", "arch/Wall1.webp", 0,
       kArchInfo.at(WALL)[0].upgrade_cost_type_},
      {3, "金库", kArchInfo.at(GOLD_STORAGE)[0].upgrade_cost_amount_, true, "", "arch/Gold_Storage1.webp", 0,
       kArchInfo.at(GOLD_STORAGE)[0].upgrade_cost_type_},
      {4, "圣水罐", kArchInfo.at(ELIXIR_STORAGE)[0].upgrade_cost_amount_, false, "等级不足",
       "arch/Elixir_Storage1.webp", 0, kArchInfo.at(ELIXIR_STORAGE)[0].upgrade_cost_type_},
      {5, "金矿", kArchInfo.at(GOLD_MINE)[0].upgrade_cost_amount_, true, "", "arch/Gold_Mine1.webp", 0,
       kArchInfo.at(GOLD_MINE)[0].upgrade_cost_type_},
      {6, "圣水收集器", kArchInfo.at(ELIXIR_COLLECTOR)[0].upgrade_cost_amount_, false, "需要完成前置任务",
       "arch/Elixir_Collector1.webp", 0, kArchInfo.at(ELIXIR_COLLECTOR)[0].upgrade_cost_type_},
      {7, "箭塔", kArchInfo.at(ARCHER_TOWER)[0].upgrade_cost_amount_, true, "VIP only", "arch/Archer_Tower1.webp", 0,
       kArchInfo.at(ARCHER_TOWER)[0].upgrade_cost_type_},
      {8, "加农炮", kArchInfo.at(CANNON)[0].upgrade_cost_amount_, false, "VIP only", "arch/Cannon1.webp", 0,
       kArchInfo.at(CANNON)[0].upgrade_cost_type_},
      {9, "训练营", kArchInfo.at(BARRACKS)[0].upgrade_cost_amount_, false, "VIP only", "arch/Barracks1.webp", 0,
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
      {310, "钢铁盔甲", 0, true, "R稀有物品！", "arch/Town_Hall2.webp", RARITY_R},
      {311, "治疗药水", 0, true, "R稀有物品！", "arch/Town_Hall2.webp", RARITY_R},
      {312, "魔法卷轴", 0, true, "R稀有物品！", "arch/Town_Hall2.webp", RARITY_R},
      {313, "力量戒指", 0, true, "R稀有物品！", "arch/Town_Hall2.webp", RARITY_R},
      {314, "速度之靴", 0, true, "R稀有物品！", "arch/Town_Hall2.webp", RARITY_R},

      // N物品 (50%)
      {315, "普通长剑", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
      {316, "皮革盔甲", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
      {317, "小型药水", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
      {318, "火把", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
      {319, "面包", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
      {320, "钥匙", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
      {321, "绳子", 0, true, "普通物品", "arch/Town_Hall1.webp", 0},
      {322, "箭袋", 0, true, "普通物品", "arch/Town_Hall1.webp", 0}}},

    {2,
     {// SSR物品 (5%)
      {301, "传奇之剑", 0, true, "SSR稀有物品！", "gacha/3.jpg", 3},

      // SR物品 (15%)
      {304, "魔法法杖", 0, true, "SR稀有物品！", "gacha/11.jpg", 2},
      {305, "精灵之弓", 0, true, "SR稀有物品！", "gacha/22.jpg", 2},

      // R物品 (30%)
      {309, "银质长剑", 0, true, "R稀有物品！", "gacha/1.jpg", 1},
      {310, "钢铁盔甲", 0, true, "R稀有物品！", "gacha/2.jpg", 1},
      {311, "治疗药水", 0, true, "R稀有物品！", "gacha/4.jpg", 1},
      {312, "魔法卷轴", 0, true, "R稀有物品！", "gacha/5.jpg", 1},

      // N物品 (50%)
      {315, "普通长剑", 0, true, "普通物品", "gacha/6.jpg", 0},
      {316, "皮革盔甲", 0, true, "普通物品", "gacha/7.jpg", 0},
      {317, "小型药水", 0, true, "普通物品", "gacha/8.jpg", 0},
      {318, "火把", 0, true, "普通物品", "gacha/9.jpg", 0},
      {319, "面包", 0, true, "普通物品", "gacha/10.jpg", 0}}},

};

class ShopPopup : public cocos2d::Layer {
private:
    // 关闭动画
    void onClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

    // 切换标签
    void switchToTab(int tabIndex);

    // 显示商品
    void showItemsInScrollView(const std::vector<ShopItem>& items, cocos2d::ui::ScrollView* scrollView,
                               int tabIndex = 1);

    int current_tab_;                                           // 当前选中的标签：1-建筑，2-法术，3-抽卡
    std::vector<ShopItem> building_items_;                      // 建筑商品
    std::vector<ShopItem> magic_items_ = kShopItemsInfo.at(2);  // 法术商品
    std::vector<ShopItem> gacha_items_;                         // 抽卡商品
    cocos2d::ui::ScrollView* scroll_view_;                      // 滚动容器引用

    // void performGacha();                          // 执行抽卡
    void showGachaAnimation(int rarity);         // 显示抽卡动画
    void showGachaResult(const ShopItem& item);  // 显示抽卡结果
    void createGachaItem();                      // 创建抽卡商品界面
    void initGachaPool();

    // 抽卡相关变量
    std::vector<ShopItem> gacha_pool_;   // 抽卡池
    Node* gacha_result_node_ = nullptr;  // 抽卡结果节点

    Arch* pending_arch_ = nullptr;

    cocos2d::EventListener* map_touch_listener_ = nullptr;

    bool is_placing_arch_ = false;

public:
    friend class Arch;

    CREATE_FUNC(ShopPopup);
    virtual bool init();

    // 展示商店面板
    void show(cocos2d::Node* parent);

    // 关闭商店面板
    void close();

    void onShopButtonClick(cocos2d::Ref* sender);

    // 面板背景遮盖
    void setupBackground();

    // 添加显示气泡提示的函数
    void showUnavailableBubble(const ShopItem& item, cocos2d::LayerColor* targetNode,
                               cocos2d::ui::ScrollView* scrollView, std::string reason);

    // 是否正在十连抽
    bool is_ten_gacha_running_ = false;

    // 当前是第几抽（0~9）
    int current_ten_index_ = 0;

    // 十连抽结果缓存（可选，但推荐）
    std::vector<ShopItem> ten_results_;

    void ShopPopup::startTenGacha();

    void ShopPopup::runNextTenGacha();

    void ShopPopup::performSingleGacha(const std::function<void(ShopItem)>& onFinished);
};
#endif  // __SHOP_POPUP_H__#pragma once
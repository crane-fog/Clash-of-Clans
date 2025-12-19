#ifndef __MAIN_VILLAGE_SCENE_H__
#define __MAIN_VILLAGE_SCENE_H__

#include "cocos2d.h"
#include "VillageScene.h"
#include "Arch.h"
//尝试把资源单独出来一个类单例，之前的获取有点搞不来
//使用例子：       
// unsigned long long currentGold = GameManager::getInstance()->getGold();
//GameManager::getInstance()->setGold(currentGold - item.price);
class GameManager {
    typedef unsigned long long ULL;
private:
    ULL my_gold_;
    ULL my_elixir_;
    ULL max_gold_;
    ULL max_elixir_;

public:
    static GameManager* getInstance() {
        static GameManager instance;
        return &instance;
    }

    void setGold(ULL gold)
    {
        this->my_gold_ = gold;
        // 发布金币更新事件
        cocos2d::EventCustom event("update_gold_event");
        event.setUserData(&my_gold_);
        cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }

    void setMaxGold(ULL max_gold)
    {
        max_gold_ = max_gold;
        cocos2d::EventCustom event("update_max_gold_event");
        event.setUserData(&max_gold_);
        cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }

    ULL getGold() const
    {
        return my_gold_;
    }

    ULL getMaxGold() const
    {
        return max_gold_;
    }

    void setElixir(ULL Elixir)
    {
        this->my_elixir_ = Elixir;
        // 发布圣水更新事件
        cocos2d::EventCustom event("update_elixir_event");
        event.setUserData(&my_elixir_);
        cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }

    void setMaxElixir(ULL max_elixir)
    {
        max_elixir_ = max_elixir;
        cocos2d::EventCustom event("update_max_elixir_event");
        event.setUserData(&max_elixir_);
        cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }

    ULL getElixir() const
    {
        return my_elixir_;
    }

    ULL getMaxElixir() const
    {
        return max_elixir_;
    }
};



// 主村庄场景类
class MainVillage : public Village {
private:
    // 村庄中的建筑状态数据
    ArchData arch_status_[MAP_SIZE][MAP_SIZE];

    // 上次退出场景的时间
    time_t last_exit_time_ = 0;

public:
    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;
    // 当对象被渲染时被自动调用
    virtual void onEnter() override;
    // 当对象退出渲染时被自动调用
    virtual void onExit() override;
    // 当对象被销毁时被自动调用
    virtual void cleanup() override;
    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(MainVillage);
    void onShopButtonClick(Ref* sender);
    bool addBuildingByNO(unsigned char no, int price);
public:
    void MainVillage::createCancelButton(Arch* pendingArch_);
    void MainVillage::createConfirmButton(Arch* pendingArch_, int price, bool type_);
    void MainVillage::cancelBuildingPlacement(Arch* pendingArch_);
    void MainVillage::confirmBuildingPlacement(Arch* pendingArch_);
    void MainVillage::removeCancelAndConfirmButtons(Arch* pendingArch_);
    void MainVillage::playBuildingDropEffect(Arch* arch);
    //延迟调用商店面板，sec为延迟秒数
    void MainVillage::showShopPopupWithDelay(float sec);

    void onAttackButtonClick(Ref* sender);

    cocos2d::LayerColor* selectedItemBg = nullptr;  // 当前选中的按钮背景
    void showChallengeSelectionPanel(cocos2d::Node* parent, int gold_, int elixir_);
    void onOptionClick(cocos2d::LayerColor* itemBg, cocos2d::ui::Button* confirmButton);


    // 获取大本营等级
    unsigned char getTownHallLevel();
    // 获取指定建筑数量
    int getBuildingCount(unsigned char archNo);
};
#endif // __MAIN_VILLAGE_SCENE_H__

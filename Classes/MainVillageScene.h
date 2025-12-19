#ifndef __MAIN_VILLAGE_SCENE_H__
#define __MAIN_VILLAGE_SCENE_H__

#include "cocos2d.h"
#include "VillageScene.h"
#include "Arch.h"

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

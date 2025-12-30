#ifndef __MAIN_VILLAGE_SCENE_H__
#define __MAIN_VILLAGE_SCENE_H__

#include "Arch.h"
#include "AudioEngine.h"
#include "Barbarian.h"
#include "cocos2d.h"
#include "CocUtility.h"
#include "VillageScene.h"

// 主村庄场景类
class MainVillage : public Village {
private:
    // 村庄中的建筑状态数据
    ArchData arch_status_[kMapSize][kMapSize];

    // 上次退出场景的时间
    time_t last_exit_time_ = 0;

    /* 按钮回调函数 */
    void onShopButtonClick(Ref* sender);
    void onAttackButtonClick(Ref* sender);
    void onTroopButtonClick(Ref* sender);
    void onLabButtonClick(Ref* sender);
    void onMessageButtonClick(Ref* sender);
    void onTroopUpradeClick(Ref* sender, Widget::TouchEventType type, unsigned char& it, cocos2d::LayerColor* panel);

    /* 建筑建造相关 */
    void createCancelButton(Arch* pendingArch_);

    void createConfirmButton(Arch* pendingArch_, int price, bool type_);

    void cancelBuildingPlacement(Arch* pendingArch_);

    void confirmBuildingPlacement(Arch* pendingArch_);

    void removeCancelAndConfirmButtons(Arch* pendingArch_);

    void playBuildingDropEffect(Arch* arch);

    // 背景音乐
    int mainhome_bgm_ = -1;

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

        // 获取大本营等级
    unsigned char getTownHallLevel();
    // 获取指定建筑数量，用于在建造建筑时限制建筑数量
    int getBuildingCount(unsigned char archNo);
    //通过编号判断新建建筑类型
    bool addBuildingByNO(unsigned char no, int price);

};
#endif  // __MAIN_VILLAGE_SCENE_H__

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
public:
    static GameManager* getInstance() {
        static GameManager instance;
        return &instance;
    }

    void setGold(unsigned long long gold) {
        this->MyGold = gold;
        // 发布金币更新事件
        cocos2d::EventCustom event("update_gold_event");
        event.setUserData(&this->MyGold);
        cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }

    unsigned long long getGold() const {
        return MyGold;
    }
    void setExixir(unsigned long long gold) {
        this->MyExilir = gold;
        // 发布金币更新事件
        cocos2d::EventCustom event("update_exilir_event");
        event.setUserData(&this->MyExilir);
        cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }

    unsigned long long getExilir() const {
        return MyExilir;
    }

private:
    unsigned long long MyGold = 1000;
    unsigned long long MyExilir = 1000;
};



// 主村庄场景类
class MainVillage : public Village {
private:
    // 村庄中的建筑状态数据
    ArchData arch_status_[MAP_SIZE][MAP_SIZE];
    // 金币储量
    unsigned long long gold_;
    // 圣水储量
    unsigned long long elixir_ ;


public:
    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;
    // 当对象被渲染时被自动调用
    virtual void onEnter() override;
    // 当对象被销毁时被自动调用
    virtual void cleanup() override;
    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(MainVillage);
    void onShopButtonClick(Ref* sender);
    bool addBuildingByNO(unsigned char no, int price);
public:
    void MainVillage::createCancelButton(Arch* pendingArch_);
    void MainVillage::createConfirmButton(Arch* pendingArch_);
    void MainVillage::cancelBuildingPlacement(Arch* pendingArch_);
    void MainVillage::confirmBuildingPlacement(Arch* pendingArch_);
    void MainVillage::removeCancelAndConfirmButtons(Arch* pendingArch_);
    void MainVillage::playBuildingDropEffect(Arch* arch);
    //延迟调用商店面板，sec为延迟秒数
    void MainVillage::showShopPopupWithDelay(float sec);
//获取金币数量
unsigned long long getGold() {
    return gold_;
}
//获取圣水数量
unsigned long long getElixir() {
    return elixir_;
}


//更新金币数量
void renewGold(unsigned long long num) {
    gold_ = num;
}
//更新圣水数量
void renewElixir(unsigned long long num) {
    elixir_ = num;
}

};


#endif // __MAIN_VILLAGE_SCENE_H__

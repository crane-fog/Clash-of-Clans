#ifndef __ENEMY_VILLAGE_SCENE_H__
#define __ENEMY_VILLAGE_SCENE_H__

#include "VillageScene.h"
#include "Troop.h"
#include <vector>
// 敌人村庄场景类
class EnemyVillage : public Village {
private:
    std::vector<Troop*> troop_list_;

    cocos2d::LayerColor* selectedItemBg = nullptr;  // 当前选中的按钮背景
public:
    virtual bool myInit(int level, unsigned long long gold, unsigned long long elixir);
    static EnemyVillage* create(int level, unsigned long long gold, unsigned long long elixir);
    void onExitButtonClick(cocos2d::Ref* sender);

    bool EnemyVillage::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event, std::set<std::pair<int, int>> occupied_cells);
    void EnemyVillage::spawnBarbarian(cocos2d::Vec2 position);
    void EnemyVillage::spawnArcher(cocos2d::Vec2 position);
    void EnemyVillage::spawnGiant(cocos2d::Vec2 position);
    void EnemyVillage::spawnDragon(cocos2d::Vec2 position);
    void EnemyVillage::spawnBalloon(cocos2d::Vec2 position);
    void EnemyVillage::spawnBomb(cocos2d::Vec2 position);
    void EnemyVillage::showInvalidSpawnMessage(std::string text= "不可以把士兵放在这里");

    void EnemyVillage::createTroopSelectionPanel(cocos2d::LayerColor * bg);
    void EnemyVillage::spawnSelectedTroop(int index);
    void onButtonClick(cocos2d::LayerColor* itemBg, int index);


    void onMouseClick(cocos2d::Event* event);
    // 添加鼠标事件监听器
    void addMouseListener(cocos2d::Node* parentNode);
};

#endif // __ENEMY_VILLAGE_SCENE_H__
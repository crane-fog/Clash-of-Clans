#ifndef __ENEMY_VILLAGE_SCENE_H__
#define __ENEMY_VILLAGE_SCENE_H__

#include "VillageScene.h"
#include "Troop.h"
#include <vector>
#include "AudioEngine.h"

// 敌人村庄场景类
class EnemyVillage : public Village {
private:
    std::vector<Troop*> troop_list_;

    cocos2d::LayerColor* selectedItemBg = nullptr;  // 当前选中的按钮背景
   
public:

    int attackingBGM = cocos2d::AudioEngine::play2d("music/attacking.mp3");
    virtual bool myInit(int level);
    static EnemyVillage* create(int level);
    void onExitButtonClick(cocos2d::Ref* sender);

    bool EnemyVillage::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event, std::set<std::pair<int, int>> occupied_cells);
    bool EnemyVillage::spawnBarbarian(cocos2d::Vec2 position);
    bool EnemyVillage::spawnArcher(cocos2d::Vec2 position);
    bool EnemyVillage::spawnGiant(cocos2d::Vec2 position);
    bool EnemyVillage::spawnDragon(cocos2d::Vec2 position);
    bool EnemyVillage::spawnBalloon(cocos2d::Vec2 position);
    bool EnemyVillage::spawnBomb(cocos2d::Vec2 position);
    void EnemyVillage::showInvalidSpawnMessage(std::string text= "不可以把士兵放在这里");
    void EnemyVillage::updateTroopCountLabel(int index);
    void EnemyVillage::disableTroopButton(int index);
    void EnemyVillage::createTroopSelectionPanel(cocos2d::LayerColor * bg);

    void onButtonClick(cocos2d::LayerColor* itemBg, int index);

  

    std::vector<int> troopMaxCounts_;          // 兵种最大数量
    std::vector<int> troopPlacedCounts_;       // 已放置数量
    std::vector<cocos2d::LayerColor*> troopButtons_;   // 按钮引用
    std::vector<cocos2d::Label*> troopCountLabels_;    // 数量标签引用

    bool onReplayButtonClick(cocos2d::Ref* sender, int gold_, int elixir_);
    void EnemyVillage::ReplayBegin();
    void EnemyVillage::startReplaySequence();
};


#endif // __ENEMY_VILLAGE_SCENE_H__
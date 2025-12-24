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
    std::set<std::pair<int, int>> occupied_cells_; // 不可下兵区域

    cocos2d::LayerColor* selectedItemBg = nullptr;  // 当前选中的按钮背景
   
public:

    int attackingBGM = cocos2d::AudioEngine::play2d("music/attacking.mp3");
    virtual bool myInit(int level);
    static EnemyVillage* create(int level);
    void onExitButtonClick(cocos2d::Ref* sender);

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    bool spawnTroop(unsigned char type, unsigned char lvl, cocos2d::Vec2 position);
    void showInvalidSpawnMessage(std::string text= "不可以把士兵放在这里");
    void updateTroopCountLabel(int index);
    void disableTroopButton(int index);
    void createTroopSelectionPanel(cocos2d::LayerColor * bg);

    void onButtonClick(cocos2d::LayerColor* itemBg, int index);


    // 利用 lambda 实现的轻量级工厂
    std::map<unsigned char, std::function<Troop* (BaseMap*, unsigned char, cocos2d::Vec2)>> troop_factories_;

    std::vector<int> troopPlacedCounts_;       // 已放置数量
    std::vector<cocos2d::LayerColor*> troopButtons_;   // 按钮引用
    std::vector<cocos2d::Label*> troopCountLabels_;    // 数量标签引用
    unsigned char selected_troop_type_; // -1表示未选择任何兵种

    //bool onReplayButtonClick(cocos2d::Ref* sender, int gold_, int elixir_);
    //void EnemyVillage::ReplayBegin();
    //void EnemyVillage::startReplaySequence();
};


#endif // __ENEMY_VILLAGE_SCENE_H__
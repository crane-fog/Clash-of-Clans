#ifndef __ENEMY_VILLAGE_SCENE_H__
#define __ENEMY_VILLAGE_SCENE_H__

#include <vector>
#include <chrono>

#include "AudioEngine.h"
#include "Troop.h"
#include "VillageScene.h"
#include "CocUtility.h"

// 敌人村庄场景类
class EnemyVillage : public Village {
private:
    // 已创建的士兵列表
    std::vector<Troop*> troop_list_;

    // 不可下兵区域
    std::set<std::pair<int, int>> occupied_cells_;

    // 当前选中的按钮背景
    cocos2d::LayerColor* selected_item_bg_ = nullptr;

public:
    int attacking_bgm_ = cocos2d::AudioEngine::play2d("music/attacking.mp3");

    virtual bool myInit(int level);
    static EnemyVillage* create(int level);

    void onExitButtonClick(cocos2d::Ref* sender);

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

    bool spawnTroop(unsigned char type, unsigned char lvl, cocos2d::Vec2 position);

    void showInvalidSpawnMessage(std::string text = "不可以把士兵放在这里");

    void updateTroopCountLabel(unsigned int index);

    void disableTroopButton(unsigned int index);

    void createTroopSelectionPanel(cocos2d::LayerColor* bg);

    void onButtonClick(cocos2d::LayerColor* itemBg, int index);

    // 利用 lambda 实现的轻量级工厂
    std::map<unsigned char, std::function<Troop*(BaseMap*, unsigned char, cocos2d::Vec2)>> troop_factories_;

    std::vector<unsigned int> troop_placed_counts_;    // 已放置数量
    std::vector<cocos2d::LayerColor*> troop_buttons_;  // 按钮
    std::vector<cocos2d::Label*> troop_count_labels_;  // 数量标签
    unsigned char selected_troop_type_;                // 255表示未选择任何兵种

    ReplayData current_replay_data_;
    std::chrono::steady_clock::time_point first_deployment_time_;
    bool has_deployed_troop_ = false;

public:
    // 回放
    // bool onReplayButtonClick(cocos2d::Ref* sender, int gold_, int elixir_);
    // void EnemyVillage::ReplayBegin();
    // void EnemyVillage::startReplaySequence();
};

#endif  // __ENEMY_VILLAGE_SCENE_H__
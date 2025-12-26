#ifndef __ENEMY_VILLAGE_SCENE_H__
#define __ENEMY_VILLAGE_SCENE_H__

#include <chrono>
#include <vector>

#include "AudioEngine.h"
#include "CocUtility.h"
#include "Troop.h"
#include "VillageScene.h"

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
    virtual bool myInit(int level);
    static EnemyVillage* create(int level);


    /* 士兵部署相关 */
    std::vector<unsigned int> troop_placed_counts_;    // 已放置数量
    std::vector<cocos2d::LayerColor*> troop_buttons_;  // 按钮
    std::vector<cocos2d::Label*> troop_count_labels_;  // 数量标签
    unsigned char selected_troop_type_;                // 255表示未选择任何兵种

    bool spawnTroop(unsigned char type, unsigned char lvl, cocos2d::Vec2 position);

    void showInvalidSpawnMessage(std::string text = "不可以把士兵放在这里", float time = 1.0f);

    void updateTroopCountLabel(unsigned int index);

    void disableTroopButton(unsigned int index);

    void createTroopSelectionPanel(cocos2d::LayerColor* bg);

    void onTroopButtonClick(cocos2d::LayerColor* itemBg, int index);

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);


    /* 回放相关 */
    static EnemyVillage* createReplay(const ReplayData& data);

    void startReplaySequence();

    // 回放数据记录
    ReplayData current_replay_data_;
    std::chrono::steady_clock::time_point first_deployment_time_;
    bool has_deployed_troop_ = false;


    // 背景音乐
    int attacking_bgm_ = cocos2d::AudioEngine::play2d("music/attacking.mp3");

    // 退出按钮回调
    void onExitButtonClick(cocos2d::Ref* sender);

    // 游戏结束标志
    bool is_game_over_ = false;

    // 每帧更新
    virtual void update(float dt) override;
};

#endif  // __ENEMY_VILLAGE_SCENE_H__
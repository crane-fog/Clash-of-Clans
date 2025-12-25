#ifndef __ATTACKSTARS_H__
#define __ATTACKSTARS_H__

#include <string.h>

#include "Arch.h"
#include "BaseMap.h"
#include "CocManager.h"
#include "cocos/ui/CocosGUI.h"
#include "TroopTargetManager.h"

class AttackStars : public cocos2d::Node {
private:
    int last_dead_arch_ = 0;  // 记录上次的数量
    bool is_complete_[3] = {false};
    int arch_sum_ = TroopTargetManager::getInstance()->getlivingsum();
    float progress_;
    cocos2d::ui::LoadingBar* progress_bar_;
    std::vector<cocos2d::Sprite*> stars_;  // 存储星星

    // 创建星星
    cocos2d::Sprite* createStar();

    // 存储监听器
    cocos2d::EventListenerCustom* dead_arch_update_listener_;

public:
    // 存储监听器
    cocos2d::EventListenerCustom* town_hall_death_listener_;

    void onTownHallDeath(cocos2d::EventCustom* event);

    bool is_town_star_ = 0;

    // 防止胜利画面重复显示
    bool is_showing_victory_ = false;

    AttackStars() : progress_(0) {}
    ~AttackStars();

    CREATE_FUNC(AttackStars);

    virtual bool init() override;

    void setProgress(float progress);

    void setStarColor(cocos2d::Sprite* star, bool isAchieved);

    // 检查其他条件
    bool someOtherConditionMet() const { return progress_ > 75.0f; }

    // 点亮星星动画
    void showPopup(cocos2d::Sprite* targetStar, int progress);

    // 更新摧毁进度条的回调函数
    void checkForUpdates(float dt = 1.0f);

    // 重置UI到原始位置
    void resetUIPosition();

    // 显示胜利画面
    void showVictoryScreen();
};

#endif  // __ATTACKSTARS_H__
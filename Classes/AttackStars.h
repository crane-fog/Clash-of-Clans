#ifndef __ATTACKSTARS_H__
#define __ATTACKSTARS_H__

#include <string.h>

#include "Arch.h"
#include "BaseMap.h"
#include "CocController.h"
#include "cocos/ui/CocosGUI.h"
#include "cocos2d.h"
#include "TroopTargetManager.h"
#include "UIcommon.h"
class AttackStars : public cocos2d::Node {
public:
    cocos2d::EventListenerCustom* town_hall_death_listener_;  // 存储监听器
    void AttackStars::onTownHallDeath(cocos2d::EventCustom* event);
    bool is_town_star_ = 0;
    bool is_showing_victory_ = false;  // 防止胜利画面重复显示
    AttackStars() : progress_(0) {}

    static AttackStars* create()
    {
        AttackStars* ret = new AttackStars();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init() override;
    void setProgress(float progress)
    {
        progress_ = progress;
        progress_bar_->setPercent(progress_);
    }

    void setStarColor(cocos2d::Sprite* star, bool isAchieved)
    {
        if (isAchieved) {
            star->setColor(cocos2d::Color3B(255, 255, 255));  // 恢复原来的颜色
        }
        else {
            star->setColor(cocos2d::Color3B(169, 169, 169));  // 灰色
        }
    }

    // 检查其他条件
    bool someOtherConditionMet() const { return progress_ > 75.0f; }

    // 点亮星星动画
    void showPopup(cocos2d::Sprite* targetStar, int progress);

    // 更新摧毁进度条的回调函数
    void checkForUpdates(float dt = 1.0f);  // 定时检查函数
    // 重置UI到原始位置
    void AttackStars::resetUIPosition();

    // 显示胜利画面
    void AttackStars::showVictoryScreen();

private:
    int last_dead_arch_ = 0;  // 记录上次的数量
    bool is_complete_[3] = {false};
    ~AttackStars();
    int arch_sum_ = TroopTargetManager::getInstance()->getlivingsum();
    float progress_;
    cocos2d::ui::LoadingBar* progress_bar_;
    std::vector<cocos2d::Sprite*> stars_;  // 存储星星的vector

    // 创建星星
    cocos2d::Sprite* createStar()
    {
        return cocos2d::Sprite::create("attack_scene/star.png");  // 星星图片
    }

    cocos2d::EventListenerCustom* dead_arch_update_listener_;  // 存储监听器
};

#endif  // __ATTACKSTARS_H__
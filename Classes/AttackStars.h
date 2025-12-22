#pragma once
#ifndef __ATTACKSTARS_H__
#define __ATTACKSTARS_H__

#include "cocos2d.h"
#include "BaseMap.h"
#include "cocos/ui/CocosGUI.h"
#include"Arch.h"
#include<string.h>
#include "CocController.h"
#include"UIcommon.h"
#include"TroopTargetManager.h"
class AttackStars : public cocos2d::Node {
public:
    AttackStars()
        : progress_(0) {
    }

    static AttackStars* create() {
        AttackStars* ret = new AttackStars();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init() override;
    void setProgress(float progress) {
        progress_ = progress;
        progressBar_->setPercent(progress_);
    }

    void setStarColor(cocos2d::Sprite* star, bool isAchieved) {
        if (isAchieved) {
            star->setColor(cocos2d::Color3B(255, 255, 255)); // 恢复原来的颜色
        }
        else {
            star->setColor(cocos2d::Color3B(169, 169, 169)); // 灰色
        }
    }

    // 检查其他条件
    bool someOtherConditionMet() {
        return progress_ > 75.0f;
    }

    //点亮星星动画
    void showPopup(cocos2d::Sprite* targetStar, int progress);

    // 更新摧毁进度条的回调函数
    void checkForUpdates(float dt);  // 定时检查函数
private:
    int lastDeadArch = 0;  // 记录上次的数量
    bool isComplete[3] = { false };
    ~AttackStars();
    int ArchSum = TroopTargetManager::getInstance()->getlivingsum();
    float progress_;
    cocos2d::ui::LoadingBar* progressBar_;
    std::vector<cocos2d::Sprite*> stars_; // 存储星星的vector

    // 创建星星
    cocos2d::Sprite* createStar() {
        return cocos2d::Sprite::create("attack_scene/star.png"); // 星星图片
    }

    cocos2d::EventListenerCustom* deadArchUpdateListener;  // 存储监听器
};

#endif // __ATTACKSTARS_H__
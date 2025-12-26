#ifndef __ATTACKSTARS_H__
#define __ATTACKSTARS_H__

#include <string.h>

#include "Arch.h"
#include "BaseMap.h"
#include "CocManager.h"
#include "cocos/ui/CocosGUI.h"
#include "TroopTargetManager.h"
// 攻击星星类，用于显示战斗中的摧毁进度和获得的星星
class AttackStars : public cocos2d::Node {
private:
    // 上次记录的死亡建筑数量
    int last_dead_arch_ = 0;

    // 星星完成状态数组，记录三颗星星是否已获得
    bool is_complete_[3] = {false};

    // 敌方建筑总数
    int arch_sum_ = TroopTargetManager::getInstance()->getlivingsum();

    // 当前摧毁进度（百分比）
    float progress_;

    // 进度条控件
    cocos2d::ui::LoadingBar* progress_bar_;

    // 存储星星精灵的数组
    std::vector<cocos2d::Sprite*> stars_;

    // 创建星星精灵
    cocos2d::Sprite* createStar();

    // 建筑死亡更新事件监听器
    cocos2d::EventListenerCustom* dead_arch_update_listener_;

public:
    // 大本营死亡事件监听器
    cocos2d::EventListenerCustom* town_hall_death_listener_;

    // 大本营死亡事件处理函数
    void onTownHallDeath(cocos2d::EventCustom* event);

    // 是否已获得大本营星星
    bool is_town_star_ = 0;

    // 防止胜利画面重复显示的标志
    bool is_showing_victory_ = false;

    // 构造函数，初始化进度为0
    AttackStars() : progress_(0) {}

    // 析构函数
    ~AttackStars();

    // 创建函数宏
    CREATE_FUNC(AttackStars);

    // 初始化函数
    virtual bool init() override;

    // 设置进度值
    void setProgress(float progress);

    // 设置星星颜色
    void setStarColor(cocos2d::Sprite* star, bool isAchieved);

    // 显示星星弹出动画
    void showPopup(cocos2d::Sprite* targetStar, int progress);

    // 更新摧毁进度条的回调函数
    void checkForUpdates(float dt = 0);

    // 重置UI到原始位置
    void resetUIPosition();

    // 显示胜利画面
    void showVictoryScreen();
};

#endif  // __ATTACKSTARS_H__
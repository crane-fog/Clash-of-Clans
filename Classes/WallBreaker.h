#ifndef __WALLBREAKER_H__
#define __WALLBREAKER_H__
#include <string>
#include <vector>

#include "Troop.h"

const std::vector<std::string> kPicsWallbreaker = {
    "troop/Wall_Breaker1-2.webp", "troop/Wall_Breaker1-2.webp", "troop/Wall_Breaker1-2.webp",
    "troop/Wall_Breaker3-4.webp", "troop/Wall_Breaker3-4.webp", "troop/Wall_Breaker5.webp",
};

class WallBreaker : public Troop {
private:
    const float kAreaSplashRadius = 0.8f;   // 普攻伤害半径
    const float kDeathDamageRadius = 1.5f;  // 死亡伤害半径
    const std::array<float, MAX_TROOP_LEVEL + 1> kDamagesUponDeath = {0, 6, 9, 13, 16, 23};

public:
    // 升到level级所需资源花费
    static const std::array<int, MAX_TROOP_LEVEL + 1> kResearchCosts;

    // 升到level级所需时间 单位：小时
    static const std::array<float, MAX_TROOP_LEVEL + 1> kResearchTimes;

    // 升到level级所需实验室等级
    static const std::array<uchar, MAX_TROOP_LEVEL + 1> kLaboratoryLevelRequireds;
    // 构造函数
    WallBreaker(BaseMap* base_map, int level = 1, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);

    // 静态创建函数
    static WallBreaker* create(BaseMap* base_map, int level = 1, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);

    // 初始化
    virtual bool initWithFile(const std::string& filename) override;

    // 执行攻击
    virtual void performAttack() override;

    // 炸弹人目标搜索逻辑
    virtual void findNewTarget() override;

    // 获取士兵类型（地面兵种）
    virtual ArchTargetType getTargetType() const override { return GROUND; }

    // 获取当前位置-像素坐标，子类需要重写来保证视觉上中心在需要的坐标
    virtual cocos2d::Vec2 getPixelPosition() const override;

    // 获取士兵类型索引（用于区分不同子类类型）
    virtual TroopType getTroopTypeIndex() const override { return WALL_BREAKER; }

    // 死亡动画&溅射伤害
    virtual void onDeath() override;

    // 在以士兵为原点，像素坐标为pos的位置播放黑烟特效
    void playBlackSmokeAt(const cocos2d::Vec2& pos);
};

#endif  // __WALLBREAKER_H__

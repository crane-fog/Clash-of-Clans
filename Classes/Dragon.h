#ifndef __DRAGON_H__
#define __DRAGON_H__
#include <string>
#include <vector>

#include "Troop.h"

const std::vector<std::string> kPicsDragon = {
    "troop/Dragon1.webp", "troop/Dragon1.webp", "troop/Dragon2.webp",
    "troop/Dragon3.webp", "troop/Dragon4.webp", "troop/Dragon5.webp",
};

class Dragon : public Troop {
private:
    const float kAreaSplashRadius = 0.3f;  // 普攻伤害半径

public:
    // 升到level级所需资源花费
    static const std::array<int, MAX_TROOP_LEVEL + 1> kResearchCosts;

    // 升到level级所需时间 单位：小时
    static const std::array<float, MAX_TROOP_LEVEL + 1> kResearchTimes;

    // 升到level级所需实验室等级
    static const std::array<uchar, MAX_TROOP_LEVEL + 1> kLaboratoryLevelRequireds;

    // 构造函数
    Dragon(BaseMap* base_map, int level = 1, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);

    // 静态创建函数
    static Dragon* create(BaseMap* base_map, int level = 1, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);

    // 初始化
    virtual bool initWithFile(const std::string& filename) override;

    // 执行攻击
    virtual void performAttack() override;

    // 获取士兵类型（空中兵种）
    virtual ArchTargetType getTargetType() const override { return AIR; }

    // 获取当前位置-像素坐标，子类需要重写来保证视觉上中心在需要的坐标
    virtual cocos2d::Vec2 getPixelPosition() const override;

    // 获取士兵类型索引（用于区分不同子类类型）
    virtual TroopType getTroopTypeIndex() const override { return DRAGON; }

    void onDeath() override;

    // 喷火动画
    void playFlameEffect();
};

#endif  // __DRAGON_H__

#ifndef __BARBARIAN_H__
#define __BARBARIAN_H__
#include <string>
#include <vector>

#include "Troop.h"
const std::vector<std::string> kPicsBarbarian = {
    "troop/Barbarian1-2.webp", "troop/Barbarian1-2.webp", "troop/Barbarian1-2.webp",
    "troop/Barbarian3-4.webp", "troop/Barbarian3-4.webp", "troop/Barbarian5.webp",
};
class Barbarian : public Troop {
private:
public:
    // 升到level级所需资源花费
    static const std::array<int, MAX_TROOP_LEVEL + 1> kResearchCosts;

    // 升到level级所需时间 单位：小时
    static const std::array<float, MAX_TROOP_LEVEL + 1> kResearchTimes;

    // 升到level级所需实验室等级
    static const std::array<uchar, MAX_TROOP_LEVEL + 1> kLaboratoryLevelRequireds;

    // 构造函数
    Barbarian(BaseMap* base_map, int level = 1, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);

    // 静态创建函数
    static Barbarian* create(BaseMap* base_map, int level = 1, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);

    // 初始化
    virtual bool initWithFile(const std::string& filename) override;

    // 执行攻击
    virtual void performAttack() override;

    // 获取士兵类型（地面兵种）
    virtual ArchTargetType getTargetType() const override { return GROUND; }

    // 获取当前位置-像素坐标，子类需要重写来保证视觉上中心在需要的坐标
    virtual cocos2d::Vec2 getPixelPosition() const override
    {
        return CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(position_.x + 0.1, position_.y - 0.3));
    }

    // 获取士兵类型索引（用于区分不同子类类型）
    virtual TroopType getTroopTypeIndex() const override { return BARBARIAN; }
};

#endif  // __BARBARIAN_H__

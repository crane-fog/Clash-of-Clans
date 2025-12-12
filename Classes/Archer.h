#ifndef __ARCHER_H__
#define __ARCHER_H__
#include<vector>
#include<string>
#include "Troop.h"
const std::vector<std::string> pics_archer = {
        "troop/Archer1-2.webp",
        "troop/Archer1-2.webp",
        "troop/Archer1-2.webp",
        "troop/Archer3-4.webp",
        "troop/Archer3-4.webp",
        "troop/Archer5.webp",
};
class Archer : public Troop {
public:
    // 构造函数
    Archer(BaseMap* base_map, int level, cocos2d::Vec2 position);

    // 静态创建函数
    static Archer* create(BaseMap* base_map, int level = 1, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);

    // 初始化
    virtual bool initWithFile(const std::string& filename) override;

    // 执行攻击
    virtual void performAttack() override;

    // 获取士兵类型（地面兵种）
    virtual ArchTargetType getTargetType() const override { return GROUND; };

    //获取当前位置-像素坐标，子类需要重写来保证视觉上中心在需要的坐标
    virtual cocos2d::Vec2 getPixelPosition() const override { return CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(position_.x, position_.y)); }

};

#endif // __ARCHER_H__

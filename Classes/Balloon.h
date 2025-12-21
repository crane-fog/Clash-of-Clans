#ifndef __BALLOON_H__
#define __BALLOON_H__
#include<vector>
#include<string>
#include "Troop.h"
const std::vector<std::string> pics_balloon = {
        "troop/Balloon1-2.webp",
        "troop/Balloon1-2.webp",
        "troop/Balloon1-2.webp",
        "troop/Balloon3-4.webp",
        "troop/Balloon3-4.webp",
        "troop/Balloon5.webp",
};
class Balloon : public Troop {
private:

public:
    // 构造函数
    Balloon(BaseMap* base_map, int level = 1, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);

    // 静态创建函数
    static Balloon* create(BaseMap* base_map, int level = 1, cocos2d::Vec2 position = cocos2d::Vec2::ZERO);

    // 初始化
    virtual bool initWithFile(const std::string& filename) override;

    // 执行攻击
    virtual void performAttack() override;

    // 获取士兵类型（空中兵种）
    virtual ArchTargetType getTargetType() const override { return AIR; }

    //获取当前位置-像素坐标，子类需要重写来保证视觉上中心在需要的坐标
    virtual cocos2d::Vec2 getPixelPosition() const override {
        cocos2d::Vec2 pixel_ground = CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(position_.x, position_.y));
        return cocos2d::Vec2(pixel_ground.x, pixel_ground.y + 20.0f);
    }
    // 获取士兵类型索引（用于区分不同子类类型）
    virtual TroopType getTroopTypeIndex() const override { return BALLOON; }

	virtual void onDeath() override;
};

#endif // __BALLOON_H__

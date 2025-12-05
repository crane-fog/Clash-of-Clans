#ifndef __I_ARCH_TARGET_H__
#define __I_ARCH_TARGET_H__

#include "cocos2d.h"

class IArchTarget {
public:
    virtual ~IArchTarget() = default;

    // 建筑攻击士兵时调用的方法
    virtual void takeDamage(float damage) = 0;

    // 获取士兵位置-网格逻辑坐标
    virtual cocos2d::Vec2 getPosition() const = 0;

    // 获取士兵类型（用于建筑选择目标，如是否为空中兵种，具体返回值见todo-士兵）
    virtual unsigned char getTargetType() const = 0;

    // 检查士兵是否还活着
    virtual bool isAlive() const = 0;
};

#endif // __I_ARCH_TARGET_H__
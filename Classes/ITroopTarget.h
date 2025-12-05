#ifndef __ITROOPTARGET_H__
#define __ITROOPTARGET_H__
#include "cocos2d.h"
class ITroopTarget {
    public:
        virtual ~ITroopTarget() = default;
        
        // 士兵攻击建筑时调用这个方法
        virtual void takeDamage(float damage, int attackType) = 0;
        
        // 获取建筑位置-网格逻辑坐标
        virtual cocos2d::Vec2 getCellPosition() const = 0;
        
        // 获取建筑类型（用于士兵选择目标）
        virtual unsigned char getTargetType() const = 0;
        
        // 检查是否还活着
        virtual bool isAlive() const = 0;
    };

#endif // __ITROOPTARGET_H__
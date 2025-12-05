#ifndef __ARCHTARGETMANAGER_H__
#define __ARCHTARGETMANAGER_H__
#include "cocos2d.h"
#include "IArchTarget.h"
#include <vector>
class ArchTargetManager {
    protected:
        std::vector<IArchTarget*> targets_;
    public:
        static ArchTargetManager* getInstance();
    
        // 士兵把自己注册为可攻击目标
        void registerArchTarget(IArchTarget* target);
        
        // 士兵把自己从目标列表移除
        void unregisterArchTarget(IArchTarget* target);
    
        // 建筑查找攻击目标 目标类型（0=地面，1=空中，2=地面+空中）
        IArchTarget* getNearestArchTarget(
            const cocos2d::Vec2& center, float radius, unsigned char target_type);
    };

#endif // __ARCHTARGETMANAGER_H__
#ifndef __TROOPTARGETMANAGER_H__
#define __TROOPTARGETMANAGER_H__
#include "cocos2d.h"
#include "ITroopTarget.h"
#include "Troop.h"
#include <vector>

class TroopTargetManager {
    protected:
        std::vector<ITroopTarget*> targets_;
    public:
        static TroopTargetManager* getInstance();
        
        // 建筑把自己注册为可攻击目标
        void registerTroopTarget(ITroopTarget* target);
        
        // 建筑把自己从目标列表移除
        void unregisterTroopTarget(ITroopTarget* target);
        
        // 士兵查找攻击目标
        ITroopTarget* getNearestTroopTarget(const cocos2d::Vec2& position, bool is_wall_included=true,
            Troop::PreferredTarget preferred_target = Troop::NONE, float& min_distance);
    };

#endif // __TROOPTARGETMANAGER_H__
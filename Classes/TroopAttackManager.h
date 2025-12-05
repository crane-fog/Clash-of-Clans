#ifndef __TROOPATTACKMANAGER_H__
#define __TROOPATTACKMANAGER_H__

#include "cocos2d.h"
#include "Troop.h"
#include <vector>

class TroopAttackManager {
private:
    struct AttackEntry {
        Troop* troop;
        float timeSinceLastAttack;
    };

    std::vector<AttackEntry> attackQueue_;

    TroopAttackManager() = default;
    TroopAttackManager(const TroopAttackManager&) = delete;
    TroopAttackManager& operator=(const TroopAttackManager&) = delete;

public:
    static TroopAttackManager* getInstance();

    // 注册士兵到攻击队列
    void registerTroop(Troop* troop);

    // 从攻击队列移除士兵
    void unregisterTroop(Troop* troop);

    // 每帧更新所有士兵的攻击状态
    void update(float dt);

    // 强制某个士兵立即攻击（用于特殊情况）
    void forceAttack(Troop* troop);

    // 获取当前注册的士兵数量
    size_t getTroopCount() const;
};

#endif // __TROOPATTACKMANAGER_H__

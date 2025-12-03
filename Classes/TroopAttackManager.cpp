#include "TroopAttackManager.h"
#include <algorithm>

TroopAttackManager* TroopAttackManager::getInstance() {
    static TroopAttackManager instance;
    return &instance;
}

void TroopAttackManager::registerTroop(Troop* troop) {
    if (!troop) return;

    // 检查是否已经注册
    auto it = std::find_if(attackQueue_.begin(), attackQueue_.end(),
        [troop](const AttackEntry& entry) { return entry.troop == troop; });

    if (it == attackQueue_.end()) {
        attackQueue_.push_back({troop, 0.0f});
    }
}

void TroopAttackManager::unregisterTroop(Troop* troop) {
    if (!troop) return;

    auto it = std::remove_if(attackQueue_.begin(), attackQueue_.end(),
        [troop](const AttackEntry& entry) { return entry.troop == troop; });

    attackQueue_.erase(it, attackQueue_.end());
}

void TroopAttackManager::update(float dt) {
    // 更新所有士兵的攻击计时器
    for (auto& entry : attackQueue_) {
        entry.timeSinceLastAttack += dt;
    }

    // 找出所有可以攻击的士兵
    std::vector<Troop*> readyToAttack;
    for (const auto& entry : attackQueue_) {
        if (entry.timeSinceLastAttack >= entry.troop->attack_speed_) {
            readyToAttack.push_back(entry.troop);
        }
    }

    // 执行攻击并重置计时器
    for (Troop* troop : readyToAttack) {
        if (troop->canAttack()) {
            troop->performAttack();
        }

        // 重置该士兵的攻击计时器
        auto it = std::find_if(attackQueue_.begin(), attackQueue_.end(),
            [troop](const AttackEntry& entry) { return entry.troop == troop; });

        if (it != attackQueue_.end()) {
            it->timeSinceLastAttack = 0.0f;
        }
    }
}

void TroopAttackManager::forceAttack(Troop* troop) {
    if (!troop) return;

    auto it = std::find_if(attackQueue_.begin(), attackQueue_.end(),
        [troop](const AttackEntry& entry) { return entry.troop == troop; });

    if (it != attackQueue_.end()) {
        if (troop->canAttack()) {
            troop->performAttack();
        }
        it->timeSinceLastAttack = 0.0f;
    }
}

size_t TroopAttackManager::getTroopCount() const {
    return attackQueue_.size();
}

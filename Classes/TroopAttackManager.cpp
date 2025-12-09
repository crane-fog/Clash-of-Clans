#include "TroopAttackManager.h"
#include <algorithm>

TroopAttackManager* TroopAttackManager::getInstance() {
    static TroopAttackManager instance;
    return &instance;
}

void TroopAttackManager::registerTroop(Troop* troop) {
    if (!troop) return;

    // 检查是否已经注册
    auto it = std::find_if(attack_queue_.begin(), attack_queue_.end(),
        [troop](const AttackEntry& entry) { return entry.troop == troop; });

    if (it == attack_queue_.end()) {
        attack_queue_.push_back({troop, 0.0f});
    }
}

void TroopAttackManager::unregisterTroop(Troop* troop) {
    if (!troop) return;

    auto it = std::remove_if(attack_queue_.begin(), attack_queue_.end(),
        [troop](const AttackEntry& entry) { return entry.troop == troop; });

    attack_queue_.erase(it, attack_queue_.end());
}

void TroopAttackManager::update(float dt) {
    // 更新所有士兵的攻击计时器
    for (auto& entry : attack_queue_) {
        entry.time_since_last_attack_ += dt;
    }

    // 找出所有可以攻击的士兵
    std::vector<Troop*> readyToAttack;
    for (const auto& entry : attack_queue_) {
        if (entry.time_since_last_attack_ >= entry.troop->attack_speed_) {
            readyToAttack.push_back(entry.troop);
        }
    }

    // 执行攻击并重置计时器
    for (Troop* troop : readyToAttack) {
        if (troop->canAttack()) {
            troop->performAttack();
        }

        // 重置该士兵的攻击计时器
        auto it = std::find_if(attack_queue_.begin(), attack_queue_.end(),
            [troop](const AttackEntry& entry) { return entry.troop == troop; });

        if (it != attack_queue_.end()) {
            it->time_since_last_attack_ = 0.0f;
        }
    }
}

void TroopAttackManager::forceAttack(Troop* troop) {
    if (!troop) return;

    auto it = std::find_if(attack_queue_.begin(), attack_queue_.end(),
        [troop](const AttackEntry& entry) { return entry.troop == troop; });

    if (it != attack_queue_.end()) {
        if (troop->canAttack()) {
            troop->performAttack();
        }
        it->time_since_last_attack_ = 0.0f;
    }
}

size_t TroopAttackManager::getTroopCount() const {
    return attack_queue_.size();
}

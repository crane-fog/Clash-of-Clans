#include "TroopTargetManager.h"
#include <algorithm>
#include <limits>

TroopTargetManager* TroopTargetManager::getInstance() {
    static TroopTargetManager instance;
    return &instance;
}

void TroopTargetManager::registerTroopTarget(ITroopTarget* target) {
    if (target && std::find(targets_.begin(), targets_.end(), target) == targets_.end()) {
        targets_.push_back(target);
    }
}

void TroopTargetManager::unregisterTroopTarget(ITroopTarget* target) {
    auto it = std::find(targets_.begin(), targets_.end(), target);
    if (it != targets_.end()) {
        targets_.erase(it);
    }
}

ITroopTarget* TroopTargetManager::getNearestTroopTarget(const cocos2d::Vec2& position, float damage_range,
    Troop::PreferredTarget preferred_target) {
    ITroopTarget* nearestTarget = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (ITroopTarget* target : targets_) {
        if (!target->isAlive()) continue;

        // 如果指定了偏好类型，检查是否匹配
        if (preferred_target != 255 && target->getTargetType() != preferred_target) continue;

        float distance = position.distance(target->getCellPosition());
        if (distance < minDistance) {
            minDistance = distance;
            nearestTarget = target;
        }
    }

    return nearestTarget;
}

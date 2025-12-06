#include "TroopTargetManager.h"
#include <algorithm>
#include <limits>
#include "CalculateHelper.h"

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

ITroopTarget* TroopTargetManager::getNearestTroopTarget(const cocos2d::Vec2& position, float& min_distance, bool is_wall_included,
    Troop::PreferredTarget preferred_target) {

    ITroopTarget* nearest_target = nullptr;
    min_distance = std::numeric_limits<float>::max();

    // 第一遍：查找符合偏好类型的目标
    if (preferred_target != Troop::NONE) {
        for (ITroopTarget* target : targets_) {
			if (target->getTargetType() == Troop::WALLT && !is_wall_included) continue;
            if (!target->isAlive()) continue;
            // 检查是否匹配偏好类型
            if (target->getTargetType() != preferred_target) continue;

            float size;
            cocos2d::Vec2 target_pos = target->getCellPosition(size);
            float distance = CalculateHelper::calculateDistanceToSquare(position, target_pos, size);

            if (distance < min_distance) {
                min_distance = distance;
                nearest_target = target;
            }
        }
    }

    // 如果找到了符合偏好类型的目标，返回它
    if (nearest_target != nullptr) {
        return nearest_target;
    }

    // 第二遍：查找所有目标中的最近者（包括不符合偏好类型的）
    min_distance = std::numeric_limits<float>::max();
    for (ITroopTarget* target : targets_) {
        if (target->getTargetType() == Troop::WALLT && !is_wall_included) continue;
        if (!target->isAlive()) continue;

        float size;
        cocos2d::Vec2 target_pos = target->getCellPosition(size);
        float distance = CalculateHelper::calculateDistanceToSquare(position, target_pos, size);

        if (distance < min_distance) {
            min_distance = distance;
            nearest_target = target;
        }
    }

    return nearest_target;
}

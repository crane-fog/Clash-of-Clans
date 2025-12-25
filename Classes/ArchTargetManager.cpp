#include "ArchTargetManager.h"

#include <algorithm>
#include <limits>

ArchTargetManager* ArchTargetManager::getInstance()
{
    static ArchTargetManager instance;
    return &instance;
}

void ArchTargetManager::clear() { targets_.clear(); }

void ArchTargetManager::registerArchTarget(IArchTarget* target)
{
    if (!target) return;
    if (std::find(targets_.begin(), targets_.end(), target) == targets_.end()) {
        targets_.push_back(target);
    }
}

void ArchTargetManager::unregisterArchTarget(IArchTarget* target)
{
    if (!target) return;
    auto it = std::find(targets_.begin(), targets_.end(), target);
    if (it != targets_.end()) {
        targets_.erase(it);
    }
}

IArchTarget* ArchTargetManager::getNearestArchTarget(const cocos2d::Vec2& center, float radius,
                                                     unsigned char target_type)
{
    IArchTarget* nearest_target = nullptr;
    float min_dist_sq = std::numeric_limits<float>::max();
    float radius_sq = radius * radius;

    for (IArchTarget* target : targets_) {
        if (!target || !target->isAlive()) continue;

        // target_type: 0=地面, 1=空中, 2=地面+空中
        auto type = target->getTargetType();
        bool is_match = false;

        if (target_type == 2) {
            if (type == IArchTarget::GROUND || type == IArchTarget::AIR) {
                is_match = true;
            }
        }
        else {
            if (static_cast<unsigned char>(type) == target_type) {
                is_match = true;
            }
        }

        if (!is_match) continue;

        // 计算距离
        float dist_sq = center.distanceSquared(target->getCellPosition());

        // 检查是否在攻击范围内且更近
        if (dist_sq <= radius_sq && dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            nearest_target = target;
        }
    }

    return nearest_target;
}

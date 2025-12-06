#include "Troop.h"
#include "TroopAttackManager.h"

Troop::Troop(int level,
             cocos2d::Vec2 position,
             PreferredTarget preferred_target,
             AttackType attack_type,
             uchar housing_space,
             uchar barracks_level_required,
             float movement_speed,
             float attack_speed,
             float range,
             const std::array<float, MAX_TROOP_LEVEL + 1>& damage_per_attacks,
             const std::array<float, MAX_TROOP_LEVEL + 1>& hitpoints,
             const std::array<int, MAX_TROOP_LEVEL + 1>& research_costs,
             const std::array<int, MAX_TROOP_LEVEL + 1>& research_times,
             const std::array<uchar, MAX_TROOP_LEVEL + 1>& laboratory_level_requireds)
    : level_(level)
    , position_(position)
    , current_hitpoints_(0)
    , preferred_target_(preferred_target)
    , attack_type_(attack_type)
    , housing_space_(housing_space)
    , barracks_level_required_(barracks_level_required)
    , movement_speed_(movement_speed)
    , attack_speed_(attack_speed)
    , range_(range)
    , damage_per_attacks_(damage_per_attacks)
    , hitpoints_(hitpoints)
    , research_costs_(research_costs)
    , research_times_(research_times)
    , laboratory_level_requireds_(laboratory_level_requireds)
{
    // 初始化当前生命值为最大生命值
    current_hitpoints_ = hitpoints_[level_];
}

bool Troop::init() {
    if (!Sprite::init()) {
        return false;
    }
    return true;
}

bool Troop::canAttack() const {
    // 基本检查：是否活着，是否有目标等
    return isAlive();
}

void Troop::takeDamage(float damage) {
    current_hitpoints_ -= damage;
    if (current_hitpoints_ <= 0) {
        current_hitpoints_ = 0;
		// TODO: 死亡处理 墓碑显示、禁用攻击与移动等
    }
}

void Troop::setLevel(int level) {
    if (level >= 1 && level <= MAX_TROOP_LEVEL) {
        level_ = level;
        // 升级时恢复满血
        current_hitpoints_ = getMaxHitpoints();
    }
}
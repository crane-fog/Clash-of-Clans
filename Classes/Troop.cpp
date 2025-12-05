#include "Troop.h"
#include "TroopAttackManager.h"

Troop::Troop()
    : level_(1)
    , current_hitpoints_(0)
    , preferred_target_(255)
    , attack_type_(0)
    , housing_space_(1)
    , barracks_level_required_(1)
    , movement_speed_(1.0f)
    , attack_speed_(1.0f)
    , range_(1.0f)
    , damage_per_attacks_{0, 10, 12, 14, 16, 18}  // 示例数据
    , hitpoints_{0, 100, 120, 140, 160, 180}     // 示例数据
    , research_costs_{0, 100, 200, 300, 400, 500}
    , research_times_{0, 1, 2, 3, 4, 5}
    , laboratory_level_requireds_{0, 1, 2, 3, 4, 5}
{
}

bool Troop::init() {
    if (!Sprite::init()) {
        return false;
    }

    // 初始化当前生命值为最大生命值
    current_hitpoints_ = getMaxHitpoints();

    // 注册到攻击管理器
    //TroopAttackManager::getInstance()->registerTroop(this);

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
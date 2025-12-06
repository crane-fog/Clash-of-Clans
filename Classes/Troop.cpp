#include "Troop.h"
#include "TroopAttackManager.h"

Troop::Troop(BaseMap* base_map,
             int level,
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
             const std::array<float, MAX_TROOP_LEVEL + 1>& research_times,
             const std::array<uchar, MAX_TROOP_LEVEL + 1>& laboratory_level_requireds)
	: base_map_(base_map)
    , level_(level)
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
    if (level_ < 1 || level_ > MAX_TROOP_LEVEL) {
        level_ = 1; // 默认等级为1，防止越界
    }
    // 初始化当前生命值为最大生命值
    current_hitpoints_ = hitpoints_[level_];
}
//TODO:所有子类都需要实现init以及create
// 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池 CREATE_FUNC(<Typename>);
// 子类需要展开一下这个宏并用带参数的构造函数替换其中的默认构造函数
 
//bool Troop::init() {
//    if (!Sprite::init()) {
//        return false;
//    }
//    return true;
//}

bool Troop::initWithFile(const std::string& filename) {
    if (!Sprite::initWithFile(filename)) {
        return false;
    }
    return true;
}

bool Troop::canAttack() const {
    // 基本检查：是否活着，是否有目标等
    return isAlive();
}

void Troop::takeDamage(float damage) {
	if (!isAlive()) return; // 已经死亡的部队不能再受伤
    if (damage < 0) return; // 负伤害无效
    current_hitpoints_ -= damage;
    if (current_hitpoints_ <= 0) {
        current_hitpoints_ = 0;
		// TODO: 死亡处理 墓碑显示、禁用攻击与移动等
    }
	//血条显示、更新等，应当是需要用cocos2d::ui::LoadingBar来实现，然后将它挂靠到Troop的子节点上
}

void Troop::setLevel(int level) {
    if (level >= 1 && level <= MAX_TROOP_LEVEL) {
        level_ = level;
        // 升级时恢复满血
        current_hitpoints_ = getMaxHitpoints();
    }
}
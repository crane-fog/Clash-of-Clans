#ifndef __TROOP_H__
#define __TROOP_H__
#include "cocos2d.h"
#include "IArchTarget.h"
#include <array>
#define MAX_TROOP_LEVEL 5//目前做到5级
class Troop :public cocos2d::Sprite, public IArchTarget{
    typedef unsigned char uchar;
protected:
    //当前等级
    int level_;
    //当前网格逻辑位置
    cocos2d::Vec2 position_;
    // 当前生命值
    float current_hitpoints_;
    /*以下为升级时要改变的属性的每级数值，初始化时直接赋值*/
    //每次伤害
    const std::array<float, MAX_TROOP_LEVEL + 1> damage_per_attacks_;

    //生命值
    const std::array<float, MAX_TROOP_LEVEL + 1> hitpoints_;

    //升到level级所需资源花费
    const std::array<int, MAX_TROOP_LEVEL + 1> research_costs_;

    //升到level级所需时间 单位：小时
    const std::array<int, MAX_TROOP_LEVEL + 1> research_times_;

    //升到level级所需实验室等级
    const std::array<uchar, MAX_TROOP_LEVEL + 1> laboratory_level_requireds_;

public:
    /*以下为升级时不改变的属性，初始化时直接赋值，由于是const直接设置为public允许外部读取*/
    enum PreferredTarget : uchar {//与ArchInfo.h里的ArchType对应
        OTHER = 0, // 其它
        RESOURCE = 1, // 资源
        DEFENSE = 2, // 防御
        WALLT = 3, // 城墙
        NONE = 255 // 无
    };
    //攻击偏好建筑类型
    const PreferredTarget preferred_target_;

    enum AttackType : uchar {
        MELEE_SINGLE_GROUND = 0, // 近战单体地面-Barbarian,Giant
        MELEE_AOE_GROUND = 1, // 近战范围地面-WallBreaker
        RANGED_SINGLE_GROUND = 2, // 远程单体地面
        RANGED_AOE_GROUND = 3, // 远程范围地面-Balloon
        RANGED_SINGLE_AIR_GROUND = 4, // 远程单体空中地面-Archer
        RANGED_AOE_AIR_GROUND = 5 // 远程范围空中地面-Dragon
        //TODO:问一下机制：
        //空中单位都不能攻击城墙?-平时没必要，死亡溅射有可能。
        //范围伤害的中心是什么？士兵or目标建筑？-平时目标中心，死亡溅射士兵中心。
    };
    //伤害类型(近战或远程,单体或范围,仅地面目标或地面和空中目标etc)
    const AttackType attack_type_;
    //占据人口
    const uchar housing_space_;
    //所需训练营等级
    const uchar barracks_level_required_;

    //移动速度 格/秒
    const float movement_speed_;
    //攻击速度 秒/次
    const float attack_speed_;
    //攻击距离 格
    const float range_;

public:
    // 构造函数相关
    Troop(int level,
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
          const std::array<uchar, MAX_TROOP_LEVEL + 1>& laboratory_level_requireds);
    virtual ~Troop() = default;

    // 初始化方法
    virtual bool init();

    // 执行攻击（由子类实现具体逻辑）
    virtual void performAttack() = 0;

    // 检查是否可以攻击
    virtual bool canAttack() const;

    /*以下为对接口IArchTarget的实现*/
    // 受到伤害
    virtual void takeDamage(float damage)override;

    // 获取士兵位置-网格逻辑坐标
    virtual cocos2d::Vec2 getCellPosition() const override { return position_; }

    // 检查是否还活着
    virtual bool isAlive() const override{ return current_hitpoints_ > 0; }

    // 获取士兵类型（用于建筑选择目标，如是否为空中兵种）
    virtual ArchTargetType getTargetType() const override = 0;

    /*以下为get&set*/
    // 获取当前生命值
    float getCurrentHitpoints() const { return current_hitpoints_; }

    // 获取最大生命值
    float getMaxHitpoints() const { return hitpoints_[level_]; }

    // 获取当前等级
    int getLevel() const { return level_; }

    // 设置等级
    void setLevel(int level);

    // 升级
    void levelUp() { setLevel(level_ + 1); }

    // 获取攻击伤害
    float getCurrentDamage() const { return damage_per_attacks_[level_]; }

    /*以下为渲染相关*/
	// 创建士兵实例
	static Troop* createTroop(const std::string& picfilename,
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
		const std::array<int, MAX_TROOP_LEVEL + 1>& research_times,
		const std::array<uchar, MAX_TROOP_LEVEL + 1>& laboratory_level_requireds);
};

#endif // __TROOP_H__
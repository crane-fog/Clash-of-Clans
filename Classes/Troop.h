#ifndef __TROOP_H__
#define __TROOP_H__
#include "cocos2d.h"
#include "IArcTarget.h"
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
	const float damage_per_attacks_[MAX_TROOP_LEVEL + 1];

	//生命值
	const float hitpoints_[MAX_TROOP_LEVEL + 1];

	//升到level级所需资源花费
	const int research_costs_[MAX_TROOP_LEVEL + 1];

	//升到level级所需时间 单位：小时
	const int research_times_[MAX_TROOP_LEVEL + 1];

	//升到level级所需实验室等级
	const uchar laboratory_level_requireds_[MAX_TROOP_LEVEL + 1];

public:
	/*以下为升级时不改变的属性，初始化时直接赋值，由于是const直接设置为public允许外部读取*/
	//攻击偏好建筑类型（1=资源建筑、2=防御建筑、3=城墙、0=其它、255=无）
	const uchar preferred_target_;
	//伤害类型(近战或远程,单体或范围,仅地面目标或地面和空中目标etc)
	const uchar attack_type_;
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
	Troop();
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

	// 获取士兵类型（用于建筑选择目标，如是否为空中兵种，具体返回值见todo-士兵）
	virtual unsigned char getTargetType() const = 0;

	/*以下为get&set*/
	// 获取当前生命值
	float getCurrentHitpoints() const { return current_hitpoints_; }

	// 获取最大生命值
	float getMaxHitpoints() const { return hitpoints_[level_]; }

	// 获取当前等级
	int getLevel() const { return level_; }

	// 设置等级
	void setLevel(int level);

	// 获取攻击伤害
	float getCurrentDamage() const { return damage_per_attacks_[level_]; }
};

#endif // __TROOP_H__
#ifndef __TROOP_H__
#define __TROOP_H__
#include <array>

#include "BaseMap.h"
#include "cocos2d.h"
#include "HealthBar.h"
#include "IArchTarget.h"
#include "ITroopTarget.h"

#define MAX_TROOP_LEVEL 5  // 目前做到5级
#define TROOP_TYPE_NUM 6   // 兵种种类数量

class Troop : public cocos2d::Sprite, public IArchTarget {
private:
    // 状态更新方法
    void updateIdleState(float dt);
    void updateMovingState(float dt);
    void updateAttackingState(float dt);
    void updateTargetLostState(float dt);
protected:
    typedef unsigned char uchar;

    // 被创建的场景地图
    BaseMap* base_map_;

    // 血条
    HealthBar* health_bar_;

    // 当前等级
    int level_;

    // 当前网格逻辑位置
    cocos2d::Vec2 position_;

    // 当前生命值
    float current_hitpoints_;

    enum Status : uchar {
        IDLE = 0,         // 空闲
        MOVING = 1,       // 移动
        ATTACKING = 2,    // 攻击
        TARGET_LOST = 3,  // 目标丢失
        DEAD = 4          // 死亡
    };

    // 当前状态
    Status status_;

    // 当前目标
    ITroopTarget* current_target_ = nullptr;

    // 当前移动方向
    cocos2d::Vec2 current_path_direction_;

    // 攻击计时器
    float attack_timer_;

    /*以下为升级时不改变的属性，初始化时直接赋值*/
    // 移动速度 格/秒
    const float kMovementSpeed;
    // 攻击速度 秒/次
    const float kAttackSpeed;
    // 攻击距离 格
    const float kRange;

    /*以下为升级时要改变的属性的每级数值，初始化时直接赋值*/
    // 每次伤害
    const std::array<float, MAX_TROOP_LEVEL + 1> kDamagePerAttacks;
    // 生命值
    const std::array<float, MAX_TROOP_LEVEL + 1> kHitpoints;

    // 执行攻击（由子类实现具体逻辑）
    virtual void performAttack() = 0;

    // 检查是否可以攻击
    virtual bool canAttack() const;

    // 查找新目标（这里提供基础，如有需要子类重写）
    virtual void findNewTarget();

    // 死亡处理（这里提供基础如墓碑显示，如有需要子类重写如死亡溅射伤害等等）
    virtual void onDeath();

    // 每帧更新
    void update(float dt);
public:
    enum TroopType : uchar { BARBARIAN = 0, ARCHER = 1, GIANT = 2, WALL_BREAKER = 3, DRAGON = 4, BALLOON = 5 };

    /*以下为升级时不改变的属性，初始化时直接赋值，由于是const直接设置为public允许外部读取*/
    enum PreferredTarget : uchar {  // 与ArchInfo.h里的ArchType对应
        OTHER = 0,                  // 其它
        RESOURCE = 1,               // 资源
        DEFENSE = 2,                // 防御
        WALLT = 3,                  // 城墙
        TRAP = 4,                   // 陷阱
        NONE = 255                  // 无
    };

    // 攻击偏好建筑类型
    const PreferredTarget kPreferredTarget;

    enum AttackType : uchar {
        MELEE_SINGLE_GROUND = 0,       // 近战单体地面-Barbarian,Giant
        MELEE_AOE_GROUND = 1,          // 近战范围地面-WallBreaker
        RANGED_SINGLE_GROUND = 2,      // 远程单体地面
        RANGED_AOE_GROUND = 3,         // 远程范围地面-Balloon
        RANGED_SINGLE_AIR_GROUND = 4,  // 远程单体空中地面-Archer
        RANGED_AOE_AIR_GROUND = 5      // 远程范围空中地面-Dragon
        // 空中单位都不能攻击城墙?-平时没必要，死亡溅射有可能。
        // 范围伤害的中心是什么？士兵or目标建筑？-平时目标中心，死亡溅射士兵中心。
    };

    // 伤害类型(近战或远程,单体或范围,仅地面目标或地面和空中目标etc)
    const AttackType kAttackType;

    // 构造函数相关
    Troop(BaseMap* base_map, int level, cocos2d::Vec2 position, PreferredTarget preferred_target,
          AttackType attack_type,  float movement_speed,
          float attack_speed, float range, const std::array<float, MAX_TROOP_LEVEL + 1>& damage_per_attacks,
          const std::array<float, MAX_TROOP_LEVEL + 1>& hitpoints);
    virtual ~Troop() = default;

    // 初始化方法，当对象被创建时被自动调用，由于Troop没有实现create，仅由子类调用。
    virtual bool initWithFile(const std::string& filename) override;  // virtual bool init();

    // 状态机相关方法
    void changeStatus(Status new_status);
    void setDead() { status_ = DEAD; }

    // 获取士兵类型索引（用于区分不同子类类型）
    virtual TroopType getTroopTypeIndex() const = 0;

    /*以下为对接口IArchTarget的实现*/
    // 受到伤害
    virtual void takeDamage(float damage) override;

    // 获取士兵位置-网格逻辑坐标
    virtual cocos2d::Vec2 getCellPosition() const override { return position_; }

    // 检查是否还活着
    virtual bool isAlive() const override { return current_hitpoints_ > 0; }

    // 获取士兵类型（用于建筑选择目标，如是否为空中兵种）
    virtual ArchTargetType getTargetType() const override = 0;

    /*以下为get&set*/
    // 设置当前位置-网格逻辑坐标 合法位置直接写入返回true；非法位置则设置到离设置点最近的方形边框上返回false
    bool setCellPosition(const cocos2d::Vec2& position);

    // 获取当前生命值
    float getCurrentHitpoints() const { return current_hitpoints_; }

    // 获取最大生命值
    float getMaxHitpoints() const { return kHitpoints[level_]; }

    // 获取当前等级
    int getLevel() const { return level_; }

    // 设置等级
    void setLevel(int level);

    // 升级
    void levelUp() { setLevel(level_ + 1); }

    // 获取攻击伤害
    float getCurrentDamage() const { return kDamagePerAttacks[level_]; }

    /*以下为渲染相关*/
    // 获取当前位置-像素坐标，子类需要重写来保证视觉上中心在需要的坐标
    virtual cocos2d::Vec2 getPixelPosition() const;

    // 重写setPosition以自动同步ZOrder - 取消，一律在updateMovingState处理
    // virtual void setPosition(const cocos2d::Vec2& pos) override {
    //    Sprite::setPosition(pos);
    //    this->setLocalZOrder(CoordAdaptor::calcOrder(CoordAdaptor::pixelToCell(base_map_,pos))); // 自动同步
    //}

    // 获取兵种字符串名
    static std::string getTroopNameFromEnum(uchar troop_no);
};

// 兵种工厂类
class TroopFactory {
    using Creater = std::function<Troop*(BaseMap*, unsigned char, cocos2d::Vec2)>;

private:
    // 存储各类兵种创建函数的映射表
    static std::map<unsigned char, Creater> creaters;

public:
    // 注册兵种创建函数，在游戏初始化时每种兵种调用一次
    static void registerCreater(unsigned char type, const Creater& creater) { creaters[type] = creater; }

    // 创建兵种实例
    static Troop* createTroop(BaseMap* base_map, unsigned char type, cocos2d::Vec2 position, unsigned char lvl = 1);
};

#endif  // __TROOP_H__
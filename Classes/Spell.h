#pragma once
#ifndef __SPELL_H__
#define __SPELL_H__
#include "cocos2d.h"
#include <vector>
#include <functional>
#include"Troop.h"
enum class SpellType {
    HEAL,           // 治疗
    SPEED_BOOST,    // 加速
    DAMAGE_BOOST,   // 攻击增强
    SLOW_ENEMY,     // 减速敌人
    AREA_DAMAGE,    // 范围伤害
    SHIELD,         // 护盾
    TELEPORT,       // 传送
    FREEZE,         // 冰冻
    RESURRECTION,   // 复活
    MANA_REGENERATION // 魔法恢复
};

enum class SpellTarget {
    ALLY,           // 友方单位
    ENEMY,          // 敌方单位
    ALL,            // 所有单位
    GROUND,         // 地面（区域效果）
    BUILDING,       // 建筑
    SELF            // 自己
};


class Spell : public cocos2d::Sprite {
public:
    // 创建法术
    static Spell* create(BaseMap* baseMap = nullptr,const std::string& auraPath = "spell/healing_effect.png",
        float radius = 5.0f
        );

    // 构造函数
    Spell();

    // 初始化函数
    virtual bool init() override;

    // 自定义初始化函数
    bool initWithParams(BaseMap* baseMap, const std::string& auraPath, float radius);

    // 设置/获取半径
    void setRadius(float radius);
    float getRadius() const { return r_; }

    // 设置/获取位置
    void setCellPosition(const cocos2d::Vec2& position);
    cocos2d::Vec2 getCellPosition() const { return position_; }

    // 设置/获取地图
    void setBaseMap(BaseMap* baseMap);
    BaseMap* getBaseMap() const { return base_map_; }

    // 更新大小（根据半径）
    void updateScaleFromRadius();


private:
    std::string aura_;
    BaseMap* base_map_;
    cocos2d::Vec2 position_;
    float r_;

    // 原始纹理大小
    cocos2d::Size originalTextureSize_= cocos2d::Size(0,0);
};

/*
class SpellBase : public cocos2d::Ref {
public:
    // 创建法术
    static SpellBase* create(SpellType type, const std::string& name,
        float radius, float duration, float value);

    // 施放法术
    virtual bool cast(const cocos2d::Vec2& position);

    // 更新法术效果（持续型法术）
    virtual void update(float dt);

    // 取消法术
    virtual void cancel();

    // Getters
    SpellType getType() const { return _type; }
    std::string getName() const { return _name; }
    std::string getDescription() const { return _description; }
    float getRadius() const { return _radius; }
    float getDuration() const { return _duration; }
    float getRemainingTime() const { return _remainingTime; }
    float getValue() const { return _value; }
    SpellTarget getTarget() const { return _target; }
    bool isActive() const { return _isActive; }
    bool isInstant() const { return _duration <= 0; }

    // Setters
    void setIcon(const std::string& icon) { _icon = icon; }
    void setDescription(const std::string& desc) { _description = desc; }
    void setTarget(SpellTarget target) { _target = target; }
    void setManaCost(float cost) { _manaCost = cost; }
    void setCooldown(float cooldown) { _cooldown = cooldown; }
    void setMaxCharges(int charges) { _maxCharges = charges; }

    // 法术效果回调
    typedef std::function<void(Unit* unit, float value)> SpellEffectCallback;
    void setEffectCallback(const SpellEffectCallback& callback) {
        _effectCallback = callback;
    }

protected:
    virtual bool init(SpellType type, const std::string& name,
        float radius, float duration, float value);

    // 查找范围内的单位
    virtual std::vector<Unit*> findTargetsInRange(const cocos2d::Vec2& position);

    // 应用法术效果
    virtual void applyEffect(Unit* unit);

    // 移除法术效果
    virtual void removeEffect(Unit* unit);

    // 创建视觉效果
    virtual void createVisualEffect(const cocos2d::Vec2& position);
    virtual void updateVisualEffect(float dt);
    virtual void removeVisualEffect();

protected:
    SpellType _type;
    std::string _name;
    std::string _description;
    std::string _icon;

    float _radius;          // 作用半径
    float _duration;        // 持续时间（0表示瞬发）
    float _remainingTime;   // 剩余时间
    float _value;           // 效果数值（治疗量、加速百分比等）

    SpellTarget _target;    // 目标类型
    float _manaCost;        // 魔法消耗
    float _cooldown;        // 冷却时间
    float _currentCooldown; // 当前冷却
    int _maxCharges;        // 最大充能次数
    int _currentCharges;    // 当前充能

    bool _isActive;         // 是否激活
    bool _isCasting;        // 是否正在施放

    // 受影响的单位
    std::vector<Unit*> _affectedUnits;

    // 视觉效果
    cocos2d::Sprite* _effectSprite;
    cocos2d::ParticleSystemQuad* _particleEffect;

    // 效果回调
    SpellEffectCallback _effectCallback;

    // 法术位置
    cocos2d::Vec2 _castPosition;

    // 声音效果
    std::string _castSound;
    std::string _loopSound;
    int _soundId;
};

// SpellHeal.h

class SpellHeal : public SpellBase {
public:
    // 直接使用固定数值的构造函数
    SpellHeal()
        : SpellBase()
    {
    }

    // 创建函数（完全模仿Barbarian风格）
    static SpellHeal* create() {
        SpellHeal* pRet = new(std::nothrow) SpellHeal();
        if (pRet && pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

protected:
    virtual bool init();

    // 静态成员初始化
    const std::array<float, 6> SpellHeal::_healAmounts = { 0, 40, 60, 80, 100, 120 };
    const std::array<float, 6> SpellHeal::_durations = { 0, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f };
    const std::array<float, 6> SpellHeal::_manaCosts = { 0, 20, 25, 30, 35, 40 };
};


bool SpellHeal::init() {
    // 获取法术等级（从游戏管理器）
    int spellLevel = 1; // 默认1级

    float healAmount = _healAmounts[spellLevel];
    float duration = _durations[spellLevel];
    float manaCost = _manaCosts[spellLevel];

    if (!SpellBase::init(SpellType::HEAL, "治疗术", 100.0f, duration, healAmount)) {
        return false;
    }

    setTarget(SpellTarget::ALLY);
    setDescription("治疗范围内的友方单位");
    setIcon("spell/healing_effect.png");
    setManaCost(manaCost);
    setCooldown(15.0f);
    setMaxCharges(2);

    _castSound = "sounds/spell_heal.mp3";

    /* 设置治疗效果回调
    setEffectCallback([this, healAmount, duration](Unit* unit, float value) {
        if (auto troop = dynamic_cast<Troop*>(unit)) {
            if (duration > 0) {
                // 持续治疗
                troop->applySpellEffect(Troop::SpellEffect::HEALING,
                    duration,
                    healAmount / duration);
            }
            else {
                // 瞬发治疗
                troop->heal(healAmount);
            }
        }
        });
        */
   // return true;
//}

#endif // __SPELL_H__
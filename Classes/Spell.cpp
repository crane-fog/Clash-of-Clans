// SpellHeal.cpp
#include "Spell.h"
#include "AudioEngine.h"
#include "Troop.h"  // 包含Troop类
#include "TroopTargetManager.h"  // 假设有这个管理器来获取单位
#include "CoordAdaptor.h"
USING_NS_CC;




// 构造函数
Spell::Spell()
    : base_map_(nullptr)
    , position_(Vec2::ZERO)
    , r_(3.0f)
    , aura_("spell/healing_effect.png") {
}

// 创建函数
Spell* Spell::create( BaseMap* baseMap,const std::string& auraPath, float radius) {
    Spell* pRet = new(std::nothrow) Spell();

    if (pRet && pRet->initWithParams( baseMap,auraPath, radius)) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}
bool Spell::init() {
    // 调用父类的init
    if (!Sprite::init()) {
        return false;
    }

    // 这里可以使用默认参数初始化
    return initWithParams( nullptr,"spell/healing_effect.png", 3.0f);
}
// 初始化函数
bool Spell::initWithParams( BaseMap* baseMap,const std::string& auraPath, float radius) {
    // 调用父类初始化
    if (!Sprite::init()) {
        return false;
    }

    // 保存参数
    aura_ = auraPath;
    r_ = radius;
    base_map_ = baseMap;

    // 加载纹理
    if (!aura_.empty()) {
        // 使用setTexture加载纹理
        this->setTexture(aura_);

        // 检查纹理是否加载成功
        if (!this->getTexture()) {
            CCLOG("警告：无法加载法术纹理: %s", aura_.c_str());

        }

    }

    originalTextureSize_ = this->cocos2d::Node::getContentSize();
    if (originalTextureSize_.width <= 0 || originalTextureSize_.height <= 0) {
        CCLOG("警告：纹理尺寸异常，使用默认尺寸");
        originalTextureSize_ = cocos2d::Size(50, 50);
    }
    // 根据半径调整大小
    updateScaleFromRadius();



    // 设置默认属性
    this->setAnchorPoint(Vec2(0.5f, 0.5f)); // 中心锚点

    return true;
}


// 根据半径调整大小
void Spell::updateScaleFromRadius() {
    if (originalTextureSize_.width <= 0 || originalTextureSize_.height <= 0) {
        CCLOG("错误：原始纹理尺寸无效");
        return;
    }

    // 计算需要的缩放比例
    
    float cellDiameter = r_; 
    cocos2d::Vec2 Ra_;
    Ra_.x = cellDiameter;
    Ra_.y = cellDiameter;
    Ra_ = CoordAdaptor::cellDeltaToPixelDelta(base_map_,Ra_);
    if (base_map_) {
        // 如果有地图，使用地图的格子到像素转换

        float targetPixelDiameter = Ra_.x;

        // 计算缩放比例
        float scaleX = targetPixelDiameter / originalTextureSize_.width;
        float scaleY = targetPixelDiameter / originalTextureSize_.height;

        // 保持纵横比，使用最小的缩放值
        float scale = std::min(scaleX, scaleY);
        this->setScale(scale);

        CCLOG("法术缩放: 半径=%.1f格, 直径=%.1f格, 像素直径=%.1f, 原始尺寸=(%.1f,%.1f), 缩放=%.2f",
            r_, cellDiameter, targetPixelDiameter,
            originalTextureSize_.width, originalTextureSize_.height, scale);
    }

}

// 设置半径
void Spell::setRadius(float radius) {
    if (radius <= 0) {
        CCLOG("警告：法术半径必须大于0，当前值: %.1f", radius);
        return;
    }

    r_ = radius;
    updateScaleFromRadius();

    CCLOG("法术半径更新为: %.1f", r_);
}

// SpellBase 实现
/*
SpellBase* SpellBase::create(SpellType type, const std::string& name,
    float radius, float duration, float value) {
    SpellBase* pRet = new(std::nothrow) SpellBase();
    if (pRet && pRet->init(type, name, radius, duration, value)) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool SpellBase::init(SpellType type, const std::string& name,
    float radius, float duration, float value) {

    _type = type;
    _name = name;
    _radius = radius;
    _duration = duration;
    _remainingTime = duration;
    _value = value;

    // 默认值
    _target = SpellTarget::ALLY;
    _manaCost = 0.0f;
    _cooldown = 0.0f;
    _currentCooldown = 0.0f;
    _maxCharges = 1;
    _currentCharges = 1;
    _isActive = false;
    _isCasting = false;

    _effectSprite = nullptr;
    _particleEffect = nullptr;
    _soundId = -1;

    return true;
}

bool SpellBase::cast(const cocos2d::Vec2& position) {
    if (_isCasting || _isActive) {
        CCLOG("法术正在施放或已激活，无法再次施放");
        return false;
    }

    if (_currentCooldown > 0) {
        CCLOG("法术冷却中，剩余时间: %.1f秒", _currentCooldown);
        return false;
    }

    if (_currentCharges <= 0) {
        CCLOG("法术充能耗尽");
        return false;
    }

    CCLOG("开始施放法术: %s 在位置 (%.1f, %.1f)",
        _name.c_str(), position.x, position.y);

    _isCasting = true;
    _castPosition = position;

    // 1. 查找范围内的目标
    //auto targets = findTargetsInRange(position);
   // CCLOG("找到 %zu 个目标", targets.size());

    /*2. 对每个目标应用效果
    for (auto target : targets) {
        if (target && target->isAlive()) {
            applyEffect(target);
            _affectedUnits.push_back(target);
        }
    }

    // 3. 创建视觉效果
    createVisualEffect(position);

    // 4. 播放音效
    if (!_castSound.empty()) {
        //_soundId = experimental::AudioEngine::play2d(_castSound);
    }

    // 5. 更新状态
    _isCasting = false;
    _isActive = (_duration > 0); // 持续法术才需要激活状态

    // 6. 消耗充能
    _currentCharges--;
    if (_currentCooldown <= 0 && _currentCharges < _maxCharges) {
        _currentCooldown = _cooldown;
    }

    return true;
}

void SpellBase::update(float dt) {
    // 更新冷却
    if (_currentCooldown > 0) {
        _currentCooldown -= dt;
        if (_currentCooldown <= 0 && _currentCharges < _maxCharges) {
            _currentCharges++;
            if (_currentCharges < _maxCharges) {
                _currentCooldown = _cooldown; // 继续充能下一个
            }
        }
    }

    // 更新持续法术
    if (_isActive && _duration > 0) {
        _remainingTime -= dt;

        if (_remainingTime <= 0) {
            // 法术结束
            cancel();
        }
        else {
            // 更新持续效果（例如持续治疗）
            updateVisualEffect(dt);

            /*对受影响的单位进行持续效果更新
            for (auto it = _affectedUnits.begin(); it != _affectedUnits.end();) {
                if (!(*it) || !(*it)->isAlive()) {
                    it = _affectedUnits.erase(it);
                }
                else {
                    // 这里可以添加持续效果的逻辑
                    // 例如：每秒治疗
                    it++;
                }
            }
 
        }
    }
}

void SpellBase::cancel() {
    CCLOG("取消法术: %s", _name.c_str());

    // 移除对单位的效果
    for (auto unit : _affectedUnits) {
        if (unit) {
            removeEffect(unit);
        }
    }
    _affectedUnits.clear();

    // 移除视觉效果
    removeVisualEffect();

    /*停止音效
    if (_soundId != -1) {
        experimental::AudioEngine::stop(_soundId);
        _soundId = -1;
    }

    if (!_loopSound.empty()) {
        experimental::AudioEngine::stopAll();
    }


    // 重置状态
    _isActive = false;
    _isCasting = false;
    _remainingTime = _duration;
}
*/

/*std::vector<Unit*> SpellBase::findTargetsInRange(const cocos2d::Vec2& position) {
    std::vector<Unit*> targets;

    // 这里需要根据_target类型查找不同的单位
    // 假设有一个全局的单位管理器
    auto allUnits = TroopTargetManager::getInstance()->getAllTroops();

    for (auto unit : allUnits) {
        if (!unit || !unit->isAlive()) continue;

        // 检查目标类型
        bool isValidTarget = false;
        switch (_target) {
            case SpellTarget::ALLY:
                // 假设unit有方法判断是否是友方
                // 这里简化处理，假设所有Troop都是友方
                isValidTarget = true;
                break;
            case SpellTarget::ENEMY:
                // 敌方单位（可能是建筑或其他）
                break;
            case SpellTarget::ALL:
                isValidTarget = true;
                break;
            case SpellTarget::GROUND:
            case SpellTarget::BUILDING:
            case SpellTarget::SELF:
                // 这些类型需要特殊处理
                break;
        }

        if (!isValidTarget) continue;

        // 检查距离
        cocos2d::Vec2 unitPos = unit->getCellPosition();
        float distance = position.distance(unitPos);

        if (distance <= _radius) {
            targets.push_back(unit);
        }
    }

    return targets;
}
*/
/*
void SpellBase::applyEffect(Unit* unit) {
    if (!unit) return;

    CCLOG("对单位应用法术效果: %s, 值: %.1f",
        _name.c_str(), _value);

    // 基本效果：治疗或伤害
    // 这里只是一个示例，具体效果由子类实现

    if (_effectCallback) {
        _effectCallback(unit, _value);
    }
}

void SpellBase::removeEffect(Unit* unit) {
    // 基础类不做具体移除操作
    // 子类可以重写这个方法
    CCLOG("从单位移除法术效果");
}

void SpellBase::createVisualEffect(const cocos2d::Vec2& position) {
    // 基础视觉效果：创建一个简单的光环
    _effectSprite = Sprite::create();

    // 创建一个圆形光环
    auto drawNode = DrawNode::create();

    // 根据法术类型选择颜色
    Color4F effectColor;
    switch (_type) {
        case SpellType::HEAL:
            effectColor = Color4F::GREEN;
            break;
        case SpellType::SPEED_BOOST:
            effectColor = Color4F::BLUE;
            break;
        case SpellType::DAMAGE_BOOST:
            effectColor = Color4F::RED;
            break;
        default:
            effectColor = Color4F::WHITE;
            break;
    }

    // 绘制外圈
    int segments = 60;
    for (int i = 0; i < segments; ++i) {
        float angle1 = 2 * M_PI * i / segments;
        float angle2 = 2 * M_PI * (i + 1) / segments;

        Vec2 p1 = Vec2(cos(angle1) * _radius, sin(angle1) * _radius);
        Vec2 p2 = Vec2(cos(angle2) * _radius, sin(angle2) * _radius);

        drawNode->drawSegment(p1, p2, 3.0f, effectColor);
    }

    // 绘制半透明填充
    drawNode->drawSolidCircle(Vec2::ZERO, _radius, 0, 30,
        1.0f, 1.0f,
        Color4F(effectColor.r, effectColor.g, effectColor.b, 0.2f));

    _effectSprite->addChild(drawNode);
    _effectSprite->setPosition(position);

    // 添加到当前运行的场景
    auto scene = Director::getInstance()->getRunningScene();
    if (scene) {
        scene->addChild(_effectSprite, 10);
    }

    // 淡入淡出动画
    _effectSprite->setOpacity(0);
    auto fadeIn = FadeIn::create(0.3f);
    auto delay = DelayTime::create(_duration > 0 ? _duration : 1.0f);
    auto fadeOut = FadeOut::create(0.5f);
    auto remove = RemoveSelf::create(true);

    _effectSprite->runAction(Sequence::create(fadeIn, delay, fadeOut, remove, nullptr));


}


void SpellBase::updateVisualEffect(float dt) {
    // 更新视觉效果，例如让光环旋转
    if (_effectSprite) {
        auto rotate = RotateBy::create(1.0f, 90);
        _effectSprite->runAction(rotate);
    }

}

void SpellBase::removeVisualEffect() {
    if (_effectSprite) {
        _effectSprite->removeFromParent();
        _effectSprite = nullptr;
    }

    if (_particleEffect) {
        _particleEffect->removeFromParent();
        _particleEffect = nullptr;
    }
}
*/



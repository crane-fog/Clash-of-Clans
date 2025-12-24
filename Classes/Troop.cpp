#include "Troop.h"
#include "TroopTargetManager.h"
#include "BaseMap.h"
#include "CalculateHelper.h"
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
             const std::array<float, MAX_TROOP_LEVEL + 1>& hitpoints)
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
    , status_(IDLE)
    , current_target_(nullptr)
    , current_path_direction_(cocos2d::Vec2::ZERO)
    , attack_timer_(0.0f)
{
    if (level_ < 1 || level_ > MAX_TROOP_LEVEL) {
        level_ = 1; // 默认等级为1，防止越界
    }
    // 初始化当前生命值为最大生命值
    current_hitpoints_ = hitpoints_[level_];
    health_bar_ = nullptr;
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
	// 创建并添加血条
    health_bar_ = HealthBar::create(hitpoints_[level_]);
    if(health_bar_ == nullptr) {
        return false;
	}
    this->addChild(health_bar_,20);
    // 设置精灵大小
    //this->setScale(1.5f);  // 根据需要调整大小

    // 设置锚点为底部中心
    this->setAnchorPoint(cocos2d::Vec2(0.5, 0));

    this->setPosition(getPixelPosition());
    base_map_->addChild(this, CoordAdaptor::calcOrder(position_));


    
    // 设置血条在士兵头顶正中心
    float offset_y = this->getContentSize().height + 1.0f; // 1 是额外间距
    float offset_x = this->getContentSize().width/2;
    health_bar_->setPosition(cocos2d::Vec2(offset_x, offset_y));

    this->scheduleUpdate();
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
	health_bar_->takeDamage(damage);
    if (current_hitpoints_ <= 0) {
        current_hitpoints_ = 0;
        health_bar_->setVisible(false);
		onDeath();
    }
}

void Troop::onDeath() {
	this->setTexture("troop/tomb.png");
	this->setScale(0.6f);  // 根据需要调整大小
    changeStatus(DEAD);
}

void Troop::setLevel(int level) {
    if (level >= 1 && level <= MAX_TROOP_LEVEL) {
        level_ = level;
        // 升级时恢复满血
        current_hitpoints_ = getMaxHitpoints();
    }
}

bool Troop::setCellPosition(const cocos2d::Vec2& position) {
    if (position.x >= 0 && position.x <= 44 && position.y >= 0 && position.y <= 44) {
        // 位置在合法范围内
        position_ = position;
        return true;
    }
    else {
        // 位置在非法范围内，设置到最近的边框上
        cocos2d::Vec2 newPos = position;

        // 限制x坐标在[0, 44]范围内
        if (newPos.x < 0) {
            newPos.x = 0;
        }
        else if (newPos.x > 44) {
            newPos.x = 44;
        }

        // 限制y坐标在[0, 44]范围内
        if (newPos.y < 0) {
            newPos.y = 0;
        }
        else if (newPos.y > 44) {
            newPos.y = 44;
        }

        position_ = newPos;
        return false;
    }
}

void Troop::update(float dt) {
    if(!isAlive()) {
        return;
	}

    // 状态机逻辑
    switch (status_) {
        case IDLE:
            updateIdleState(dt);
            break;
        case MOVING:
            updateMovingState(dt);
            break;
        case ATTACKING:
            updateAttackingState(dt);
            break;
        case TARGET_LOST:
            updateTargetLostState(dt);
            break;
    }
}

void Troop::updateIdleState(float dt) {
    // 查找目标
    findNewTarget();

    // 如果找到目标，切换到移动状态
    if (current_target_) {
        changeStatus(MOVING);
    }
}

void Troop::updateMovingState(float dt) {
    // 更新ZOrder
    bool is_air = (getTroopTypeIndex() == Troop::BALLOON || getTroopTypeIndex() == Troop::DRAGON);
    int currentZ = CoordAdaptor::calcOrder(CoordAdaptor::pixelToCell(base_map_, getPosition()));
    if (is_air) {
        if (currentZ+20 != this->getLocalZOrder()) {
            this->setLocalZOrder(currentZ+20);
        }
    }
    else {
        if (currentZ != this->getLocalZOrder()) {
            this->setLocalZOrder(currentZ);
        }
    }
    
    // 检查目标是否还有效
    if (!current_target_ || !current_target_->isAlive()) {
        changeStatus(TARGET_LOST);
        return;
    }

    // 检查是否已经在攻击范围内
    if (TroopTargetManager::getInstance()->isInAttackRange(getCellPosition(), current_target_, range_)) {
        changeStatus(ATTACKING);
        return;
    }

    // 获取移动方向（地面兵种考虑阻挡，空中兵种忽略）
    current_path_direction_ = TroopTargetManager::getInstance()->getNextMoveDirection(getCellPosition(), current_target_, range_, is_air);

    // 执行移动
    if (current_path_direction_ != cocos2d::Vec2::ZERO) {
        // 归一化方向向量，确保所有方向的移动速度一致
        // 斜向向量（如{1,1}）的模长为sqrt(2)，需要归一化为单位向量
        current_path_direction_.normalize();
        cocos2d::Vec2 new_position;
        new_position = getCellPosition() + current_path_direction_ * movement_speed_ * dt;
        //撞到墙上了
        //TODO:感觉路径上的墙这一块还要改
        if (!is_air&&TroopTargetManager::getInstance()->isCellWall(new_position)) {
            ITroopTarget* wall = TroopTargetManager::getInstance()->getTroopTargetByCellPos(new_position);
            float size;
            new_position = getCellPosition() + current_path_direction_ * (-range_ +
                CalculateHelper::calculateDistanceToSquare(getCellPosition(), wall->getCellPosition(size), 1.0f));
            current_target_ = wall;
            changeStatus(ATTACKING);
        }
        setCellPosition(new_position);
        setPosition(getPixelPosition());
    }
	}

void Troop::updateAttackingState(float dt) {
    // 检查目标是否还有效
    if (!current_target_ || !current_target_->isAlive()) {
        changeStatus(TARGET_LOST);
        return;
    }

    // 检查是否还在攻击范围内
    /*if (!TroopTargetManager::getInstance()->isInAttackRange(getCellPosition(), current_target_, range_)) {
        changeStatus(MOVING);
        return;
    }*/

	// 攻击定时逻辑
    attack_timer_ += dt;
    if (attack_timer_ >= attack_speed_) {
        performAttack();
        attack_timer_ = 0.0f;
    }
    // 如果目标被摧毁，标记为目标丢失
    if (!current_target_->isAlive()) {
        changeStatus(TARGET_LOST);
    }
}

void Troop::updateTargetLostState(float dt) {
    // 清除当前目标
    current_target_ = nullptr;
    current_path_direction_ = cocos2d::Vec2::ZERO;

    // 查找新目标
    findNewTarget();

    // 如果找到目标，切换到移动状态；否则保持IDLE状态
    if (current_target_) {
        changeStatus(MOVING);
    } else {
        changeStatus(IDLE);
    }
}

void Troop::changeStatus(Status new_status) {
    if(status_!=DEAD) status_ = new_status;
}

void Troop::findNewTarget() {
    float min_dist;
    current_target_ = TroopTargetManager::getInstance()->getNearestTroopTarget(
        getCellPosition(), min_dist, false, preferred_target_);
}

std::string Troop::getTroopNameFromEnum(uchar troop_no) {
    switch (troop_no) {
        case BARBARIAN:
            return "野蛮人";
        case ARCHER:
            return "弓箭手";
        case GIANT:
            return "巨人";
        case WALL_BREAKER:
            return "炸弹人";
        case DRAGON:
            return "飞龙";
        case BALLOON:
            return "气球兵";
        default:
            return "Unknown";
    }
}
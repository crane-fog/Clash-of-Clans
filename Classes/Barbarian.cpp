#include "Barbarian.h"
#include "TroopTargetManager.h"
#include "TroopAttackManager.h"
Barbarian::Barbarian(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, NONE, MELEE_SINGLE_GROUND,1,1,2,1,0.4,
        std::array<float, MAX_TROOP_LEVEL + 1>({0,9,12,15,18,23}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,45,54,65,85,105}), 
        std::array<int, MAX_TROOP_LEVEL + 1>({0,0,10000,50000,130000,300000}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,0,0.5,1,2,4}),
        std::array<uchar, MAX_TROOP_LEVEL + 1>({0,1,1,3,5,6})
        )
{
}

Barbarian* Barbarian::create(BaseMap* base_map, int level, cocos2d::Vec2 position) {
    Barbarian* pRet = new(std::nothrow) Barbarian(base_map,level,position); 
    if (pRet && pRet->initWithFile(pics_barbarian.at(level))) {
        pRet->autorelease(); 
        return pRet;
    }
    else {
        delete pRet; pRet = nullptr; return nullptr;
    }
}

bool Barbarian::initWithFile(const std::string& filename) {
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    //base_map_->sprites_.push_back(this);
    return true;
}

void Barbarian::performAttack() {
    if (!current_target_ || !current_target_->isAlive()) {
        changeStatus(TARGET_LOST);
        return;
    }

    // 检查是否在攻击范围内
    if (!TroopTargetManager::getInstance()->isInAttackRange(getCellPosition(), current_target_, this)) {
        changeStatus(MOVING);
        return;
    }

    // 执行近战攻击
    float damage = getCurrentDamage();
    current_target_->takeDamage(damage);

    // 如果目标被摧毁，标记为目标丢失
    if (!current_target_->isAlive()) {
        changeStatus(TARGET_LOST);
    }

    // 播放攻击动画
    // 播放攻击音效
}

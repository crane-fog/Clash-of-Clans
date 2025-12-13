#include "Archer.h"
#include "TroopTargetManager.h"
#include "TroopAttackManager.h"

Archer::Archer(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, NONE, RANGED_SINGLE_AIR_GROUND, 1, 2, 3.0f, 1.0f, 3.5f,
        std::array<float, MAX_TROOP_LEVEL + 1>({0,8,10,13,16,20}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,22,26,29,33,40}),
        std::array<int, MAX_TROOP_LEVEL + 1>({0,0,20000,80000,200000,500000}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,0,1,2,3,8}),
        std::array<uchar, MAX_TROOP_LEVEL + 1>({0,1,1,3,5,6})
        )
{
}

Archer* Archer::create(BaseMap* base_map, int level, cocos2d::Vec2 position) {
    Archer* pRet = new(std::nothrow) Archer(base_map, level, position);
    if (pRet && pRet->initWithFile(pics_archer.at(level))) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet; pRet = nullptr; return nullptr;
    }
}

bool Archer::initWithFile(const std::string& filename) {
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    //base_map_->sprites_.push_back(this);
    return true;
}

void Archer::performAttack() {
    if (!current_target_ || !current_target_->isAlive()) {
        changeStatus(TARGET_LOST);
        return;
    }

    // 检查是否在攻击范围内
    if (!TroopTargetManager::getInstance()->isInAttackRange(getCellPosition(), current_target_, this)) {
        changeStatus(MOVING);
        return;
    }

    // 执行远程攻击
    float damage = getCurrentDamage();
    current_target_->takeDamage(damage);

    // 如果目标被摧毁，标记为目标丢失
    if (!current_target_->isAlive()) {
        changeStatus(TARGET_LOST);
    }

    // 播放攻击动画
    // 播放攻击音效
}

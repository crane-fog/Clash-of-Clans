#include "Giant.h"
#include "TroopTargetManager.h"
#include "TroopAttackManager.h"

Giant::Giant(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, DEFENSE, MELEE_SINGLE_GROUND, 5, 3, 1.5f, 2.0f, 1.0f,
        std::array<float, MAX_TROOP_LEVEL + 1>({0,24,30,40,48,62}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,400,500,600,700,900}),
        std::array<int, MAX_TROOP_LEVEL + 1>({0,0,40000,150000,400000,800000}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,0,2,4,6,12}),
        std::array<uchar, MAX_TROOP_LEVEL + 1>({0,0,2,4,5,6})
        )
{
}

Giant* Giant::create(BaseMap* base_map, int level, cocos2d::Vec2 position) {
    Giant* pRet = new(std::nothrow) Giant(base_map, level, position);
    if (pRet && pRet->initWithFile(pics_giant.at(level))) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet; pRet = nullptr; return nullptr;
    }
}

bool Giant::initWithFile(const std::string& filename) {
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    //base_map_->sprites_.push_back(this);
    return true;
}

void Giant::performAttack() {

    // 执行近战攻击
    float damage = getCurrentDamage();
    current_target_->takeDamage(damage);

    // 播放攻击动画

    // 播放攻击音效
}

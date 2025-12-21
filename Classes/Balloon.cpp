#include "Balloon.h"
#include "TroopTargetManager.h"
Balloon::Balloon(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, DEFENSE, RANGED_AOE_GROUND, 5, 6, 1.3f, 3.0f,0.0f,
        std::array<float, MAX_TROOP_LEVEL + 1>({0,75,96,144,216,324}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,150,180,216,280,390}),
        std::array<int, MAX_TROOP_LEVEL + 1>({0,0,100000,400000,720000,1300000}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,0,4,6,18,24}),
        std::array<uchar, MAX_TROOP_LEVEL + 1>({0,1,2,4,5,6})
        )
{
}

Balloon* Balloon::create(BaseMap* base_map, int level, cocos2d::Vec2 position) {
    Balloon* pRet = new(std::nothrow) Balloon(base_map,level,position);
    if (pRet && pRet->initWithFile(pics_balloon.at(level))) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet; pRet = nullptr; return nullptr;
    }
}

bool Balloon::initWithFile(const std::string& filename) {
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    //base_map_->sprites_.push_back(this);
    return true;
}

void Balloon::performAttack() {
    // TODO:执行范围攻击
    float damage = getCurrentDamage();
    current_target_->takeDamage(damage);

    // 播放攻击动画
    // 播放攻击音效
}

void Balloon::onDeath() {
    //TODO:死亡溅射伤害
}
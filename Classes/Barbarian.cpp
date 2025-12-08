#include "Barbarian.h"
#include "TroopTargetManager.h"
#include "TroopAttackManager.h"
#include "BaseMap.h"
#include "CoordAdaptor.h"
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
    if (pRet && pRet->initWithFile(pics.at(level))) {
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

    // 设置精灵大小
    //this->setScale(1.5f);  // 根据需要调整大小

    // 设置锚点为中心
    this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));

    setPosition(CoordAdaptor::cellToPixel(base_map_, position_));
	base_map_->addChild(this, 3);//TODO:这个地方的层级需要调整，需要在自身x大于建筑、y小于建筑时显示在建筑之上，反之显示在建筑之下
	//base_map_->sprites_.push_back(this);
    return true;
}

void Barbarian::performAttack() {
    // 查找最近的目标建筑（优先资源建筑）
    float distance;
    ITroopTarget* target = TroopTargetManager::getInstance()
        ->getNearestTroopTarget(getCellPosition(),distance,true,NONE);

    if (!target || !target->isAlive()) {
        return;  // 没有有效目标
    }

    // 检查是否在攻击范围内（近战攻击需要非常近的距离）
    if (distance > range_) {
        return;  // 距离太远，无法攻击
    }

    // 执行近战攻击
    float damage = getCurrentDamage();
    target->takeDamage(damage);

    // 播放攻击动画

    // 播放攻击音效
}

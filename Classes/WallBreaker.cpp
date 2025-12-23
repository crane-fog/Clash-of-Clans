#include "WallBreaker.h"
#include "TroopTargetManager.h"
#include "AudioEngine.h"

WallBreaker::WallBreaker(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, WALLT, MELEE_AOE_GROUND, 2, 5, 3.0f, 0.0f, 0.5f,
        std::array<float, MAX_TROOP_LEVEL + 1>({ 0,10,20,25,30,43 }),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,20,24,29,35,53}),
        std::array<int, MAX_TROOP_LEVEL + 1>({0,0,80000,200000,450000,1000000}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,0,3,4,12,16}),
        std::array<uchar, MAX_TROOP_LEVEL + 1>({0,1,2,4,5,6})
        )
{
}

WallBreaker* WallBreaker::create(BaseMap* base_map, int level, cocos2d::Vec2 position) {
    WallBreaker* pRet = new(std::nothrow) WallBreaker(base_map,level,position);
    if (pRet && pRet->initWithFile(pics_wallbreaker.at(level))) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet; pRet = nullptr; return nullptr;
    }
}

bool WallBreaker::initWithFile(const std::string& filename) {
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    //base_map_->sprites_.push_back(this);
    return true;
}

void WallBreaker::findNewTarget() {
    TroopTargetManager* target_manager = TroopTargetManager::getInstance();

    // 1. 首先尝试获取最优的墙壁目标（周围至少有2个墙壁的墙）
    ITroopTarget* optimal_wall = target_manager->getOptimalWallTarget(position_);
    if (optimal_wall != nullptr) {
        current_target_ = optimal_wall;
        return;
    }

    // 2. 如果没有最优墙壁，获取最近的墙壁
    float min_distance;
    ITroopTarget* nearest_wall = target_manager->getNearestTroopTarget(position_, min_distance, true, Troop::WALLT);
    if (nearest_wall != nullptr) {
        current_target_ = nearest_wall;
        return;
    }

    // 3. 如果连墙都没有，获取最近的建筑
    ITroopTarget* nearest_building = target_manager->getNearestTroopTarget(position_, min_distance, false, Troop::NONE);
    current_target_ = nearest_building;
}

void WallBreaker::performAttack() {
    // 执行近战范围攻击
    
    float damage = getCurrentDamage();
    if (current_target_->getTargetType() == Troop::WALLT) {
		damage *= 40; // 对墙壁伤害加成
    }
    float size;
    cocos2d::Vec2 center = current_target_->getCellPosition(size);
    std::vector<ITroopTarget*> targets = TroopTargetManager::getInstance()->getTargetsInRange(
        center, area_splash_radius_);
    for (auto target : targets) {
        target->takeDamage(damage);
    }


    // 播放攻击动画

    //// 播放攻击音效
    //int wallbreaker_hit = cocos2d::AudioEngine::play2d("music/babarian_hit.mp3",false,0.7f);
    //// 检查音频的状态，直到播放完成
    //this->schedule([wallbreaker_hit, this](float dt) {
    //    if (cocos2d::AudioEngine::getState(wallbreaker_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
    //        // 停止音效播放并释放资源
    //        cocos2d::AudioEngine::uncache("music/babarian_hit.mp3");
    //        this->unschedule("stop_audio_key"); // 停止检查
    //    }
    //    }, 0.1f, "stop_audio_key");

	// 攻击后自爆
    this->setTexture("troop/tomb.png");
    this->setScale(0.6f);  // 根据需要调整大小
    changeStatus(DEAD);
	this->takeDamage(this->getCurrentHitpoints()); // 自爆
}

void WallBreaker::onDeath() {
    if(status_==DEAD)
		return;
    float damage = damages_upon_death_[level_];
    std::vector<ITroopTarget*> targets = TroopTargetManager::getInstance()->getTargetsInRange(
        getCellPosition(), death_damage_radius_);
    for (auto target : targets) {
        if(target->getTargetType()== Troop::WALLT)
			damage *= 40; // 对墙壁伤害加成
        target->takeDamage(damage);
    }
    this->setTexture("troop/tomb.png");
    this->setScale(0.6f);  // 根据需要调整大小
    changeStatus(DEAD);
}
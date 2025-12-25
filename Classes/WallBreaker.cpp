#include "WallBreaker.h"

#include "AudioEngine.h"
#include "TroopTargetManager.h"

// 升到level级所需资源花费
const std::array<int, MAX_TROOP_LEVEL + 1> WallBreaker::kResearchCosts = {0, 0, 80000, 200000, 450000, 1000000};

// 升到level级所需时间 单位：小时
const std::array<float, MAX_TROOP_LEVEL + 1> WallBreaker::kResearchTimes = {0, 0, 3, 4, 12, 16};

// 升到level级所需实验室等级
const std::array<Troop::uchar, MAX_TROOP_LEVEL + 1> WallBreaker::kLaboratoryLevelRequireds = {0, 1, 2, 4, 5, 6};

WallBreaker::WallBreaker(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, WALLT, MELEE_AOE_GROUND, 2, 5, 3.0f, 0.0f, 0.5f,
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 10, 20, 25, 30, 43}),
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 20, 24, 29, 35, 53}))
{}

WallBreaker* WallBreaker::create(BaseMap* base_map, int level, cocos2d::Vec2 position)
{
    WallBreaker* p_ret = new (std::nothrow) WallBreaker(base_map, level, position);
    if (p_ret && p_ret->initWithFile(kPicsWallbreaker.at(level))) {
        p_ret->autorelease();
        return p_ret;
    }
    else {
        delete p_ret;
        p_ret = nullptr;
        return nullptr;
    }
}

bool WallBreaker::initWithFile(const std::string& filename)
{
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    // base_map_->sprites_.push_back(this);
    return true;
}

void WallBreaker::findNewTarget()
{
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

void WallBreaker::performAttack()
{
    // 执行近战范围攻击

    float damage = getCurrentDamage();
    if (current_target_->getTargetType() == Troop::WALLT) {
        damage *= 40;  // 对墙壁伤害加成
    }
    float size;
    cocos2d::Vec2 center = current_target_->getCellPosition(size);
    std::vector<ITroopTarget*> targets =
        TroopTargetManager::getInstance()->getTargetsInRange(center, kAreaSplashRadius);
    for (auto target : targets) {
        target->takeDamage(damage);
        float size;
        // 播放攻击动画
        playBlackSmokeAt(
            CoordAdaptor::cellDeltaToPixelDelta(base_map_, target->getCellPosition(size) - this->getCellPosition()));
    }

    //// 播放攻击音效
    // int wallbreaker_hit = cocos2d::AudioEngine::play2d("music/babarian_hit.mp3",false,0.7f);
    //// 检查音频的状态，直到播放完成
    // this->schedule([wallbreaker_hit, this](float dt) {
    //     if (cocos2d::AudioEngine::getState(wallbreaker_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
    //         // 停止音效播放并释放资源
    //         cocos2d::AudioEngine::uncache("music/babarian_hit.mp3");
    //         this->unschedule("stop_audio_key"); // 停止检查
    //     }
    //     }, 0.1f, "stop_audio_key");

    // 攻击后自爆
    this->setTexture("troop/tomb.png");
    this->setScale(0.6f);  // 根据需要调整大小
    changeStatus(DEAD);
    this->takeDamage(this->getCurrentHitpoints());  // 自爆
}

void WallBreaker::onDeath()
{
    if (status_ == DEAD) return;
    float damage = damages_upon_death_[level_];
    std::vector<ITroopTarget*> targets =
        TroopTargetManager::getInstance()->getTargetsInRange(getCellPosition(), kDeathDamageRadius);
    for (auto target : targets) {
        if (target->getTargetType() == Troop::WALLT) damage *= 40;  // 对墙壁伤害加成
        target->takeDamage(damage);
        float size;
        playBlackSmokeAt(
            CoordAdaptor::cellDeltaToPixelDelta(base_map_, target->getCellPosition(size) - this->getCellPosition()));
    }
    this->setTexture("troop/tomb.png");
    this->setScale(0.6f);  // 根据需要调整大小
    changeStatus(DEAD);
}

void WallBreaker::playBlackSmokeAt(const cocos2d::Vec2& pos)
{
    auto smoke = Sprite::create("troop/smoke.png");
    if (!smoke) return;

    smoke->setColor(cocos2d::Color3B(100, 100, 100));
    smoke->setPosition(pos);
    smoke->setScale(2.0f);
    smoke->setOpacity(255);
    this->addChild(smoke, 10);  // zOrder 可根据需要调整
                                // smoke->setGlobalZOrder(1000); // 确保在最前面显示
    auto scale = cocos2d::ScaleTo::create(0.6f, 1.2f);
    auto fade = cocos2d::FadeOut::create(1.0f);
    auto cleanup = cocos2d::RemoveSelf::create();

    smoke->runAction(cocos2d::Sequence::create(cocos2d::Spawn::create(scale, fade, nullptr), cleanup, nullptr));
}

cocos2d::Vec2 WallBreaker::getPixelPosition() const
{
    return CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(position_.x + 0.1f, position_.y - 0.3f));
}
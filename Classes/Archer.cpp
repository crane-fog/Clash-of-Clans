#include "Archer.h"

#include "AudioEngine.h"
#include "CocUtility.h"
#include "TroopTargetManager.h"

// 升到level级所需资源花费
const std::array<int, MAX_TROOP_LEVEL + 1> Archer::kResearchCosts = {0, 0, 20000, 80000, 200000, 500000};

// 升到level级所需时间 单位：小时
const std::array<float, MAX_TROOP_LEVEL + 1> Archer::kResearchTimes = {0, 0, 1, 2, 3, 8};

// 升到level级所需实验室等级
const std::array<Troop::uchar, MAX_TROOP_LEVEL + 1> Archer::kLaboratoryLevelRequireds = {0, 1, 1, 3, 5, 6};

Archer::Archer(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, NONE, RANGED_SINGLE_AIR_GROUND, 1, 2, 3.0f, 1.0f, 3.5f,
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 8, 10, 13, 16, 20}),
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 22, 26, 29, 33, 40}))
{}

Archer* Archer::create(BaseMap* base_map, int level, cocos2d::Vec2 position)
{
    Archer* p_ret = new (std::nothrow) Archer(base_map, level, position);
    if (p_ret && p_ret->initWithFile(kPicsArcher.at(level))) {
        p_ret->autorelease();
        return p_ret;
    }
    else {
        delete p_ret;
        p_ret = nullptr;
        return nullptr;
    }
}

bool Archer::initWithFile(const std::string& filename)
{
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    // base_map_->sprites_.push_back(this);
    return true;
}

void Archer::performAttack()
{
    // 执行远程攻击
    float damage = getCurrentDamage();
    current_target_->takeDamage(damage);

    // 播放攻击动画
    // 播放攻击音效
    int archer_hit = cocos2d::AudioEngine::play2d("music/archer_hit.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule(
        [archer_hit, this](float dt) {
            if (cocos2d::AudioEngine::getState(archer_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                // 停止音效播放并释放资源
                cocos2d::AudioEngine::uncache("music/archer_hit.mp3");
                this->unschedule("stop_audio_key");  // 停止检查
            }
        },
        0.1f, "stop_audio_key");
}

cocos2d::Vec2 Archer::getPixelPosition() const
{
    return CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(position_.x, position_.y));
}
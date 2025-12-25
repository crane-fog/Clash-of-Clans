#include "Barbarian.h"

#include "AudioEngine.h"
#include "TroopTargetManager.h"

// 升到level级所需资源花费
const std::array<int, MAX_TROOP_LEVEL + 1> Barbarian::kResearchCosts = {0, 0, 10000, 50000, 130000, 300000};

// 升到level级所需时间 单位：小时
const std::array<float, MAX_TROOP_LEVEL + 1> Barbarian::kResearchTimes = {0, 0, 0.5, 1, 2, 4};

// 升到level级所需实验室等级
const std::array<Troop::uchar, MAX_TROOP_LEVEL + 1> Barbarian::kLaboratoryLevelRequireds = {0, 1, 1, 3, 5, 6};

Barbarian::Barbarian(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, NONE, MELEE_SINGLE_GROUND, 1, 1, 2, 1, 0.4,
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 9, 12, 15, 18, 23}),
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 45, 54, 65, 85, 105}))
{}

Barbarian* Barbarian::create(BaseMap* base_map, int level, cocos2d::Vec2 position)
{
    Barbarian* p_ret = new (std::nothrow) Barbarian(base_map, level, position);
    if (p_ret && p_ret->initWithFile(kPicsBarbarian.at(level))) {
        p_ret->autorelease();
        return p_ret;
    }
    else {
        delete p_ret;
        p_ret = nullptr;
        return nullptr;
    }
}

bool Barbarian::initWithFile(const std::string& filename)
{
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    // base_map_->sprites_.push_back(this);
    return true;
}

void Barbarian::performAttack()
{
    // 执行近战攻击
    float damage = getCurrentDamage();
    current_target_->takeDamage(damage);

    // 播放攻击动画
    // 播放攻击音效
    int babarian_hit = cocos2d::AudioEngine::play2d("music/babarian_hit.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule(
        [babarian_hit, this](float dt) {
            if (cocos2d::AudioEngine::getState(babarian_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                // 停止音效播放并释放资源
                cocos2d::AudioEngine::uncache("music/babarian_hit.mp3");
                this->unschedule("stop_audio_key");  // 停止检查
            }
        },
        0.1f, "stop_audio_key");
}

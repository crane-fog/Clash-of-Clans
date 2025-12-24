#include "Giant.h"
#include "TroopTargetManager.h"
#include "AudioEngine.h"

//升到level级所需资源花费
const std::array<int, MAX_TROOP_LEVEL + 1> Giant::research_costs_ = { 0,0,40000,150000,400000,800000 };

//升到level级所需时间 单位：小时
const std::array<float, MAX_TROOP_LEVEL + 1> Giant::research_times_ = { 0,0,2,4,6,12 };

//升到level级所需实验室等级
const std::array<Troop::uchar, MAX_TROOP_LEVEL + 1> Giant::laboratory_level_requireds_ = { 0,0,2,4,5,6 };

Giant::Giant(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, DEFENSE, MELEE_SINGLE_GROUND, 5, 3, 1.5f, 2.0f, 1.0f,
        std::array<float, MAX_TROOP_LEVEL + 1>({0,24,30,40,48,62}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,400,500,600,700,900})
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
    int giant_hit = cocos2d::AudioEngine::play2d("music/babarian_hit.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule([giant_hit, this](float dt) {
        if (cocos2d::AudioEngine::getState(giant_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
            // 停止音效播放并释放资源
            cocos2d::AudioEngine::uncache("music/babarian_hit.mp3");
            this->unschedule("stop_audio_key"); // 停止检查
        }
        }, 0.1f, "stop_audio_key");
}

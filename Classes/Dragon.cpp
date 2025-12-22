#include "Dragon.h"
#include "TroopTargetManager.h"
#include "AudioEngine.h"
Dragon::Dragon(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, NONE, RANGED_AOE_AIR_GROUND, 20, 9, 2.0f, 1.25f,0.3f,
        std::array<float, MAX_TROOP_LEVEL + 1>({0,175,200,225,262.5,300}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,1900,2100,2300,2700,3100}),
        std::array<int, MAX_TROOP_LEVEL + 1>({0,0,1000000,2000000,3000000,3800000}),
        std::array<float, MAX_TROOP_LEVEL + 1>({0,0,18,36,72,84}),
        std::array<uchar, MAX_TROOP_LEVEL + 1>({0,1,5,6,7,8})
        )
{
}

Dragon* Dragon::create(BaseMap* base_map, int level, cocos2d::Vec2 position) {
    Dragon* pRet = new(std::nothrow) Dragon(base_map,level,position);
    if (pRet && pRet->initWithFile(pics_dragon.at(level))) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet; pRet = nullptr; return nullptr;
    }
}

bool Dragon::initWithFile(const std::string& filename) {
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    //base_map_->sprites_.push_back(this);
    return true;
}

void Dragon::performAttack() {
    // 执行范围攻击（可以攻击地面和空中目标）
    float damage = getCurrentDamage();
    current_target_->takeDamage(damage);
    // 播放攻击动画
    // 播放攻击音效
    int fire_hit = cocos2d::AudioEngine::play2d("music/fire_hit.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule([fire_hit, this](float dt) {
        if (cocos2d::AudioEngine::getState(fire_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
            // 停止音效播放并释放资源
            cocos2d::AudioEngine::uncache("music/fire_hit.mp3");
            this->unschedule("stop_audio_key"); // 停止检查
        }
        }, 0.1f, "stop_audio_key");
}

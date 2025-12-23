#include "WallBreaker.h"
#include "TroopTargetManager.h"
#include "AudioEngine.h"

WallBreaker::WallBreaker(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, WALLT, MELEE_AOE_GROUND, 2, 5, 3.0f, 100000.0f, 0.5f,
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

void WallBreaker::performAttack() {
    // 执行近战范围攻击
    
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
}

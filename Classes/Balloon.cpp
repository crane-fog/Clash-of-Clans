#include "Balloon.h"
#include "TroopTargetManager.h"
#include "AudioEngine.h"
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
    float damage = getCurrentDamage();
    float size;
    cocos2d::Vec2 center = current_target_->getCellPosition(size);
    std::vector<ITroopTarget*> targets = TroopTargetManager::getInstance()->getTargetsInRange(
		center,area_splash_radius_);
    for (auto target : targets) {
        target->takeDamage(damage);
	}
    // 播放攻击动画
    // 播放攻击音效
    int bomb_hit = cocos2d::AudioEngine::play2d("music/bomb_hit.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule([bomb_hit, this](float dt) {
        if (cocos2d::AudioEngine::getState(bomb_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
            // 停止音效播放并释放资源
            cocos2d::AudioEngine::uncache("music/bomb_hit.mp3");
            this->unschedule("stop_audio_key"); // 停止检查
        }
        }, 0.1f, "stop_audio_key");
}

void Balloon::onDeath() {
    // ====== 1. 视觉动画序列：弹跳 → 下落 → 缩小+淡出 ======
    auto jump = cocos2d::JumpBy::create(0.5f, cocos2d::Vec2(0, 0), 5.0f, 1);
    auto scaleUp = cocos2d::ScaleTo::create(0.25f, 1.1f);
    auto scaleDown = cocos2d::ScaleTo::create(0.25f, 1.0f);
    auto scalePulse = cocos2d::Sequence::create(scaleUp, scaleDown, nullptr);
    auto visualJump = cocos2d::Spawn::create(jump, scalePulse, nullptr);

    // 下落偏移（请替换为你实际的值）
    cocos2d::Vec2 fallOffset(0, -50);
    auto fall = cocos2d::MoveBy::create(0.3f, fallOffset);
    auto shrink = cocos2d::ScaleTo::create(0.3f, 0.01f);
    auto fadeOut = cocos2d::FadeTo::create(0.3f, 0);
    auto vanish = cocos2d::Spawn::create(shrink, fadeOut, nullptr);

    auto visualSequence = cocos2d::Sequence::create(visualJump,fall,vanish,nullptr);

    // ====== 2. 攻击逻辑：延迟后触发 AOE ======
    auto delay = cocos2d::DelayTime::create(death_damage_delay_);
    auto triggerAoe = cocos2d::CallFunc::create(CC_CALLBACK_0(Balloon::triggerDeathDamage, this));
    auto attackSequence = cocos2d::Sequence::create(delay, triggerAoe, nullptr);

    // ====== 同时启动两个动作 ======
    this->runAction(visualSequence);
    this->runAction(attackSequence);
}

void Balloon::triggerDeathDamage() {
    float damage = damages_upon_death_[level_];
    std::vector<ITroopTarget*> targets = TroopTargetManager::getInstance()->getTargetsInRange(
        getCellPosition(), death_damage_radius_);
    for (auto target : targets) {
        target->takeDamage(damage);
    }
}
#include "Balloon.h"

#include "AudioEngine.h"
#include "CocUtility.h"
#include "TroopTargetManager.h"

// 升到level级所需资源花费
const std::array<int, MAX_TROOP_LEVEL + 1> Balloon::kResearchCosts = {0, 0, 10000, 40000, 72000, 130000};

// 升到level级所需时间 单位：小时
const std::array<float, MAX_TROOP_LEVEL + 1> Balloon::kResearchTimes = {0, 0, 4, 6, 18, 24};

// 升到level级所需实验室等级
const std::array<Troop::uchar, MAX_TROOP_LEVEL + 1> Balloon::kLaboratoryLevelRequireds = {0, 1, 2, 4, 5, 6};

Balloon::Balloon(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, DEFENSE, RANGED_AOE_GROUND, 5, 6, 1.3f, 3.0f, 0.0f,
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 75, 96, 144, 216, 324}),
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 150, 180, 216, 280, 390}))
{
    attack_timer_ = kAttackSpeed - 0.75f;  // 初始延迟0.75秒攻击
}

Balloon* Balloon::create(BaseMap* base_map, int level, cocos2d::Vec2 position)
{
    Balloon* p_ret = new (std::nothrow) Balloon(base_map, level, position);
    if (p_ret && p_ret->initWithFile(kPicsBalloon.at(level))) {
        p_ret->autorelease();
        return p_ret;
    }
    else {
        delete p_ret;
        p_ret = nullptr;
        return nullptr;
    }
}

bool Balloon::initWithFile(const std::string& filename)
{
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    // base_map_->sprites_.push_back(this);
    return true;
}

void Balloon::performAttack()
{
    float damage = getCurrentDamage();
    float size;
    cocos2d::Vec2 center = current_target_->getCellPosition(size);
    std::vector<ITroopTarget*> targets =
        TroopTargetManager::getInstance()->getTargetsInRange(center, kAreaSplashRadius);
    for (auto target : targets) {
        target->takeDamage(damage);
    }
    // 播放攻击动画
    this->throwBomb();
    // 播放攻击音效
    int bomb_hit = cocos2d::AudioEngine::play2d("music/bomb_hit.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule(
        [bomb_hit, this](float dt) {
            if (cocos2d::AudioEngine::getState(bomb_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                // 停止音效播放并释放资源
                cocos2d::AudioEngine::uncache("music/bomb_hit.mp3");
                this->unschedule("stop_audio_key");  // 停止检查
            }
        },
        0.1f, "stop_audio_key");
}

void Balloon::onDeath()
{
    // ====== 1. 视觉动画序列：弹跳 → 下落 → 缩小+淡出 ======
    auto jump = cocos2d::JumpBy::create(0.5f, cocos2d::Vec2(0, 0), 5.0f, 1);
    auto scale_up = cocos2d::ScaleTo::create(0.25f, 1.1f);
    auto scale_down = cocos2d::ScaleTo::create(0.25f, 1.0f);
    auto scale_pulse = cocos2d::Sequence::create(scale_up, scale_down, nullptr);
    auto visual_jump = cocos2d::Spawn::create(jump, scale_pulse, nullptr);

    // 下落偏移（请替换为你实际的值）
    cocos2d::Vec2 fall_offset(0, -50);
    auto fall = cocos2d::MoveBy::create(0.3f, fall_offset);
    auto shrink = cocos2d::ScaleTo::create(0.3f, 0.01f);
    auto fade_out = cocos2d::FadeTo::create(0.3f, 0);
    auto vanish = cocos2d::Spawn::create(shrink, fade_out, nullptr);

    auto visual_sequence = cocos2d::Sequence::create(visual_jump, fall, vanish, nullptr);

    // ====== 2. 攻击逻辑：延迟后触发 AOE ======
    auto delay = cocos2d::DelayTime::create(kDeathDamageDelay);
    auto trigger_aoe = cocos2d::CallFunc::create(CC_CALLBACK_0(Balloon::triggerDeathDamage, this));
    auto attack_sequence = cocos2d::Sequence::create(delay, trigger_aoe, nullptr);

    // ====== 同时启动两个动作 ======
    this->runAction(visual_sequence);
    this->runAction(attack_sequence);
    changeStatus(DEAD);
    // 播放攻击音效
    int bomb_hit = cocos2d::AudioEngine::play2d("music/bomb_hit.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule(
        [bomb_hit, this](float dt) {
            if (cocos2d::AudioEngine::getState(bomb_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                // 停止音效播放并释放资源
                cocos2d::AudioEngine::uncache("music/bomb_hit.mp3");
                this->unschedule("stop_audio_key");  // 停止检查
            }
        },
        0.1f, "stop_audio_key");
}

void Balloon::triggerDeathDamage()
{
    float damage = kDamagesUponDeath[level_];
    std::vector<ITroopTarget*> targets =
        TroopTargetManager::getInstance()->getTargetsInRange(getCellPosition(), kDeathDamageRadius);
    for (auto target : targets) {
        target->takeDamage(damage);
    }
}

cocos2d::Vec2 Balloon::getPixelPosition() const
{
    cocos2d::Vec2 pixel_ground = CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(position_.x, position_.y));
    return cocos2d::Vec2(pixel_ground.x, pixel_ground.y + kPixelYOffset);
}

void Balloon::throwBomb()
{
    // === 创建炸弹 ===
    auto bomb = cocos2d::Sprite::create("troop/bomb.png");
    if (!bomb) return;

    bomb->setScale(0.75f);
    float center_x = this->getContentSize().width * 0.5f;
    bomb->setPosition(cocos2d::Vec2(center_x, 0));
    this->addChild(bomb, 10);

    // === 垂直下落：直接移动到目标位置 ===
    float distance = kPixelYOffset;
    float fall_speed = 300.0f;                  // 像素/秒
    float duration = (distance > 0) ? distance / fall_speed : 0.5f;
    duration = std::max(0.1f, std::min(duration, 2.0f));  // 限制合理范围

    auto move_action = cocos2d::MoveTo::create(duration, cocos2d::Vec2(center_x, -distance));
    auto cleanup = cocos2d::RemoveSelf::create();
    auto on_arrived = cocos2d::CallFunc::create([=]() {
        // === 播放黑烟 ===
        auto smoke = cocos2d::Sprite::create("troop/smoke.png");
        if (!smoke) return;

        smoke->setColor(cocos2d::Color3B(100, 100, 100));
        smoke->setPosition(cocos2d::Vec2(center_x, -distance));
        smoke->setScale(2.0f);
        smoke->setOpacity(255);
        this->addChild(smoke, 10);

        auto scale_action = cocos2d::ScaleTo::create(0.6f, 1.2f);
        auto fade_action = cocos2d::FadeOut::create(1.0f);
        auto cleanup_action = cocos2d::RemoveSelf::create();
        smoke->runAction(cocos2d::Sequence::create(cocos2d::Spawn::create(scale_action, fade_action, nullptr),
                                                   cleanup_action, nullptr));
    });
    bomb->runAction(cocos2d::Sequence::create(move_action, cleanup, on_arrived, nullptr));
}
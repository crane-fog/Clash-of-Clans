#include "Dragon.h"

#include "AudioEngine.h"
#include "TroopTargetManager.h"
#include "CocUtility.h"

// 升到level级所需资源花费
const std::array<int, MAX_TROOP_LEVEL + 1> Dragon::kResearchCosts = {0, 0, 1000000, 2000000, 3000000, 3800000};

// 升到level级所需时间 单位：小时
const std::array<float, MAX_TROOP_LEVEL + 1> Dragon::kResearchTimes = {0, 0, 18, 36, 72, 84};

// 升到level级所需实验室等级
const std::array<Troop::uchar, MAX_TROOP_LEVEL + 1> Dragon::kLaboratoryLevelRequireds = {0, 1, 5, 6, 7, 8};

Dragon::Dragon(BaseMap* base_map, int level, cocos2d::Vec2 position)
    : Troop(base_map, level, position, NONE, RANGED_AOE_AIR_GROUND, 20, 9, 2.0f, 1.25f, 0.3f,
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 175, 200, 225, 262.5, 300}),
            std::array<float, MAX_TROOP_LEVEL + 1>({0, 1900, 2100, 2300, 2700, 3100}))
{}

Dragon* Dragon::create(BaseMap* base_map, int level, cocos2d::Vec2 position)
{
    Dragon* p_ret = new (std::nothrow) Dragon(base_map, level, position);
    if (p_ret && p_ret->initWithFile(kPicsDragon.at(level))) {
        p_ret->autorelease();
        return p_ret;
    }
    else {
        delete p_ret;
        p_ret = nullptr;
        return nullptr;
    }
}

bool Dragon::initWithFile(const std::string& filename)
{
    if (!Troop::initWithFile(filename)) {
        return false;
    }
    // base_map_->sprites_.push_back(this);
    return true;
}

void Dragon::performAttack()
{
    // 执行范围攻击（可以攻击地面和空中目标）
    float damage = getCurrentDamage();
    float size;
    cocos2d::Vec2 center = current_target_->getCellPosition(size);
    std::vector<ITroopTarget*> targets =
        TroopTargetManager::getInstance()->getTargetsInRange(center, kAreaSplashRadius);
    for (auto target : targets) {
        target->takeDamage(damage);
    }
    // 播放攻击动画
    // 播放攻击音效
    int fire_hit = cocos2d::AudioEngine::play2d("music/fire_hit.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule(
        [fire_hit, this](float dt) {
            if (cocos2d::AudioEngine::getState(fire_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                // 停止音效播放并释放资源
                cocos2d::AudioEngine::uncache("music/fire_hit.mp3");
                this->unschedule("stop_audio_key");  // 停止检查
            }
        },
        0.1f, "stop_audio_key");
}

void Dragon::onDeath()
{
    // 获取目标像素位置（提前计算好，避免在 lambda 中访问可能失效的成员）
    cocos2d::Vec2 target_pos = CoordAdaptor::cellToPixel(base_map_, getCellPosition());

    // 淡出动作
    auto fade_out = cocos2d::FadeOut::create(0.5f);

    // 在淡出结束后，一次性设置位置、纹理和缩放
    auto setup_tomb = cocos2d::CallFunc::create([this, target_pos]() {
        this->setPosition(target_pos);
        this->setTexture("troop/tomb.png");
        this->setScale(0.6f);
        this->setOpacity(255);  // 确保墓碑完全可见
    });

    // 执行序列：先淡出，再瞬间切换为墓碑（位置+图+缩放）
    this->runAction(cocos2d::Sequence::create(fade_out, setup_tomb, nullptr));
    changeStatus(DEAD);
}

cocos2d::Vec2 Dragon::getPixelPosition() const
{
    cocos2d::Vec2 pixel_ground = CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(position_.x, position_.y));
    return cocos2d::Vec2(pixel_ground.x, pixel_ground.y + 10.0f);
}
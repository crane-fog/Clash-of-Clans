#include "HealthBar.h"

USING_NS_CC;

HealthBar* HealthBar::create(float max_hp, float max_width, bool is_troop)
{
    HealthBar* p_ret = new (std::nothrow) HealthBar();
    if (p_ret && p_ret->init(max_hp, max_width, is_troop)) {
        p_ret->autorelease();
        return p_ret;
    }
    else {
        delete p_ret;
        p_ret = nullptr;
        return nullptr;
    }
}

bool HealthBar::init(float max_hp, float max_width, bool is_troop)
{
    if (!Sprite::init()) {
        return false;
    }
    max_width_ = max_width;
    max_health_ = max_hp;
    current_health_ = max_hp;
    is_troop_ = is_troop;
    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->setPosition(Vec2(0, 0));
    this->setContentSize(Size(max_width_, kMaxHeight));

    // 创建背景（黑色半透明）
    bar_bg_ = ui::Scale9Sprite::create("LoadingBarFile.png");

    bar_bg_->setContentSize(Size(max_width_, kMaxHeight));
    bar_bg_->setColor(Color3B(0, 0, 0));  // 黑色
    bar_bg_->setOpacity(128);             // 半透明
    bar_bg_->setAnchorPoint(Vec2(0.5f, 0.5f));
    bar_bg_->setPosition(Vec2(max_width_ / 2, kMaxHeight / 2));  // 相对于 HealthBar 自身
    this->addChild(bar_bg_, 10);                                 // zOrder 较高确保在上层

    // 创建前景（从左到右填充，颜色根据血量变化）
    bar_fg_ = ui::Scale9Sprite::create("LoadingBarFile.png");
    bar_fg_->setContentSize(Size(max_width_ - 2 * kPadding, kMaxHeight - 2 * kPadding));
    if (is_troop_)
        bar_fg_->setColor(Color3B::GREEN);  // 士兵血条初始为绿色
    else
        bar_fg_->setColor(Color3B(216, 73, 197));             // 建筑血条初始为紫色
    bar_fg_->setAnchorPoint(Vec2(0.0f, 0.5f));                // 左中对齐
    bar_fg_->setPosition(Vec2(kPadding, kMaxHeight / 2.0f));  // 相对于 HealthBar 自身
    this->addChild(bar_fg_, 11);

    // 初始隐藏
    this->setVisible(false);

    return true;
}

HealthBar::~HealthBar()
{
    // 取消可能存在的定时器
    this->unschedule(CC_SCHEDULE_SELECTOR(HealthBar::hideBar));
}

void HealthBar::takeDamage(float damage)
{
    float old_hp = current_health_;
    current_health_ -= damage;
    if (current_health_ < 0) current_health_ = 0;

    // 只有血量真正变化才更新
    if (old_hp != current_health_) {
        // 显示血条
        this->setVisible(true);

        updateDisplay();
        resetHideTimer();  // 重置隐藏计时器
    }
}

void HealthBar::setMaxHealth(float max_hp)
{
    max_health_ = max_hp;
    updateDisplay();
}

void HealthBar::heal(float amount)
{
    current_health_ += amount;
    if (current_health_ > max_health_) {
        current_health_ = max_health_;
    }

    // 显示血条
    this->setVisible(true);

    updateDisplay();
    resetHideTimer();
}

void HealthBar::updateDisplay()
{
    float ratio = current_health_ / max_health_;

    if (is_troop_) {
        // 根据血量变色：高血量绿色，低血量红色
        if (ratio > 0.6f) {
            bar_fg_->setColor(Color3B::GREEN);
        }
        else if (ratio > 0.3f) {
            bar_fg_->setColor(Color3B(255, 165, 0));  // 橙色过渡
        }
        else {
            bar_fg_->setColor(Color3B::RED);
        }
    }

    float fg_height = kMaxHeight - 2 * kPadding;           // 高度也缩小
    float fg_width = (max_width_ - 2 * kPadding) * ratio;  // 宽度基于可填充区域

    // 设置前景尺寸（比背景小一圈）
    bar_fg_->setContentSize(Size(fg_width, fg_height));
}

void HealthBar::resetHideTimer()
{
    // 先取消之前的调度（避免多个定时器）
    this->unschedule(CC_SCHEDULE_SELECTOR(HealthBar::hideBar));
    // 重新调度
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(HealthBar::hideBar), kScheduleInterval);
}

void HealthBar::hideBar(float dt) { this->setVisible(false); }

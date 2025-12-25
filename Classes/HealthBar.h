#ifndef __HEALTH_BAR_H__
#define __HEALTH_BAR_H__

#include "ui/CocosGUI.h"

class HealthBar : public cocos2d::Sprite {
private:
    const float kPadding = 2.0f;            // 前景与背景的内边距（单边）
    const float kMaxHeight = 10.0f;        // 血条高度
    const float kScheduleInterval = 5.0f;  // 几秒后隐藏血条
    float max_health_ = 0;
    float current_health_ = 0;
    float max_width_ = 50.0f;
    bool is_troop_ = true;  // 默认是士兵血条，要改为false表示建筑血条，颜色会不同
    cocos2d::ui::Scale9Sprite* bar_bg_ = nullptr;
    cocos2d::ui::Scale9Sprite* bar_fg_ = nullptr;

public:
    static HealthBar* create(float max_hp, float max_width = 50.0f, bool is_troop = true);

    // 初始化血条
    virtual bool init(float max_hp, float max_width, bool is_troop);

    // 析构函数：确保清理定时器
    ~HealthBar();

    // 受到伤害时调用
    void takeDamage(float damage);

    // 设置最大生命值（可选）
    void setMaxHealth(float max_hp);

    // 获取当前血量
    float getCurrentHealth() const { return current_health_; }

    // 获取最大生命值
    float getMaxHealth() const { return max_health_; }

    // 恢复血量
    void heal(float amount);

    // 设置血条可见性
    void setHealthBarVisible(bool visible) { this->setVisible(visible); }

    // 设置血条位置（相对于父节点）
    void setHealthBarPosition(const cocos2d::Vec2& position) { this->setPosition(position); }

private:
    void updateDisplay();
    void resetHideTimer();
    void hideBar(float dt);
};

#endif  // __HEALTH_BAR_H__
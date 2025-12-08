#ifndef __HEALTH_BAR_H__
#define __HEALTH_BAR_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class HealthBar : public cocos2d::Sprite {
private:
    const float padding_=2.0f; // 前景与背景的内边距（单边）
	const float max_height_ = 10.0f; // 血条高度
	const float schedule_interval_ = 5.0f; // 几秒后隐藏血条
    float max_health_=0;
    float current_health_=0;
    float max_width_= 50.0f;
	bool is_troop_ = true;//默认是士兵血条，要改为false表示建筑血条，颜色会不同
    cocos2d::ui::Scale9Sprite* bar_bg_ = nullptr;
    cocos2d::ui::Scale9Sprite* bar_fg_ = nullptr;

public:
    static HealthBar* create(float max_hp, float max_width = 50.0f,bool is_troop = true) {
        HealthBar* pRet = new(std::nothrow) HealthBar();
        if (pRet && pRet->init(max_hp, max_width,is_troop)) {
            pRet->autorelease();
            return pRet;
        }
        else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    // 初始化血条
    bool init(float max_hp, float max_width, bool is_troop) {
        if (!Sprite::init()) {
            return false;
        }
        max_width_ = max_width;
        max_health_ = max_hp;
        current_health_ = max_hp;
        is_troop_ = is_troop;
        this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
        this->setPosition(cocos2d::Vec2(0, 0));
        this->setContentSize(cocos2d::Size(max_width_, max_height_));

        // 创建背景（黑色半透明）
        bar_bg_ = cocos2d::ui::Scale9Sprite::create("LoadingBarFile.png");

        bar_bg_->setContentSize(cocos2d::Size(max_width_, max_height_));
        bar_bg_->setColor(cocos2d::Color3B(0, 0, 0)); // 黑色
        bar_bg_->setOpacity(128); // 半透明
        bar_bg_->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
        bar_bg_->setPosition(cocos2d::Vec2(max_width_/2, max_height_/2)); // 相对于 HealthBar 自身
        this->addChild(bar_bg_, 10); // zOrder 较高确保在上层

        // 创建前景（从左到右填充，颜色根据血量变化）
        bar_fg_ = cocos2d::ui::Scale9Sprite::create("LoadingBarFile.png");
        bar_fg_->setContentSize(cocos2d::Size(max_width_ - 2 * padding_, max_height_ -2*padding_));
        if(is_troop_) bar_fg_->setColor(cocos2d::Color3B::GREEN); // 士兵血条初始为绿色
		else bar_fg_->setColor(cocos2d::Color3B(216, 73, 197));//建筑血条初始为紫色
		bar_fg_->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));// 左中对齐
        bar_fg_->setPosition(cocos2d::Vec2(padding_, max_height_ / 2.0f)); // 相对于 HealthBar 自身
        this->addChild(bar_fg_, 11);

        // 初始隐藏
        this->setVisible(false);
        //// 调试框：在 HealthBar::init() 中加一个红色矩形，表示背景区域
        //auto debug = cocos2d::DrawNode::create();
        //debug->drawRect(
        //    cocos2d::Vec2(-max_width_ / 2, -7.5f),
        //    cocos2d::Vec2(max_width_ / 2, 7.5f),
        //    cocos2d::Color4F::RED
        //);
        //this->addChild(debug, 100);


        return true;
    }

    // 析构函数：确保清理定时器
    ~HealthBar() {
        // 取消可能存在的定时器
        this->unschedule(CC_SCHEDULE_SELECTOR(HealthBar::hideBar));
    }

    // 受到伤害时调用
    void takeDamage(float damage) {
        float old_hp = current_health_;
        current_health_ -= damage;
        if (current_health_ < 0) current_health_ = 0;

        // 只有血量真正变化才更新
        if (old_hp != current_health_) {
            // 显示血条
            this->setVisible(true);

            updateDisplay();
            resetHideTimer(); // 重置隐藏计时器
        }
    }

    // 设置最大生命值（可选）
    void setMaxHealth(float max_hp) {
        max_health_ = max_hp;
        updateDisplay();
    }

    // 获取当前血量
    float getCurrentHealth() const { return current_health_; }

    // 获取最大生命值
    float getMaxHealth() const { return max_health_; }

    // 恢复血量
    void heal(float amount) {
        current_health_ += amount;
        if (current_health_ > max_health_) {
            current_health_ = max_health_;
        }

        // 显示血条
        this->setVisible(true);

        updateDisplay();
        resetHideTimer();
    }

    // 设置血条可见性
    void setHealthBarVisible(bool visible) {
        this->setVisible(visible);
    }

    // 设置血条位置（相对于父节点）
    void setHealthBarPosition(const cocos2d::Vec2& position) {
        this->setPosition(position);
    }

private:
    void updateDisplay() {
        float ratio = current_health_ / max_health_;

        if (is_troop_) {
            // 根据血量变色：高血量绿色，低血量红色
            if (ratio > 0.6f) {
                bar_fg_->setColor(cocos2d::Color3B::GREEN);
            }
            else if (ratio > 0.3f) {
                bar_fg_->setColor(cocos2d::Color3B(255, 165, 0)); // 橙色过渡
            }
            else {
                bar_fg_->setColor(cocos2d::Color3B::RED);
            }
        }

        float fg_height = max_height_ - 2 * padding_; // 高度也缩小
        float fg_width = (max_width_ - 2 * padding_) * ratio; // 宽度基于可填充区域

        // 设置前景尺寸（比背景小一圈）
        bar_fg_->setContentSize(cocos2d::Size(fg_width, fg_height));
    }

    void resetHideTimer() {
        // 先取消之前的调度（避免多个定时器）
        this->unschedule(CC_SCHEDULE_SELECTOR(HealthBar::hideBar));
        // 重新调度
        this->scheduleOnce(CC_SCHEDULE_SELECTOR(HealthBar::hideBar), schedule_interval_);
    }

    void hideBar(float dt) {
        this->setVisible(false);
    }
};

#endif // __HEALTH_BAR_H__
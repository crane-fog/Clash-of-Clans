#include "UIparts.h"
#include "cocos/ui/CocosGUI.h"
#include"MainVillageScene.h"
#include"DataHelper.h"
USING_NS_CC;
using namespace ui;

bool UIBars::init()
{

    if (!Node::init())
    {
        return false;
    }

    // 获取屏幕尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    unsigned long long gold = GameManager::getInstance()->getGold();
    unsigned long long elixir = GameManager::getInstance()->getElixir();
    unsigned long long maxGold = GameManager::getInstance()->getMaxGold();
    unsigned long long maxElixir = GameManager::getInstance()->getMaxElixir();

    // 创建多个进度条:金币和圣水
    createProgressBarWithBackground("金币", Color3B::YELLOW, "Gold.png", gold, visibleSize.width - 500, visibleSize.height - 50, maxGold);
    createProgressBarWithBackground("圣水", Color3B(128, 0, 158), "Elixir.png", elixir, visibleSize.width - 500, visibleSize.height - 150, maxElixir);


    // 创建返回按钮 - 固定在左上角
    auto backButton = ui::Button::create("BackButton.png");
    backButton->setPosition(Vec2(80, visibleSize.height - 80));
    backButton->addClickEventListener([&](Ref* sender) {
        // 按钮点击事件
        });
    this->addChild(backButton);

    // 注册金币更新事件监听
    goldUpdateListener = cocos2d::EventListenerCustom::create("update_gold_event", CC_CALLBACK_1(UIBars::onGoldUpdated, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(goldUpdateListener, this);

    // 注册圣水更新事件监听
    elixirUpdateListener = cocos2d::EventListenerCustom::create("update_elixir_event", CC_CALLBACK_1(UIBars::onElixirUpdated, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(elixirUpdateListener, this);

    // 注册最大金币更新事件监听
    maxGoldUpdateListener = cocos2d::EventListenerCustom::create("update_max_gold_event", CC_CALLBACK_1(UIBars::onMaxGoldUpdated, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(maxGoldUpdateListener, this);

    // 注册最大圣水更新事件监听
    maxElixirUpdateListener = cocos2d::EventListenerCustom::create("update_max_elixir_event", CC_CALLBACK_1(UIBars::onMaxElixirUpdated, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(maxElixirUpdateListener, this);

    return true;
}
void UIBars::createProgressBarWithBackground(const std::string& title, const cocos2d::Color3B& barColor, const std::string& iconPath, unsigned long long nowAmount, float x, float y, unsigned long long UpperLimit)
{
    ProgressBarData data;
    data.title = title;
    float percent = 0;
    if (UpperLimit > 0) {
        percent = (float)nowAmount * 100.0f / UpperLimit;
    }
    // 创建图像图标 
    data.icon = Sprite::create(iconPath); // 图标图片
    if (data.icon) {
        data.icon->setPosition(Vec2(x + 450, y)); // 滑动条右边
        data.icon->setScale(1.0f); // 调整图标大小
        this->addChild(data.icon);
    }
    // 创建标题
    auto titleLabel = Label::createWithSystemFont(title, "Arial", 45);
    titleLabel->setPosition(Vec2(x - 220, y));
    titleLabel->setTextColor(Color4B::WHITE);
    titleLabel->setAnchorPoint(Vec2(0, 0.5));
    this->addChild(titleLabel);

    // 创建背景框
    auto background = LayerColor::create(Color4B(255, 255, 255, 150), 500, 40); // 黑色半透明
    background->setPosition(Vec2(x - 100, y - 24.0f)); // 设置位置
    this->addChild(background, 0);
    data.background = nullptr; // 由于使用LayerColor，这里设为null

    // 创建进度条
    data.loadingBar = cocos2d::ui::LoadingBar::create("LoadingBarFile.png");
    if (data.loadingBar) {
        data.loadingBar->setDirection(cocos2d::ui::LoadingBar::Direction::RIGHT);
        data.loadingBar->setPercent(percent);
        data.loadingBar->setPosition(Vec2(x + 150, y - 4));
        data.loadingBar->setScaleX(2.4f);
        data.loadingBar->setScaleY(2.0f);
        data.loadingBar->setColor(barColor);
        this->addChild(data.loadingBar, 1);
    }

    // 创建数量标签
    data.percentLabel = Label::createWithSystemFont(StringUtils::format("%llu / %llu", nowAmount, UpperLimit), "Arial", 30);
    data.percentLabel->setPosition(Vec2(x + 150, y - 5));
    data.percentLabel->setTextColor(Color4B::BLACK);
    this->addChild(data.percentLabel, 2);

    // 保存到容器
    progressBars_.push_back(data);
}


void UIBars::updateProgressBar(const std::string& title, unsigned long long nowAmount, unsigned long long maxAmount)
{
    for (auto& data : progressBars_) {
        if (data.title == title) {
            if (data.loadingBar && data.percentLabel) {
                float percent = 0;
                if (maxAmount > 0) {
                    percent = (float)nowAmount * 100.0f / maxAmount;
                }
                if (percent > 100.0f) percent = 100.0f;
                data.loadingBar->setPercent(percent);
                data.percentLabel->setString(StringUtils::format("%llu / %llu", nowAmount, maxAmount));
            }
            break;
        }
    }
}



void UIBars::onGoldUpdated(cocos2d::EventCustom* event) {
    unsigned long long gold = *static_cast<unsigned long long*>(event->getUserData());
    unsigned long long maxGold = GameManager::getInstance()->getMaxGold();
    updateProgressBar("金币", gold, maxGold);
}
void UIBars::onElixirUpdated(cocos2d::EventCustom* event) {
    unsigned long long  elixir = *static_cast<unsigned long long*>(event->getUserData());
    unsigned long long maxElixir = GameManager::getInstance()->getMaxElixir();
    updateProgressBar("圣水", elixir, maxElixir);
}

void UIBars::onMaxGoldUpdated(cocos2d::EventCustom* event) {
    unsigned long long maxGold = *static_cast<unsigned long long*>(event->getUserData());
    unsigned long long gold = GameManager::getInstance()->getGold();
    updateProgressBar("金币", gold, maxGold);
}

void UIBars::onMaxElixirUpdated(cocos2d::EventCustom* event) {
    unsigned long long maxElixir = *static_cast<unsigned long long*>(event->getUserData());
    unsigned long long elixir = GameManager::getInstance()->getElixir();
    updateProgressBar("圣水", elixir, maxElixir);
}

//倒计时类相关
void CountdownTimer::start(unsigned int seconds,
    std::function<void(int)> onTick,
    std::function<void()> onComplete) {
    remainingTime_ = seconds;
    totalTime_ = seconds;
    onTick_ = onTick;
    onComplete_ = onComplete;
    isRunning_ = true;

    schedule([this](float dt) {
        this->updateTimer(dt);
        }, 1.0f, CC_REPEAT_FOREVER, 0, "TimerUpdate");
}

void CountdownTimer::updateTimer(float dt) {
    if (!isRunning_) return;

    if (remainingTime_ > 0) {
        remainingTime_--;

        // 回调通知
        if (onTick_) {
            onTick_(remainingTime_);
        }

        if (remainingTime_ == 0) {
            unschedule("TimerUpdate");
            isRunning_ = false;

            if (onComplete_) {
                onComplete_();
            }
        }
    }
}


bool AttackStars::init() {

    if (!Node::init()) {
        return false;
    }
    int x_ = 200;
    int y_ = 20;
    // 获取屏幕尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto prog_label = Label::createWithSystemFont(StringUtils::format("摧毁进度：%llu  %%", progress_), "Arial", 30);
    prog_label->setPosition(Vec2(230, visibleSize.height - 30));
    this->addChild(prog_label);
    // 创建背景框
    auto background = LayerColor::create(Color4B(0, 0,0, 150), 400, 40); // 黑色半透明
    background->setPosition(Vec2(30, visibleSize.height-100)); // 设置位置
    this->addChild(background, 0);

    // 创建进度条
    progressBar_ = cocos2d::ui::LoadingBar::create("LoadingBarFile.png");
    progressBar_->setScaleX(2.3f);
    progressBar_->setScaleY(3.0f);
    progressBar_->setPosition(cocos2d::Vec2(30+x_, visibleSize.height - 100+y_));
    progressBar_->setPercent(0); // 初始为0%
    
    this->addChild(progressBar_);

    // 创建星星精灵并将其放入 vector
    for (int i = 0; i < 3; ++i) {
        auto star = createStar();
        star->setScale(0.1f);
        star->setPosition(cocos2d::Vec2(130 + i *100, visibleSize.height - 150)); // 星星的位置
        stars_.push_back(star); // 添加星星到 vector
        this->addChild(star);
        setStarColor(star, false); // 初始化星星颜色为灰色
    }


    return true;

}
void AttackStars::updateStars() {
    // 更新星星状态
    for (int i = 0; i < stars_.size();) {
        // 根据进度或其他条件点亮星星
        if (progress_ == 50.0f) {  // 比如每50%点亮一颗星
            setStarColor(stars_[i], true);
            // 弹窗显示星星飞行动画
            showPopup(stars_[i], progress_);
            i++;
        }
        else if (progress_ == 100.0f) {
            setStarColor(stars_[i], true);
            // 弹窗显示星星飞行动画
            //showPopup(stars_[i], progress_);
            i++;
        }
        else {
            setStarColor(stars_[i], false);
        }
        // 检查第三颗星星的条件（例如进度达到75%）
        if (someOtherConditionMet()) {
            setStarColor(stars_[i], true); // 第三颗星满足其他条件时点亮
            // 弹窗显示星星飞行动画
           // showPopup(stars_[i], progress_);
            i++;
        }
    }


}

void AttackStars::showPopup(cocos2d::Sprite* targetStar, float progress) {
    // 获取屏幕尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建飞行星星精灵
    auto flyingStar = createStar();
    flyingStar->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2));  // 从屏幕中央起飞
    flyingStar->setScale(1.0f); // 初始大小设置为1
    this->addChild(flyingStar);

    // 创建提示文本
    auto progressText = cocos2d::Label::createWithSystemFont(
        StringUtils::format("摧毁进度达到：%.0f %%", progress), "Arial", 30);

    progressText->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2 - 200)); // 设置文字显示位置
    this->addChild(progressText);

    // 飞行动画：飞到目标位置并添加旋转和缩放
    auto moveTo = cocos2d::MoveTo::create(1.0f, targetStar->getPosition());  // 飞到目标星星位置
    auto rotateBy = cocos2d::RotateBy::create(1.0f, 360);  // 缓慢旋转360度
    auto scaleTo = cocos2d::ScaleTo::create(1.0f, 0.1f);  // 到达目标位置时缩小到0.1倍大小
    auto fadeOut = cocos2d::FadeOut::create(0.5f);  // 飞到后淡出

    // 创建旋转和缩放的组合动作
    auto rotateAndScale = cocos2d::Spawn::create(rotateBy, scaleTo, nullptr);

    // 创建飞行动作和淡出
    auto spawn = cocos2d::Spawn::create(moveTo, rotateAndScale, fadeOut, nullptr);

    // 创建文字淡出动画
    auto fadeOutText = cocos2d::FadeOut::create(0.5f);

    // 在动画结束后移除飞行星星和提示文本
    auto cleanup = cocos2d::CallFunc::create([flyingStar, progressText] {
        flyingStar->removeFromParent();
        progressText->removeFromParent();
        });

    // 组合动画：星星的飞行、旋转、缩放和淡出
    flyingStar->runAction(cocos2d::Sequence::create(spawn, cleanup, nullptr));
    progressText->runAction(fadeOutText);

}
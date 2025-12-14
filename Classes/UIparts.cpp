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

    unsigned long long gold = 100;
    unsigned long long elixir = 0;
    bool success = DataHelper::readSourceData("data/SourceData.dat", gold, elixir);
    GameManager::getInstance()->setGold(gold);
    GameManager::getInstance()->setElixir(elixir);
    if (success) {
        CCLOG("金币储量: %llu", gold);
        CCLOG("圣水储量: %llu", elixir);
    }
    else {
        CCLOG("读取文件失败！");
    }

    // 创建多个进度条:金币和圣水
    createProgressBarWithBackground("金币", Color3B::YELLOW, "Gold.png", gold, visibleSize.width - 500, visibleSize.height - 50, GoldLimit);
    createProgressBarWithBackground("圣水", Color3B(128, 0, 158), "Elixir.png", elixir, visibleSize.width - 500, visibleSize.height - 150, ElixirLimit);


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

    return true;
}
void UIBars::createProgressBarWithBackground(const std::string& title, const cocos2d::Color3B& barColor, const std::string& iconPath, unsigned long long nowAmount, float x, float y, unsigned long long UpperLimit)
{
    ProgressBarData data;
    data.title = title;
    float percent = nowAmount*100/UpperLimit;
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
    data.percentLabel = Label::createWithSystemFont(StringUtils::format(" %llu %", nowAmount), "Arial", 30);
    data.percentLabel->setPosition(Vec2(x + 150, y - 5));
    data.percentLabel->setTextColor(Color4B::BLACK);
    this->addChild(data.percentLabel, 2);

    // 保存到容器
    progressBars_.push_back(data);
}


void UIBars::updateProgressBar(const std::string& title, unsigned long long nowAmount)
{
    for (auto& data : progressBars_) {
        if (data.title == title) {
            if (data.loadingBar && data.percentLabel) {
                data.loadingBar->setPercent(nowAmount*100/GoldLimit);
                data.percentLabel->setString(StringUtils::format("%llu %", nowAmount));
            }
            break;
        }
    }
}



void UIBars::onGoldUpdated(cocos2d::EventCustom* event) {
    unsigned long long gold = *static_cast<unsigned long long*>(event->getUserData());
    updateProgressBar("金币", gold);
}
void UIBars::onElixirUpdated(cocos2d::EventCustom* event) {
    unsigned long long  elixir = *static_cast<unsigned long long*>(event->getUserData());
    updateProgressBar("圣水", elixir);
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

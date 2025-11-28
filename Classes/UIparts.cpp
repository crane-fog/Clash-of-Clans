#include "UIparts.h"
#include "cocos/ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

bool UI::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // 获取屏幕尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建多个进度条:金币和圣水
    createProgressBarWithBackground("Gold", Color3B::YELLOW, "Gold.png", 50.0f, visibleSize.width - 500, visibleSize.height - 100, 100);
    createProgressBarWithBackground("Elixir", Color3B(128, 0, 158), "Elixir.png", 100.0f, visibleSize.width - 500, visibleSize.height - 200, 100);


    // 创建返回按钮 - 固定在左上角
    auto backButton = ui::Button::create("BackButton.png");
    backButton->setPosition(Vec2(80, visibleSize.height - 80));
    backButton->addClickEventListener([&](Ref* sender) {
        // 按钮点击事件
        });
    this->addChild(backButton);

    auto button = Button::create("shop.png", "shopSelected.png", "shopDisabled.png");

    button->setTitleText("SHOP");

    button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
        switch (type)
        {
            case ui::Widget::TouchEventType::BEGAN:
                break;
            case ui::Widget::TouchEventType::ENDED:
                break;
            default:
                break;
        }
        });

    this->addChild(button);
    return true;
}
void UI::createProgressBarWithBackground(const std::string& title, const cocos2d::Color3B& barColor, const std::string& iconPath, float percent, float x, float y, int UpperLimit)
{
    ProgressBarData data;
    data.title = title;

    // 创建图像图标 
    data.icon = Sprite::create(iconPath); // 你的图标图片
    if (data.icon) {
        data.icon->setPosition(Vec2(x + 450, y)); // 滑动条右边
        data.icon->setScale(1.0f); // 调整图标大小
        this->addChild(data.icon);
    }
    // 创建标题
    auto titleLabel = Label::createWithSystemFont(title, "Arial", 45);
    titleLabel->setPosition(Vec2(x - 200, y));
    titleLabel->setTextColor(Color4B::WHITE);
    titleLabel->setAnchorPoint(Vec2(0, 0.5));
    this->addChild(titleLabel);

    // 创建背景框
    auto background = LayerColor::create(Color4B(0, 0, 0, 150), 500, 40); // 黑色半透明
    background->setPosition(Vec2(x - 100, y - 24.0f)); // 设置位置
    this->addChild(background, 0);
    data.background = nullptr; // 由于使用LayerColor，这里设为null

    // 创建进度条
    data.loadingBar = cocos2d::ui::LoadingBar::create("GoldLoadingBarFile.png");
    if (data.loadingBar) {
        data.loadingBar->setDirection(cocos2d::ui::LoadingBar::Direction::RIGHT);
        data.loadingBar->setPercent(percent);
        data.loadingBar->setPosition(Vec2(x + 150, y - 4));
        data.loadingBar->setScaleX(2.6f);
        data.loadingBar->setScaleY(2.0f);
        data.loadingBar->setColor(barColor);
        this->addChild(data.loadingBar, 1);
    }

    // 创建百分比标签
    data.percentLabel = Label::createWithSystemFont(StringUtils::format("%f%", percent * UpperLimit), "Arial", 30);
    data.percentLabel->setPosition(Vec2(x + 150, y - 5));
    data.percentLabel->setTextColor(Color4B::BLACK);
    this->addChild(data.percentLabel, 2);

    // 保存到容器
    progressBars_.push_back(data);
}

void UI::updateProgressBar(int index, float percent)
{
    if (index >= 0 && index < progressBars_.size()) {
        auto& data = progressBars_[index];
        if (data.loadingBar && data.percentLabel) {
            data.loadingBar->setPercent(percent);
            data.percentLabel->setString(StringUtils::format("%.0f%%", percent));
        }
    }
}

void UI::updateProgressBar(const std::string& title, float percent)
{
    for (auto& data : progressBars_) {
        if (data.title == title) {
            if (data.loadingBar && data.percentLabel) {
                data.loadingBar->setPercent(percent);
                data.percentLabel->setString(StringUtils::format("%.0f%%", percent));
            }
            break;
        }
    }
}
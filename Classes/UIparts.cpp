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

void UICommonHelper::showChallengeSelectionPanel(cocos2d::Node* parent)
{
    // 播放音效
    auto select_bgm = AudioEngine::play2d("music/choosing_battle.mp3", true);
    
    // 创建一个覆盖全屏的面板
    auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
    auto panel = cocos2d::LayerColor::create(cocos2d::Color4B(130, 130, 190, 255));  // 黑色背景
    parent->addChild(panel, 99999);
    
    // 将 helper 实例添加到 panel 中，确保其生命周期跟随 panel
    panel->addChild(this);
    
    // 重置选择状态
    can_confirm_ = -1;
    selected_item_bg_ = nullptr;

    // 面板标题
    auto title_label = cocos2d::Label::createWithSystemFont("选择挑战场景", "Arial", 56);
    title_label->setPosition(cocos2d::Vec2(visible_size.width / 2, visible_size.height - 50));
    panel->addChild(title_label, 1);

    // 确认按钮
    auto confirm_button = cocos2d::ui::Button::create("attack_scene/yes.png");
    confirm_button->setPosition(cocos2d::Vec2(visible_size.width - 200, 100));
    confirm_button->setScale(0.8f);
    confirm_button->setEnabled(false);  // 默认不可点击
    confirm_button->setName("confirm_attack");
    panel->addChild(confirm_button);

    confirm_button->addClickEventListener([panel, this, select_bgm](cocos2d::Ref* sender) {
        if (can_confirm_ != -1) {
            // 播放音效
            int button_hit = cocos2d::AudioEngine::play2d("music/button.mp3", false, 0.7f);
            // 检查音频的状态，直到播放完成
            this->schedule([button_hit, this](float dt) {
                if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                    // 停止音效播放并释放资源
                    cocos2d::AudioEngine::uncache("music/button.mp3");
                    this->unschedule("stop_audio_key"); // 停止检查
                }
                }, 0.1f, "stop_audio_key");

            CocController::getInstance()->changeScene(can_confirm_ + 1);
            panel->removeFromParent();
            this->selected_item_bg_ = nullptr;
            AudioEngine::stop(select_bgm);
        }
    });

    // 退出按钮
    auto exit_button = cocos2d::ui::Button::create("attack_scene/exit.png");
    exit_button->setPosition(cocos2d::Vec2(200, 100));
    exit_button->setScale(0.8f);
    exit_button->addClickEventListener([panel, this, select_bgm](cocos2d::Ref* sender) {
        // 播放音效
        int button_hit = cocos2d::AudioEngine::play2d("music/button.mp3", false, 0.7f);
        // 检查音频的状态，直到播放完成
        this->schedule([button_hit, this](float dt) {
            if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                // 停止音效播放并释放资源
                cocos2d::AudioEngine::uncache("music/button.mp3");
                this->unschedule("stop_audio_key"); // 停止检查
            }
            }, 0.1f, "stop_audio_key");

        panel->removeFromParent();
        selected_item_bg_ = nullptr;
        AudioEngine::stop(select_bgm);
    });
    panel->addChild(exit_button);

    // 创建选项
    const std::array<std::string, 4> scene_names = { "关卡1", "关卡2", "关卡3", "关卡4" };
    const std::array<std::string, 4> scene_images = { "attack_scene/Scenery1.webp", "attack_scene/Scenery2.webp", "attack_scene/Scenery3.webp", "attack_scene/Scenery4.webp" };

    for (size_t i = 0; i < scene_names.size(); i++) {
        createOptionItem(panel, i, scene_names[i], scene_images[i], confirm_button);
    }
}

void UICommonHelper::createOptionItem(cocos2d::Node* panel, int index, const std::string& name, const std::string& image_path, cocos2d::ui::Button* confirm_button)
{
    float button_width = 350;
    float button_height = 400;
    float padding = 130;

    // 选项背景
    auto item_bg = cocos2d::LayerColor::create(cocos2d::Color4B(255, 255, 255, 255), button_width, button_height);
    item_bg->setPosition(cocos2d::Vec2((button_width + padding) * index + 50, 350));
    item_bg->setTag(index);

    // 选项图片
    auto item_pic = cocos2d::Sprite::create(image_path);
    if (item_pic) {
        float scale = std::min(button_width / item_pic->getContentSize().width, button_height / item_pic->getContentSize().height);
        item_pic->setPosition(cocos2d::Vec2(button_width / 2, button_height / 2 + 20));
        item_pic->setScale(scale);
        item_bg->addChild(item_pic);
    }

    // 显示场景名称
    auto name_label = cocos2d::Label::createWithSystemFont(name, "Arial", 34);
    name_label->setPosition(cocos2d::Vec2(button_width / 2, 25));
    name_label->setColor(cocos2d::Color3B::BLACK);
    item_bg->addChild(name_label, 150);

    panel->addChild(item_bg);

    // 添加触摸事件监听器
    auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
    touch_listener->setSwallowTouches(true); // 吞噬触摸，防止穿透
    touch_listener->onTouchBegan = [item_bg, index, confirm_button, this](cocos2d::Touch* touch, cocos2d::Event* event) {
        cocos2d::Rect button_rect = item_bg->getBoundingBox();
        // 将触摸点转换为父节点坐标系（因为getBoundingBox是相对于父节点的）
        cocos2d::Vec2 touch_location = item_bg->getParent()->convertTouchToNodeSpace(touch);
        
        if (button_rect.containsPoint(touch_location)) {
            onOptionClick(item_bg, index, confirm_button);
            return true;
        }
        return false;
    };
    panel->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, item_bg);
}

void UICommonHelper::onOptionClick(cocos2d::LayerColor* item_bg, int index, cocos2d::ui::Button* confirm_button)
{
    // 播放音效
    int button_hit = cocos2d::AudioEngine::play2d("music/button.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule([button_hit, this](float dt) {
        if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
            // 停止音效播放并释放资源
            cocos2d::AudioEngine::uncache("music/button.mp3");
            this->unschedule("stop_audio_key"); // 停止检查
        }
        }, 0.1f, "stop_audio_key");

    if (selected_item_bg_ == item_bg) return;

    // 取消之前的选中状态
    if (selected_item_bg_) {
        selected_item_bg_->setColor(cocos2d::Color3B::WHITE);
        remove_border(selected_item_bg_);
    }

    // 设置新的选中状态
    selected_item_bg_ = item_bg;
    if (selected_item_bg_) {
        selected_item_bg_->setColor(cocos2d::Color3B::BLUE);
        draw_border(selected_item_bg_);
        confirm_button->setEnabled(true);
        can_confirm_ = index;
    }
}

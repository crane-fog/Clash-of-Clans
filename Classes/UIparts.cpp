#include "UIparts.h"

#include "cocos/ui/CocosGUI.h"
#include "DataHelper.h"
#include "MainVillageScene.h"
USING_NS_CC;
using namespace ui;

bool UIBars::init()
{
    if (!Node::init()) {
        return false;
    }

    // 获取屏幕尺寸
    auto visible_size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    unsigned long long gold = ResourceManager::getInstance()->getGold();
    unsigned long long elixir = ResourceManager::getInstance()->getElixir();
    unsigned long long max_gold = ResourceManager::getInstance()->getMaxGold();
    unsigned long long max_elixir = ResourceManager::getInstance()->getMaxElixir();
    unsigned long long jewel = ResourceManager::getInstance()->getJewel();
    unsigned long long max_jewel = ResourceManager::getInstance()->getMaxJewel();

    // 创建多个进度条:金币和圣水
    createProgressBarWithBackground("金币", Color3B::YELLOW, "Gold.png", gold, visible_size.width - 500,
                                    visible_size.height - 50, max_gold);
    createProgressBarWithBackground("圣水", Color3B(128, 0, 158), "Elixir.png", elixir, visible_size.width - 500,
                                    visible_size.height - 150, max_elixir);
    createProgressBarWithBackground("宝石", Color3B(255, 0, 255), "Jewel.jpg", jewel, visible_size.width - 500,
                                    visible_size.height - 250, max_jewel);

    // 创建返回按钮 - 固定在左上角
    auto back_button = ui::Button::create("BackButton.png");
    back_button->setPosition(Vec2(80, visible_size.height - 80));
    back_button->addClickEventListener([&](Ref* sender) {
        // 按钮点击事件
    });
    this->addChild(back_button);

    // 注册金币更新事件监听
    gold_update_listener_ =
        cocos2d::EventListenerCustom::create("update_gold_event", CC_CALLBACK_1(UIBars::onGoldUpdated, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(gold_update_listener_,
                                                                                                   this);

    // 注册圣水更新事件监听
    elixir_update_listener_ =
        cocos2d::EventListenerCustom::create("update_elixir_event", CC_CALLBACK_1(UIBars::onElixirUpdated, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(elixir_update_listener_,
                                                                                                   this);

    // 注册宝石更新事件监听
    elixir_update_listener_ =
        cocos2d::EventListenerCustom::create("update_jewel_event", CC_CALLBACK_1(UIBars::onJewelUpdated, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(elixir_update_listener_,
                                                                                                   this);

    // 注册最大金币更新事件监听
    max_gold_update_listener_ =
        cocos2d::EventListenerCustom::create("update_max_gold_event", CC_CALLBACK_1(UIBars::onMaxGoldUpdated, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
        max_gold_update_listener_, this);

    // 注册最大圣水更新事件监听
    max_elixir_update_listener_ = cocos2d::EventListenerCustom::create("update_max_elixir_event",
                                                                   CC_CALLBACK_1(UIBars::onMaxElixirUpdated, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
        max_elixir_update_listener_, this);

    return true;
}
void UIBars::createProgressBarWithBackground(const std::string& title, const cocos2d::Color3B& barColor,
                                             const std::string& iconPath, unsigned long long nowAmount, float x,
                                             float y, unsigned long long UpperLimit)
{
    ProgressBarData data;
    data.title_ = title;

    // 创建图像图标
    data.icon_ = Sprite::create(iconPath);  // 图标图片
    if (data.icon_) {
        data.icon_->setPosition(Vec2(x + (UpperLimit == 0 ? 20 : 450), y));  // 滑动条右边
        data.icon_->setScale(1.0f);                 // 调整图标大小
        this->addChild(data.icon_);
    }
    // 创建标题
    auto title_label = Label::createWithSystemFont(title, "Arial", 45);
    title_label->setPosition(Vec2(x - 220, y));
    title_label->setTextColor(Color4B::WHITE);
    title_label->setAnchorPoint(Vec2(0, 0.5));
    this->addChild(title_label);

    // 创建背景框
    auto background = LayerColor::create(Color4B(255, 255, 255, 150), (UpperLimit == 0 ? 70.0f : 500.0f), 40.0f);  // 黑色半透明
    background->setPosition(Vec2(x - 100, y - 24.0f));                           // 设置位置
    this->addChild(background, 0);
    data.background_ = nullptr;  // 由于使用LayerColor，这里设为null

    if (UpperLimit > 0) {
        float percent = nowAmount * 100.0f / UpperLimit;
    
        // 创建进度条
        data.loading_bar_ = cocos2d::ui::LoadingBar::create("LoadingBarFile.png");
        if (data.loading_bar_) {
            data.loading_bar_->setDirection(cocos2d::ui::LoadingBar::Direction::RIGHT);
            data.loading_bar_->setPercent(percent);
            data.loading_bar_->setPosition(Vec2(x + 150, y - 4));
            data.loading_bar_->setScaleX(2.4f);
            data.loading_bar_->setScaleY(2.0f);
            data.loading_bar_->setColor(barColor);
            this->addChild(data.loading_bar_, 1);
        }

        // 创建数量标签
        data.percent_label_ =
            Label::createWithSystemFont(StringUtils::format("%llu / %llu", nowAmount, UpperLimit), "Arial", 30);
        data.percent_label_->setPosition(Vec2(x + 150, y - 5));
        data.percent_label_->setTextColor(Color4B::BLACK);
        this->addChild(data.percent_label_, 2);
    }
    else {
        // 创建数量标签 (仅显示数字)
        data.percent_label_ = Label::createWithSystemFont(StringUtils::format("%llu", nowAmount), "Arial", 30);
        data.percent_label_->setPosition(Vec2(x - 80, y - 5));
        data.percent_label_->setTextColor(Color4B::BLACK);
        this->addChild(data.percent_label_, 2);
    }

    // 保存到容器
    progress_bars_.push_back(data);
}

void UIBars::updateProgressBar(const std::string& title, unsigned long long nowAmount, unsigned long long maxAmount)
{
    for (auto& data : progress_bars_) {
        if (data.title_ == title) {
            if (data.loading_bar_ && data.percent_label_) {
                if (maxAmount > 0) {
                    float percent = (float)nowAmount * 100.0f / maxAmount;
                    data.loading_bar_->setPercent(percent);
                    data.percent_label_->setString(StringUtils::format("%llu / %llu", nowAmount, maxAmount));
                }
                else {
                    data.percent_label_->setString(StringUtils::format("%llu", nowAmount));
                }
            }
            break;
        }
    }
}

void UIBars::onGoldUpdated(cocos2d::EventCustom* event)
{
    unsigned long long gold = *static_cast<unsigned long long*>(event->getUserData());
    unsigned long long max_gold = ResourceManager::getInstance()->getMaxGold();
    updateProgressBar("金币", gold, max_gold);
}
void UIBars::onElixirUpdated(cocos2d::EventCustom* event)
{
    unsigned long long elixir = *static_cast<unsigned long long*>(event->getUserData());
    unsigned long long max_elixir = ResourceManager::getInstance()->getMaxElixir();
    updateProgressBar("圣水", elixir, max_elixir);
}
void UIBars::onJewelUpdated(cocos2d::EventCustom* event)
{
    unsigned long long jewel = *static_cast<unsigned long long*>(event->getUserData());
    updateProgressBar("宝石", jewel, 0);
}
void UIBars::onMaxGoldUpdated(cocos2d::EventCustom* event)
{
    unsigned long long max_gold = *static_cast<unsigned long long*>(event->getUserData());
    unsigned long long gold = ResourceManager::getInstance()->getGold();
    updateProgressBar("金币", gold, max_gold);
}

void UIBars::onMaxElixirUpdated(cocos2d::EventCustom* event)
{
    unsigned long long max_elixir = *static_cast<unsigned long long*>(event->getUserData());
    unsigned long long elixir = ResourceManager::getInstance()->getElixir();
    updateProgressBar("圣水", elixir, max_elixir);
}

// 倒计时类相关
void CountdownTimer::start(unsigned int seconds, std::function<void(int)> onTick, std::function<void()> onComplete)
{
    remaining_time_ = seconds;
    total_time_ = seconds;
    on_tick_ = onTick;
    on_complete_ = onComplete;
    is_running_ = true;

    schedule([this](float dt) { this->updateTimer(dt); }, 1.0f, CC_REPEAT_FOREVER, 0, "TimerUpdate");
}

void CountdownTimer::updateTimer(float dt)
{
    if (!is_running_) return;

    if (remaining_time_ > 0) {
        remaining_time_--;

        // 回调通知
        if (on_tick_) {
            on_tick_(remaining_time_);
        }

        if (remaining_time_ == 0) {
            unschedule("TimerUpdate");
            is_running_ = false;

            if (on_complete_) {
                on_complete_();
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
            this->schedule(
                [button_hit, this](float dt) {
                    if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                        // 停止音效播放并释放资源
                        cocos2d::AudioEngine::uncache("music/button.mp3");
                        this->unschedule("stop_audio_key");  // 停止检查
                    }
                },
                0.1f, "stop_audio_key");

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
        this->schedule(
            [button_hit, this](float dt) {
                if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                    // 停止音效播放并释放资源
                    cocos2d::AudioEngine::uncache("music/button.mp3");
                    this->unschedule("stop_audio_key");  // 停止检查
                }
            },
            0.1f, "stop_audio_key");

        panel->removeFromParent();
        selected_item_bg_ = nullptr;
        AudioEngine::stop(select_bgm);
    });
    panel->addChild(exit_button);

    // 创建选项
    const std::array<std::string, 4> kSceneNames = {"关卡1", "关卡2", "关卡3", "关卡4"};
    const std::array<std::string, 4> kSceneImages = {"attack_scene/Scenery1.webp", "attack_scene/Scenery2.webp",
                                                     "attack_scene/Scenery3.webp", "attack_scene/Scenery4.webp"};

    for (unsigned int i = 0; i < kSceneNames.size(); i++) {
        int progress = 0;
        // 查找对应关卡的进度 (Level ID 1-based)
        for (const auto& info : CocController::getInstance()->level_info_list_) {
            if (info.level_ == i + 1) {
                progress = info.progress_;
                break;
            }
        }
        createOptionItem(panel, i, kSceneNames[i], kSceneImages[i], confirm_button, progress);
    }
}

void UICommonHelper::createOptionItem(cocos2d::Node* panel, int index, const std::string& name,
                                      const std::string& image_path, cocos2d::ui::Button* confirm_button, int progress)
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
        float scale = std::min(button_width / item_pic->getContentSize().width,
                               button_height / item_pic->getContentSize().height);
        item_pic->setPosition(cocos2d::Vec2(button_width / 2, button_height / 2 + 20));
        item_pic->setScale(scale);
        item_bg->addChild(item_pic);
    }

    // 显示场景名称
    auto name_label = cocos2d::Label::createWithSystemFont(name, "Arial", 34);
    name_label->setPosition(cocos2d::Vec2(button_width / 2, 25));
    name_label->setColor(cocos2d::Color3B::BLACK);
    item_bg->addChild(name_label, 150);

    // 显示进度
    if (progress >= 0) {
        auto progress_label = cocos2d::Label::createWithSystemFont(StringUtils::format("进度: %d%%", progress), "Arial", 28);
        progress_label->setPosition(cocos2d::Vec2(button_width / 2, 65));
        progress_label->setColor(cocos2d::Color3B(cocos2d::Color3B::WHITE));
        item_bg->addChild(progress_label, 150);
    }

    panel->addChild(item_bg);

    // 添加触摸事件监听器
    auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
    touch_listener->setSwallowTouches(true);  // 吞噬触摸，防止穿透
    touch_listener->onTouchBegan = [item_bg, index, confirm_button, this](cocos2d::Touch* touch,
                                                                          cocos2d::Event* event) {
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
    this->schedule(
        [button_hit, this](float dt) {
            if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                // 停止音效播放并释放资源
                cocos2d::AudioEngine::uncache("music/button.mp3");
                this->unschedule("stop_audio_key");  // 停止检查
            }
        },
        0.1f, "stop_audio_key");

    if (selected_item_bg_ == item_bg) return;

    // 取消之前的选中状态
    if (selected_item_bg_) {
        selected_item_bg_->setColor(cocos2d::Color3B::WHITE);
        removeBorder(selected_item_bg_);
    }

    // 设置新的选中状态
    selected_item_bg_ = item_bg;
    if (selected_item_bg_) {
        selected_item_bg_->setColor(cocos2d::Color3B::BLUE);
        drawBorder(selected_item_bg_);
        confirm_button->setEnabled(true);
        can_confirm_ = index;
    }
}

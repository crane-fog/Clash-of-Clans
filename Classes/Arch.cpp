#include "Arch.h"

#include <vector>

#include "ArchTargetManager.h"
#include "AudioEngine.h"
#include "BaseMap.h"
#include "CocManager.h"
#include "CocUtility.h"
#include "MainVillageScene.h"
#include "ui/CocosGUI.h"
#include "UIcommon.h"

USING_NS_CC;

std::map<unsigned char, std::function<Arch*(const ArchData&, BaseMap*, bool)>> ArchFactory::creaters;

ArchData::ArchData(Arch* a)
    : no_(a->no_),
      level_(a->level_),
      x_(a->x_),
      y_(a->y_),
      remaining_upgrade_time_(a->remaining_upgrade_time_),
      current_hp_(a->current_hp_),
      current_capacity_(a->current_capacity_)
{}

Arch* Arch::create(const ArchData& data, BaseMap* base_map, bool is_mine)
{
    Arch* p_ret = ArchFactory::createArch(data, base_map, is_mine);
    if (p_ret) {
        if (p_ret->initWithFile(kArchInfo.at(data.no_)[data.level_ - 1].image_)) {
            p_ret->autorelease();
            return p_ret;
        }
        else {
            delete p_ret;
            p_ret = nullptr;
            return nullptr;
        }
    }
    else {
        delete p_ret;
        p_ret = nullptr;
        return nullptr;
    }
}

bool Arch::initWithFile(const std::string& filename)
{
    if (!Sprite::initWithFile(filename)) {
        return false;
    }
    setAnchorPoint(Vec2(0.5f, 0.4f));
    unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
    float scale = 1.5f * CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(size, 0)).x / this->getContentSize().width;
    Vec2 middle_pos = Vec2(x_ + size / 2.0f, y_ + size / 2.0f);
    setScale(scale);
    setPosition(CoordAdaptor::cellToPixel(base_map_, middle_pos));
    base_map_->archs_.push_back(this);
    base_map_->addChild(this, CoordAdaptor::calcOrder(middle_pos));

    if (!is_mine_) {
        health_bar_ = HealthBar::create(static_cast<float>(kArchInfo.at(no_)[level_ - 1].hp_), 50.0f, false);
        if (health_bar_ == nullptr) {
            return false;
        }
        this->addChild(health_bar_);
        // 不要管这两个诡异的数据是怎么来的，反正看起来位置差不多（
        health_bar_->setHealthBarPosition(
            CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(size / 4.0f - 3.25f, size / 2.0f + 5.25f)));
    }

    return true;
}

void Arch::onEnter()
{
    Sprite::onEnter();

    updateWall();
    // 我方建筑生产资源
    if (kArchInfo.at(no_)[level_ - 1].type_ == RESOURCE && is_mine_) {
        startResourceProduction();
    }

    if (remaining_upgrade_time_ > 0) {
        // 恢复升级状态
        std::string notice = "升级";
        startUpgradeAnimation(remaining_upgrade_time_, notice);
    }

    // 添加触摸监听
    if (is_mine_) {
        touch_listener_ = EventListenerTouchOneByOne::create();
        touch_listener_->setSwallowTouches(true);

        touch_listener_->onTouchBegan = CC_CALLBACK_2(Arch::onTouchDown, this);
        touch_listener_->onTouchMoved = CC_CALLBACK_2(Arch::onTouchMove, this);
        touch_listener_->onTouchEnded = CC_CALLBACK_2(Arch::onTouchUp, this);
        touch_listener_->onTouchCancelled = CC_CALLBACK_2(Arch::onTouchCancel, this);

        _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener_, this);
    }
    else {
        // 敌方建筑开启攻击逻辑
        scheduleUpdate();
    }
}

void Arch::onExit()
{
    if (is_mine_ && touch_listener_) {
        _eventDispatcher->removeEventListener(touch_listener_);
        touch_listener_ = nullptr;
    }

    // 清理升级相关的显示和定时器，防止重复添加
    this->removeChildByName("upgrading");
    this->removeChildByName("upgrade_timer");
    this->stopActionByTag(999);  // 停止升级动画

    Sprite::onExit();
}

void Arch::createHighlight()
{
    if (highlight_node_) return;

    highlight_node_ = Node::create();
    base_map_->addChild(highlight_node_, 0);  // 层级低于建筑

    unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            auto sprite = Sprite::create("SingleCellGreen.png");
            if (sprite) {
                highlight_node_->addChild(sprite);
            }
        }
    }
    updateHighlightPos();
}

void Arch::updateHighlightPos()
{
    if (!highlight_node_) return;

    unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
    auto children = highlight_node_->getChildren();

    int index = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (index < children.size()) {
                auto sprite = children.at(index);
                Vec2 pos = CoordAdaptor::cellToPixel(base_map_, Vec2(x_ + i + 0.5f, y_ + j + 0.5f));
                sprite->setPosition(pos);
                index++;
            }
        }
    }
}

void Arch::updateHighlightColor(bool collision)
{
    if (!highlight_node_) return;
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
    std::string texture_name = collision ? "SingleCellRed.png" : "SingleCellGreen.png";

    for (auto child : highlight_node_->getChildren()) {
        auto sprite = dynamic_cast<Sprite*>(child);
        if (sprite) {
            sprite->setTexture(texture_name);
        }
    }
}

bool Arch::checkCollision(int checkX, int checkY)
{
    unsigned char my_size = kArchInfo.at(no_)[level_ - 1].size_;

    for (auto other : base_map_->archs_) {
        if (other == this) continue;

        unsigned char other_size = kArchInfo.at(other->no_)[other->level_ - 1].size_;
        unsigned char other_x = other->x_;
        unsigned char other_y = other->y_;

        bool intersect = !(checkX >= other_x + other_size || checkX + my_size <= other_x ||
                           checkY >= other_y + other_size || checkY + my_size <= other_y);

        if (intersect) {
            return true;
        }
    }
    return false;
}

void Arch::removeHighlight()
{
    if (highlight_node_) {
        highlight_node_->removeFromParent();
        highlight_node_ = nullptr;
    }
}

bool Arch::onTouchDown(Touch* touch, Event* event)
{
    Vec2 pos = this->convertToNodeSpace(touch->getLocation());

    if (pos.x >= 0 && pos.x <= getContentSize().width && pos.y >= 0 && pos.y <= getContentSize().height) {
        is_dragging_ = false;
        touch_start_pos_ = touch->getLocation();
        original_x_ = x_;
        original_y_ = y_;
        base_map_->setInputEnabled(false);  // 临时禁用地图拖动
        return true;
    }
    return false;
}

void Arch::onTouchUp(Touch* touch, Event* event)
{
    base_map_->setInputEnabled(true);  // 恢复地图拖动
    removeHighlight();
    if (!is_dragging_ && !is_upgrading_) {
        showArchPanel();
    }
    else {
        // 检查碰撞
        bool collision = checkCollision(x_, y_);
        unsigned char my_size = kArchInfo.at(no_)[level_ - 1].size_;

        if (collision) {
            // 发生碰撞，回到原位
            x_ = original_x_;
            y_ = original_y_;
            this->setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(x_ + my_size / 2.0f, y_ + my_size / 2.0f)));
            // 播放音效
            int colliide_hit = cocos2d::AudioEngine::play2d("music/fail.mp3", false, 0.7f);
            // 检查音频的状态，直到播放完成
            this->schedule(
                [colliide_hit, this](float dt) {
                    if (cocos2d::AudioEngine::getState(colliide_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                        // 停止音效播放并释放资源
                        cocos2d::AudioEngine::uncache("music/fail.mp3");
                        this->unschedule("stop_audio_key");  // 停止检查
                    }
                },
                0.1f, "stop_audio_key");
        }
        unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
        this->setLocalZOrder(CoordAdaptor::calcOrder(Vec2(x_ + size / 2.0f, y_ + size / 2.0f)));  // 恢复并设置新层级
        is_dragging_ = false;

        if (this->no_ == WALL) {
            updateSurroundingWalls(x_, y_);
            this->updateWall();
        }
    }
}

void Arch::onTouchMove(Touch* touch, Event* event)
{
    if (touch->getLocation().distance(touch_start_pos_) > 10.0f) {
        if (!is_dragging_) {
            is_dragging_ = true;
            this->setLocalZOrder(100);  // 开始拖动时置顶
            if (this->no_ == WALL) {
                updateSurroundingWalls(x_, y_, true);
                this->updateWall(nullptr, true);
            }
            createHighlight();
        }
    }

    if (is_dragging_) {
        // 获取触摸点在 BaseMap 中的位置
        Vec2 touch_in_map = base_map_->convertToNodeSpace(touch->getLocation());

        // 转换为格子坐标
        Vec2 cell_pos = CoordAdaptor::pixelToCell(base_map_, touch_in_map);

        // 建筑大小
        unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;

        // 计算新的左下角坐标 (四舍五入吸附)
        int new_x = static_cast<int>(std::round(cell_pos.x - size / 2.0f));
        int new_y = static_cast<int>(std::round(cell_pos.y - size / 2.0f));

        // 边界检查
        if (new_x < 0) new_x = 0;
        if (new_y < 0) new_y = 0;
        if (new_x > kMapSize - size) new_x = kMapSize - size;
        if (new_y > kMapSize - size) new_y = kMapSize - size;

        // 更新位置
        if (new_x != x_ || new_y != y_) {
            x_ = static_cast<unsigned char>(new_x);
            y_ = static_cast<unsigned char>(new_y);
            this->setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(x_ + size / 2.0f, y_ + size / 2.0f)));
            updateHighlightPos();

            bool collision = checkCollision(x_, y_);
            updateHighlightColor(collision);
        }
    }
}

void Arch::onTouchCancel(Touch* touch, Event* event)
{
    unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
    if (is_dragging_) {
        this->setLocalZOrder(CoordAdaptor::calcOrder(Vec2(x_ + size / 2.0f, y_ + size / 2.0f)));
    }
    base_map_->setInputEnabled(true);
    is_dragging_ = false;
    removeHighlight();
}
// 建筑信息面板
void Arch::showArchPanel()
{
    // 检查面板是否已经存在，如果存在就不再创建
    if (this->getChildByName("ARCH_PANEL")) {
        CCLOG("面板已经存在，不能重复打开！");
        return;  // 面板已经存在，直接返回
    }
    auto bg = LayerColor::create(Color4B(220, 220, 200, 180));
    bg->setContentSize(Size(400, 270));
    bg->setPosition(Vec2(150, 170));

    this->addChild(bg, 100, "ARCH_PANEL");

    // 绘制边框
    drawBorder(bg);

    // 创建面板容器
    auto panel = cocos2d::ui::Layout::create();
    panel->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    panel->setBackGroundColor(Color3B(50, 50, 50));
    panel->setBackGroundColorOpacity(200);
    panel->setContentSize(Size(340, 220));
    panel->setPosition(Vec2(30, 10));
    panel->setScale(0.8f);  // 初始缩小
    panel->setOpacity(0);   // 初始透明
    bg->addChild(panel, 100, "CONTENT_PANEL");

    const auto& info = kArchInfo.at(no_)[level_ - 1];

    auto label = Label::createWithSystemFont(
        getArchNameFromEnum(no_) + "\n------------------\n" + ("等级: " + std::to_string(level_) + "\n") +
            ("生命值: " + std::to_string(current_hp_) + "/" + std::to_string(info.hp_) + "\n"),
        "Arial", 22);
    label->setPosition(Vec2(160, 120));
    panel->addChild(label, 0, "INFO_LABEL");

    // 关闭按钮
    auto close_btn = cocos2d::ui::Button::create();
    close_btn->setTitleText("关闭");
    close_btn->setTitleFontSize(24);
    close_btn->setPosition(Vec2(70, 30));
    close_btn->addClickEventListener([=](Ref*) { this->removeChildByName("ARCH_PANEL"); });
    panel->addChild(close_btn);

    // 创建升级按钮
    auto upgrade_btn = cocos2d::ui::Button::create();
    upgrade_btn->setTitleText("升级");
    upgrade_btn->setTitleFontSize(24);
    upgrade_btn->setPosition(Vec2(230, 30));     // 设置按钮位置在左下角
    upgrade_btn->setContentSize(Size(100, 40));  // 设置按钮大小
    upgrade_btn->addClickEventListener([=](Ref*) {
        // 处理升级操作
        CCLOG("升级按钮点击");
        archUpgrade();
    });
    panel->addChild(upgrade_btn);

    // 吞噬所有触摸
    panel->setTouchEnabled(true);    // 启用触摸事件
    panel->setSwallowTouches(true);  // 吞噬触摸事件

    // 执行显示动画序列
    auto show_sequence = Sequence::create(
        // 第一步：淡入遮罩
        CallFunc::create([bg]() { bg->setOpacity(255); }),

        // 第二步：面板缩放和淡入动画
        Spawn::create(ScaleTo::create(0.2f, 1.0f),                             // 放大到正常大小
                      FadeIn::create(0.2f),                                    // 淡入
                      EaseBackOut::create(MoveBy::create(0.2f, Vec2(0, 20))),  // 轻微弹跳效果
                      nullptr),

        // 第三步：添加轻微抖动（模拟弹出效果）
        Sequence::create(ScaleTo::create(0.05f, 1.02f), ScaleTo::create(0.05f, 1.0f), nullptr),

        nullptr);

    panel->runAction(show_sequence);
}

void Arch::closeArchPanel()
{
    // 移除面板
    this->removeChildByName("ARCH_PANEL");
}

std::string Arch::getArchNameFromEnum(unsigned char archNo)
{
    switch (archNo) {
        case TOWN_HALL:
            return "大本营";
        case WALL:
            return "城墙";
        case GOLD_STORAGE:
            return "金库";
        case ELIXIR_STORAGE:
            return "圣水罐";
        case GOLD_MINE:
            return "金矿";
        case ELIXIR_COLLECTOR:
            return "圣水收集器";
        case BARRACKS:
            return "训练营";
        case ARMY_CAMP:
            return "兵营";
        case CANNON:
            return "加农炮";
        case ARCHER_TOWER:
            return "箭塔";
        case BOMB:
            return "隐形炸弹";
        default:
            return "未知建筑";
    }
}

void Arch::archUpgrade()
{
    unsigned char max = static_cast<unsigned char>(kArchInfo.at(no_).size());
    if (level_ < max) {
        // 获取大本营等级
        unsigned char town_hall_level = 1;
        if (base_map_) {
            for (auto arch : base_map_->archs_) {
                if (arch->getNo() == TOWN_HALL) {
                    town_hall_level = arch->getLevel();
                    break;
                }
            }
        }

        // 检查是否受大本营等级限制
        if (kArchTownHallLevelLimit.find(no_) != kArchTownHallLevelLimit.end()) {
            const auto& limits = kArchTownHallLevelLimit.at(no_);
            if (level_ < limits.size()) {
                unsigned char required_th = limits[level_];
                if (town_hall_level < required_th) {
                    showRefusePopup("需大本营等级 " + std::to_string(required_th));
                    return;
                }
            }
        }

        // 创建一个新的面板显示升级前后的数据和金币提示
        createUpgradeComparisonPanel();
    }
    else {
        // 弹出提示窗显示“当前已是最高等级”
        showRefusePopup("当前已是最高等级");
    }
}
// 创建显示的弹窗
void Arch::showRefusePopup(std::string text_)
{
    auto visible_size = Director::getInstance()->getVisibleSize();

    // 创建背景遮罩
    auto popup_bg = LayerColor::create(Color4B(0, 0, 0, 180));  // 半透明背景
    popup_bg->setContentSize(Size(400, 200));
    popup_bg->setPosition(Vec2(visible_size.width / 3 - 200, visible_size.height / 3 - 100));
    this->addChild(popup_bg, 1000);  // 设置层级

    // 创建提示标签
    auto label = Label::createWithSystemFont(text_, "Arial", 30);
    label->setPosition(Vec2(popup_bg->getContentSize().width / 2, popup_bg->getContentSize().height / 2));
    label->setTextColor(Color4B::RED);
    popup_bg->addChild(label);

    // 弹窗消失动画
    auto fade_out = FadeOut::create(1.0f);                                                       // 设置渐隐动画
    auto remove_popup = RemoveSelf::create();                                                    // 移除弹窗
    auto sequence = Sequence::create(fade_out, DelayTime::create(4.0f), remove_popup, nullptr);  // 延迟4秒再消失

    popup_bg->runAction(sequence);  // 应用到整个弹窗（背景和文字）
    label->runAction(sequence->clone());
}

void Arch::createUpgradeComparisonPanel()
{
    // 创建背景遮罩
    auto popup_bg = LayerColor::create(Color4B(255, 255, 255, 255));  // 半透明背景
    popup_bg->setContentSize(Size(400, 300));
    popup_bg->setPosition(Vec2(150, 170));
    popup_bg->setTag(1000);
    this->addChild(popup_bg, 1000);  // 设置层级
    // 绘制边框
    drawBorder(popup_bg);
    // 创建标题标签
    auto title_label = Label::createWithSystemFont("确认升级", "Arial", 30);
    title_label->setPosition(Vec2(popup_bg->getContentSize().width / 2, popup_bg->getContentSize().height - 40));
    title_label->setTextColor(Color4B::BLACK);
    popup_bg->addChild(title_label);

    // 创建升级前后的数据对比标签
    auto info_label =
        Label::createWithSystemFont("当前等级: " + std::to_string(level_) + " -> " + std::to_string(level_ + 1) + "\n" +
                                        "生命值: " + std::to_string(kArchInfo.at(no_)[level_ - 1].hp_) + " -> " +
                                        std::to_string(kArchInfo.at(no_)[level_].hp_) + "\n" + "\n" +
                                        "金币需求: " + std::to_string(kArchInfo.at(no_)[level_].upgrade_cost_amount_) +
                                        "\n" + "升级用时: " + std::to_string(kArchInfo.at(no_)[level_].upgrade_time_),
                                    "Arial", 24);
    if (kArchInfo.at(no_)[level_].upgrade_cost_type_ == ELIXIR) {
        info_label->setString("当前等级: " + std::to_string(level_) + " -> " + std::to_string(level_ + 1) + "\n" +
                              "生命值: " + std::to_string(kArchInfo.at(no_)[level_ - 1].hp_) + " -> " +
                              std::to_string(kArchInfo.at(no_)[level_].hp_) + "\n" + "\n" +
                              "圣水需求: " + std::to_string(kArchInfo.at(no_)[level_].upgrade_cost_amount_) + "\n" +
                              "升级用时: " + std::to_string(kArchInfo.at(no_)[level_].upgrade_time_));
    }
    info_label->setPosition(Vec2(popup_bg->getContentSize().width / 2, popup_bg->getContentSize().height / 2));
    info_label->setTextColor(Color4B::BLACK);
    info_label->setName("INFO_LABEL");
    popup_bg->addChild(info_label);

    unsigned int cost = kArchInfo.at(no_)[level_].upgrade_cost_amount_;
    unsigned long long current = 0;
    if (kArchInfo.at(no_)[level_].upgrade_cost_type_ == GOLD) {
        current = ResourceManager::getInstance()->getGold();
    }
    else {
        current = ResourceManager::getInstance()->getElixir();
        unsigned long long current = 0;
    }
    // 创建取消按钮
    auto cancel_label = Label::createWithSystemFont("取消", "Arial", 30);
    cancel_label->setTextColor(Color4B::RED);  // 设置字体颜色为红色
    auto cancel_button = MenuItemLabel::create(cancel_label, CC_CALLBACK_1(Arch::onUpgradeCancel, this));
    cancel_button->setPosition(Vec2(popup_bg->getContentSize().width / 3, 30));

    // 创建确认按钮
    auto confirm_label = Label::createWithSystemFont("确认", "Arial", 30);
    confirm_label->setTextColor(Color4B::GREEN);  // 设置字体颜色为红色
    auto confirm_button =
        MenuItemLabel::create(confirm_label, CC_CALLBACK_1(Arch::buidingUpgrading, this, this, UPGRADING, cost, current,
                                                           kArchInfo.at(no_)[level_].upgrade_cost_type_));
    confirm_button->setPosition(Vec2(popup_bg->getContentSize().width * 2 / 3, 30));

    // 将按钮添加到菜单中
    auto menu = Menu::create(cancel_button, confirm_button, nullptr);
    menu->setPosition(Vec2::ZERO);
    popup_bg->addChild(menu);
}

void Arch::onUpgradeCancel(Ref* sender)
{
    // 关闭升级面板
    this->removeChildByTag(1000);  // 1000是面板的tag，可以根据需要调整
}

void Arch::startUpgradeAnimation(unsigned int time, const std::string& notice)
{
    // 添加亮暗效果的动画
    auto fade_out = FadeTo::create(0.5f, 50);
    auto fade_in = FadeTo::create(0.5f, 255);
    auto sequence = Sequence::create(fade_out, fade_in, nullptr);
    auto repeat = Repeat::create(sequence, time);
    repeat->setTag(999);
    this->runAction(repeat);

    // 创建升级标签
    auto upgrade_label =
        Label::createWithSystemFont(notice + "中...还需: " + std::to_string(time) + " 秒 ", "Arial", 22);
    upgrade_label->setPosition(Vec2(120, 200));
    upgrade_label->setTextColor(Color4B::BLACK);
    upgrade_label->setName("upgrading");
    upgrade_label->setTag(998);
    this->addChild(upgrade_label);

    auto timer = CountdownTimer::create();
    timer->setName("upgrade_timer");
    timer->setTag(997);
    this->addChild(timer);
    timer->start(
        time,
        [notice, this, upgrade_label](int remaining) {
            upgrade_label->setString(notice + "中...还需: " + std::to_string(remaining) + " 秒");
            this->remaining_upgrade_time_ = remaining;
            // 播放施工音效
            int upgrading_noise = cocos2d::AudioEngine::play2d("music/upgrading.mp3", false, 0.5f);
            // 检查音频的状态，直到播放完成
            this->schedule(
                [upgrading_noise, this, remaining](float dt) {
                    if (remaining == 0) {
                        // 停止音效播放并释放资源
                        cocos2d::AudioEngine::stop(upgrading_noise);
                        cocos2d::AudioEngine::uncache("music/upgrading.mp3");
                        this->unschedule("stop_audio_key");  // 停止检查
                    }
                },
                0.1f, "stop_audio_key");
        },
        [notice, this, upgrade_label]() {
            upgrade_label->setString(notice + "完成！");
            this->removeChildByName("upgrading");

            // 延迟移除定时器，防止在回调中删除自身导致崩溃
            this->scheduleOnce([this](float) { this->removeChildByName("upgrade_timer"); }, 0.0f,
                               "remove_upgrade_timer_delayed");

            this->remaining_upgrade_time_ = 0;

            // 更新图片纹理
            auto new_img = kArchInfo.at(no_)[level_ - 1].image_;
            this->setTexture(new_img);
            is_upgrading_ = false;
            // 播放音效
            int upgraded = cocos2d::AudioEngine::play2d("music/upgraded.mp3", false, 0.7f);
            // 检查音频的状态，直到播放完成
            this->schedule(
                [upgraded, this](float dt) {
                    if (cocos2d::AudioEngine::getState(upgraded) == cocos2d::AudioEngine::AudioState::PAUSED) {
                        // 停止音效播放并释放资源
                        cocos2d::AudioEngine::uncache("music/upgraded.mp3");
                        this->unschedule("stop_audio_key");  // 停止检查
                    }
                },
                0.1f, "stop_audio_key"); 
            // 更新UI显示
            showArchPanel();
            // 移除加速按钮
            this->removeChildByName("speedUpButton");
            onUpgradeFinished();
        });
}

void Arch::buidingUpgrading(Ref* sender, Arch* arch, bool a, unsigned int cost, unsigned long long currentGold,
                            bool type)
{
    if (cost > currentGold) {
        if (type == GOLD) {
            // 如果金币不足，显示金币不足的弹窗
            showRefusePopup("金币不足，无法升级！");
        }
        else {
            showRefusePopup("圣水不足，无法升级！");
        }
    }
    else {
        // 如果金币足够，进行升级操作
        is_upgrading_ = true;
        if (type == GOLD) {
            ResourceManager::getInstance()->setGold(currentGold - cost);  // 减少金币
        }
        else {
            ResourceManager::getInstance()->setElixir(currentGold - cost);  // 减少圣水
        }
        std::string notice;
        // 执行升级逻辑
        if (a) {
            arch->level_++;
            notice = "升级";
        }
        else {
            notice = "建造";
        }

        arch->current_hp_ = kArchInfo.at(arch->no_)[arch->level_ - 1].hp_;

        // 获取升级时间（持续的总时长）
        unsigned int upgrade_time = kArchInfo.at(arch->no_)[arch->level_ - 1].upgrade_time_;
        arch->remaining_upgrade_time_ = upgrade_time;

        // 关闭面板
        this->removeChildByTag(1000);
        this->removeChildByName("ARCH_PANEL");

        if (upgrade_time > 0) {
            arch->startUpgradeAnimation(upgrade_time, notice);
            // 创建一个绿色背景的加速按钮
            auto label = Label::createWithSystemFont("加速施工", "Arial", 24);
            label->setTextColor(Color4B::GREEN);
            label->setPosition(Vec2(0, 0));

            auto speed_up_button = MenuItemLabel::create(label, [=](Ref* sender) {
                // 扣除一颗宝石
                if (ResourceManager::getInstance()->getJewel() > 0) {
                    ResourceManager::getInstance()->setJewel(ResourceManager::getInstance()->getJewel() - 1);
                    arch->remaining_upgrade_time_ = 0;  // 立即完成升级
                                                        // 播放音效
                    int upgraded = cocos2d::AudioEngine::play2d("music/upgraded.mp3", false, 0.7f);
                    // 检查音频的状态，直到播放完成
                    this->schedule(
                        [upgraded, this](float dt) {
                            if (cocos2d::AudioEngine::getState(upgraded) == cocos2d::AudioEngine::AudioState::PAUSED) {
                                // 停止音效播放并释放资源
                                cocos2d::AudioEngine::uncache("music/upgraded.mp3");
                                this->unschedule("stop_audio_key");  // 停止检查
                            }
                        },
                        0.1f, "stop_audio_key");
                    // 完成升级
                    auto new_img = kArchInfo.at(arch->no_)[arch->level_ - 1].image_;
                    arch->setTexture(new_img);
                    arch->setOpacity(255);
                    arch->showArchPanel();
                    arch->onUpgradeFinished();
                    is_upgrading_ = false;
                    // 移除加速按钮
                    this->removeChildByName("speedUpButton");

                    // 移除加速动画
                    this->stopActionByTag(999);
                    this->removeChildByTag(998);
                    this->removeChildByTag(997);

                    // 显示加速完成的弹窗
                    showRefusePopup("加速完成！");
                }
                else {
                    // 宝石不足，弹出提示
                    showRefusePopup("宝石不足，无法加速施工！");
                }
            });

            // 设置按钮的背景颜色为绿色
            speed_up_button->setColor(Color3B::GREEN);
            speed_up_button->setPosition(Vec2(arch->getContentSize().width / 2, 100));  // 按钮位置调整

            auto menu = Menu::create(speed_up_button, nullptr);
            menu->setPosition(Vec2::ZERO);
            arch->addChild(menu, 1, "speedUpButton");
        }
        else {
            // 立即完成
            auto new_img = kArchInfo.at(no_)[level_ - 1].image_;
            arch->setTexture(new_img);
            arch->showArchPanel();
            arch->onUpgradeFinished();
            // 播放音效
            int upgraded = cocos2d::AudioEngine::play2d("music/upgraded.mp3", false, 0.7f);
            // 检查音频的状态，直到播放完成
            this->schedule(
                [upgraded, this](float dt) {
                    if (cocos2d::AudioEngine::getState(upgraded) == cocos2d::AudioEngine::AudioState::PAUSED) {
                        // 停止音效播放并释放资源
                        cocos2d::AudioEngine::uncache("music/upgraded.mp3");
                        this->unschedule("stop_audio_key");  // 停止检查
                    }
                },
                0.1f, "stop_audio_key");
        }
    }
}

void Arch::updateUpgradeTime(long long elapsed)
{
    if (remaining_upgrade_time_ > 0) {
        if (remaining_upgrade_time_ > elapsed) {
            remaining_upgrade_time_ -= static_cast<unsigned int>(elapsed);
        }
        else {
            remaining_upgrade_time_ = 0;
            // 升级完成，更新纹理
            auto new_img = kArchInfo.at(no_)[level_ - 1].image_;
            this->setTexture(new_img);
            // 恢复透明度
            this->setOpacity(255);
            // 播放音效
            int upgraded = cocos2d::AudioEngine::play2d("music/upgraded.mp3", false, 0.7f);
            // 检查音频的状态，直到播放完成
            this->schedule(
                [upgraded, this](float dt) {
                    if (cocos2d::AudioEngine::getState(upgraded) == cocos2d::AudioEngine::AudioState::PAUSED) {
                        // 停止音效播放并释放资源
                        cocos2d::AudioEngine::uncache("music/upgraded_add.mp3");
                        this->unschedule("stop_audio_key");  // 停止检查
                    }
                },
                0.1f, "stop_audio_key");
        }
    }
}

// 资源生产
void Arch::startResourceProduction()
{
    // 获取建筑资源的生产速度
    const auto& info = kArchInfo.at(no_)[level_ - 1];
    float produce_speed_per_second = info.produce_speed_ / 60.0f;
    if (produce_speed_per_second == 0) return;
    // 启动资源生产定时器
    this->schedule(
        [=](float dt) {
            if (current_capacity_ <= info.max_capacity_) {
                // 增加生产量，每秒按生产速度增加
                current_capacity_ += static_cast<unsigned int>(produce_speed_per_second);

                // 如果容量超过最大值，设置为最大容量
                if (current_capacity_ > info.max_capacity_) {
                    current_capacity_ = info.max_capacity_;
                }
                // 更新建筑的显示
                updateBuildingDisplay();
            }
        },
        1.0f, "resource_production_timer");  // 每秒更新一次
}

// 更新建筑资源的显示
void Arch::updateBuildingDisplay()
{
    const auto& info = kArchInfo.at(no_)[level_ - 1];

    // 如果容量大于一定值，显示资源转移图标
    if (current_capacity_ > info.max_capacity_ / 20 && !this->getChildByName("resource_icon")) {
        auto icon = cocos2d::ui::Button::create();
        if (kArchInfo.at(no_)[level_ - 1].produce_type_ == ELIXIR) {
            icon->loadTextureNormal("ElixirPop.png");
        }
        else if (kArchInfo.at(no_)[level_ - 1].produce_type_ == GOLD) {
            icon->loadTextureNormal("GoldPop.png");
        }
        icon->setPosition(Vec2(x_ + 50.0f, y_ + 180.0f));  // 显示在建筑上方
        icon->setName("resource_icon");
        this->addChild(icon);

        // 添加金币动画效果
        auto scale_up = ScaleTo::create(0.2f, 1.0f);                    // 放大到1.5倍
        auto scale_down = ScaleTo::create(0.2f, 0.7f);                  // 缩小到1.2倍
        auto bounce = Sequence::create(scale_up, scale_down, nullptr);  // 往复动画
        auto repeat_bounce = RepeatForever::create(bounce);             // 无限重复

        // 淡入效果
        auto fade_in = FadeIn::create(0.3f);  // 透明度渐变为不透明

        // 执行动画
        icon->runAction(repeat_bounce);
        icon->runAction(fade_in);  // 渐显动画

        // 给图标添加点击事件
        icon->setTouchEnabled(true);
        // 点击后将资源转移到总资源
        icon->addClickEventListener([=](Ref*) {
             unsigned int collected = 0;
            const ArchInfo& arch_info = kArchInfo.at(no_)[level_ - 1];

            if (arch_info.produce_type_ == GOLD) {
                unsigned long long current_gold = ResourceManager::getInstance()->getGold();
                unsigned long long max_gold_storage = ResourceManager::getInstance()->getMaxGold();

                unsigned long long can_add = (max_gold_storage > current_gold) ? (max_gold_storage - current_gold) : 0;
                if (can_add >= current_capacity_) {
                    collected = current_capacity_;
                    // 播放音效
                    int resources_add = cocos2d::AudioEngine::play2d("music/resources_add.mp3", false, 0.7f);
                    // 检查音频的状态，直到播放完成
                    this->schedule(
                        [resources_add, this](float dt) {
                            if (cocos2d::AudioEngine::getState(resources_add) ==
                                cocos2d::AudioEngine::AudioState::PAUSED) {
                                // 停止音效播放并释放资源
                                cocos2d::AudioEngine::uncache("music/resources_add.mp3");
                                this->unschedule("stop_audio_key");  // 停止检查
                            }
                        },
                        0.1f, "stop_audio_key");
                }
                else {
                    collected = static_cast<unsigned int>(can_add);
                }

                ResourceManager::getInstance()->setGold(current_gold + collected);
            }
            else {
                unsigned long long current_elixir = ResourceManager::getInstance()->getElixir();
                unsigned long long max_elixir_storage = ResourceManager::getInstance()->getMaxElixir();

                unsigned long long can_add =
                    (max_elixir_storage > current_elixir) ? (max_elixir_storage - current_elixir) : 0;
                if (can_add >= current_capacity_) {
                    collected = current_capacity_;
                    // 播放音效
                    int resources_add = cocos2d::AudioEngine::play2d("music/resources_add.mp3", false, 0.7f);
                    // 检查音频的状态，直到播放完成
                    this->schedule(
                        [resources_add, this](float dt) {
                            if (cocos2d::AudioEngine::getState(resources_add) ==
                                cocos2d::AudioEngine::AudioState::PAUSED) {
                                // 停止音效播放并释放资源
                                cocos2d::AudioEngine::uncache("music/resources_add.mp3");
                                this->unschedule("stop_audio_key");  // 停止检查
                            }
                        },
                        0.1f, "stop_audio_key");
                }
                else {
                    collected = static_cast<unsigned int>(can_add);
                }

                ResourceManager::getInstance()->setElixir(current_elixir + collected);
            }

            current_capacity_ -= collected;

            // 只有当资源被收集到低于阈值时才移除图标
            if (current_capacity_ <= arch_info.max_capacity_ / 20) {
                this->removeChildByName("resource_icon");
            }

            updateBuildingDisplay();  // 更新建筑显示
        });
    }
}

/* 具体建筑的虚函数重写 */
void TownHall::onDeath()
{
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("town_hall_destroyed");
    Arch::onDeath();
}

void Wall::onDeath()
{
    updateSurroundingWalls(x_, y_);
    updateWall();
    Arch::onDeath();
}

void Wall::updateSurroundingWalls(int x, int y, bool is_moving)
{
    for (auto arch : base_map_->archs_) {
        if (arch->getTargetType() != WALLT) continue;
        if (arch == this) continue;

        int dx = abs(static_cast<int>(arch->getx()) - x);
        int dy = abs(static_cast<int>(arch->gety()) - y);

        if (dx + dy == 1) {
            arch->updateWall(this, is_moving);
        }
    }
}

void Wall::updateWall(Arch* moving_wall, bool is_moving)
{
    // 清理旧的连接节点
    for (auto node : connection_nodes_) {
        if (node) node->removeFromParent();
    }
    connection_nodes_.clear();

    if (is_destroyed_) return;

    if (moving_wall == nullptr && is_moving) return;

    // 创建副本避免冲突
    std::vector<Arch*> archs_copy = base_map_->archs_;

    for (auto arch : archs_copy) {
        if (arch->getTargetType() != WALLT) continue;
        if (arch == this) continue;
        if (arch->is_destroyed_) continue;

        Wall* other = static_cast<Wall*>(arch);
        int dx = abs(other->x_ - x_);
        int dy = abs(other->y_ - y_);

        if (dx + dy == 1 && (other != moving_wall || !is_moving)) {
            int my_order = this->getLocalZOrder();
            int other_order = other->getLocalZOrder();

            bool i_am_owner = false;
            if (my_order < other_order) {
                i_am_owner = true;
            }
            else if (my_order == other_order) {
                if (this < other) i_am_owner = true;
            }

            if (i_am_owner) {
                auto connection_node = Node::create();

                Vec2 mid_cell((x_ + other->x_) / 2.0f + 0.5f, (y_ + other->y_) / 2.0f + 0.5f);
                Vec2 my_cell(x_ + 0.5f, y_ + 0.5f);

                Vec2 mid_pixel = CoordAdaptor::cellToPixel(base_map_, mid_cell);
                Vec2 my_pixel = CoordAdaptor::cellToPixel(base_map_, my_cell);

                float scale = this->getScale();
                if (scale == 0.0f) scale = 1.0f;

                Vec2 local_pos = (mid_pixel - my_pixel) / scale;

                Size size = this->getContentSize();
                Vec2 anchor_offset(size.width * 0.5f, size.height * 0.4f);

                connection_node->setPosition(anchor_offset + local_pos);

                this->addChild(connection_node);
                connection_nodes_.push_back(connection_node);

                Vec2 dir(static_cast<float>(other->x_ - x_), static_cast<float>(other->y_ - y_));
                Vec2 pixel_delta = CoordAdaptor::cellDeltaToPixelDelta(base_map_, dir);
                Vec2 local_delta = pixel_delta / scale;

                for (int k = 1; k <= 4; ++k) {
                    auto sprite = Sprite::create(kArchInfo.at(WALL)[level_ - 1].image_);
                    if (sprite) {
                        sprite->setScale(0.6f);
                        sprite->setPosition(local_delta * (k / 5.0f - 0.5f));
                        connection_node->addChild(sprite);
                    }
                }
            }
        }
    }
}

GoldStorage::GoldStorage(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine)
{
    if (is_mine) {
        ResourceManager::getInstance()->setMaxGold(kArchInfo.at(no_)[level_ - 1].max_capacity_);
    }
}

void GoldStorage::showArchPanel()
{
    if (getChildByName("ARCH_PANEL")) {
        return;
    }
    Arch::showArchPanel();
    auto panel = getChildByName("ARCH_PANEL")->getChildByName("CONTENT_PANEL");
    auto label = dynamic_cast<Label*>(panel->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    str += "储量: " + std::to_string(ResourceManager::getInstance()->getGold()) + "/" +
           std::to_string(kArchInfo.at(GOLD_STORAGE)[level_ - 1].max_capacity_) + "\n";
    label->setString(str);
}

void GoldStorage::createUpgradeComparisonPanel()
{
    Arch::createUpgradeComparisonPanel();
    auto popup_bg = getChildByTag(1000);
    auto label = dynamic_cast<Label*>(popup_bg->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    std::string split = "\n\n";
    size_t pos = str.find(split);
    std::string add = "最大储量: " + std::to_string(kArchInfo.at(no_)[level_ - 1].max_capacity_) + " -> " +
                      std::to_string(kArchInfo.at(no_)[level_].max_capacity_) + "\n";
    str.insert(pos + 1, add);
    label->setString(str);
}

void GoldStorage::onUpgradeFinished()
{
    ResourceManager::getInstance()->setMaxGold(kArchInfo.at(no_)[level_ - 1].max_capacity_);
}

ElixirStorage::ElixirStorage(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine)
{
    if (is_mine) {
        ResourceManager::getInstance()->setMaxElixir(kArchInfo.at(no_)[level_ - 1].max_capacity_);
    }
}

void ElixirStorage::showArchPanel()
{
    if (getChildByName("ARCH_PANEL")) {
        return;
    }
    Arch::showArchPanel();
    auto panel = getChildByName("ARCH_PANEL")->getChildByName("CONTENT_PANEL");
    auto label = dynamic_cast<Label*>(panel->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    str += "储量: " + std::to_string(ResourceManager::getInstance()->getElixir()) + "/" +
           std::to_string(kArchInfo.at(ELIXIR_STORAGE)[level_ - 1].max_capacity_) + "\n";
    label->setString(str);
}

void ElixirStorage::createUpgradeComparisonPanel()
{
    Arch::createUpgradeComparisonPanel();
    auto popup_bg = getChildByTag(1000);
    auto label = dynamic_cast<Label*>(popup_bg->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    std::string split = "\n\n";
    size_t pos = str.find(split);
    std::string add = "最大储量: " + std::to_string(kArchInfo.at(no_)[level_ - 1].max_capacity_) + " -> " +
                      std::to_string(kArchInfo.at(no_)[level_].max_capacity_) + "\n";
    str.insert(pos + 1, add);
    label->setString(str);
}

void ElixirStorage::onUpgradeFinished()
{
    ResourceManager::getInstance()->setMaxElixir(kArchInfo.at(no_)[level_ - 1].max_capacity_);
}

void GoldMine::showArchPanel()
{
    if (getChildByName("ARCH_PANEL")) {
        return;
    }
    Arch::showArchPanel();
    auto panel = getChildByName("ARCH_PANEL")->getChildByName("CONTENT_PANEL");
    auto label = dynamic_cast<Label*>(panel->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    str += "生产速度: " + std::to_string(kArchInfo.at(GOLD_MINE)[level_ - 1].produce_speed_) + " 金币/分钟\n";
    str += "当前容量: " + std::to_string(current_capacity_) + "/" +
           std::to_string(kArchInfo.at(GOLD_MINE)[level_ - 1].max_capacity_) + "\n";
    label->setString(str);
}

void GoldMine::createUpgradeComparisonPanel()
{
    Arch::createUpgradeComparisonPanel();
    auto popup_bg = getChildByTag(1000);
    auto label = dynamic_cast<Label*>(popup_bg->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    std::string split = "\n\n";
    size_t pos = str.find(split);
    std::string add = "生产速度: " + std::to_string(kArchInfo.at(no_)[level_ - 1].produce_speed_) + " -> " +
                      std::to_string(kArchInfo.at(no_)[level_].produce_speed_) + "\n";
    add += "最大容量: " + std::to_string(kArchInfo.at(no_)[level_ - 1].max_capacity_) + " -> " +
           std::to_string(kArchInfo.at(no_)[level_].max_capacity_) + "\n";
    str.insert(pos + 1, add);
    label->setString(str);
}

void ElixirCollector::showArchPanel()
{
    if (getChildByName("ARCH_PANEL")) {
        return;
    }
    Arch::showArchPanel();
    auto panel = getChildByName("ARCH_PANEL")->getChildByName("CONTENT_PANEL");
    auto label = dynamic_cast<Label*>(panel->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    str += "生产速度: " + std::to_string(kArchInfo.at(ELIXIR_COLLECTOR)[level_ - 1].produce_speed_) + " 圣水/分钟\n";
    str += "当前容量: " + std::to_string(current_capacity_) + "/" +
           std::to_string(kArchInfo.at(ELIXIR_COLLECTOR)[level_ - 1].max_capacity_) + "\n";
    label->setString(str);
}

void ElixirCollector::createUpgradeComparisonPanel()
{
    Arch::createUpgradeComparisonPanel();
    auto popup_bg = getChildByTag(1000);
    auto label = dynamic_cast<Label*>(popup_bg->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    std::string split = "\n\n";
    size_t pos = str.find(split);
    std::string add = "生产速度: " + std::to_string(kArchInfo.at(no_)[level_ - 1].produce_speed_) + " -> " +
                      std::to_string(kArchInfo.at(no_)[level_].produce_speed_) + "\n";
    add += "最大容量: " + std::to_string(kArchInfo.at(no_)[level_ - 1].max_capacity_) + " -> " +
           std::to_string(kArchInfo.at(no_)[level_].max_capacity_) + "\n";
    str.insert(pos + 1, add);
    label->setString(str);
}

Barracks::Barracks(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine)
{
    if (is_mine) {
        TroopConfig::getInstance()->setBarrackLevel(level_);
    }
}

void Barracks::showArchPanel()
{
    if (getChildByName("ARCH_PANEL")) {
        return;
    }
    Arch::showArchPanel();
    auto bg = dynamic_cast<LayerColor*>(getChildByName("ARCH_PANEL"));
    auto panel = bg->getChildByName("CONTENT_PANEL");
    auto label = dynamic_cast<Label*>(panel->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    str += "当前可用的兵种：\n";
    int extra_lines = 0;
    int index = TroopConfig::getInstance()->getUnlockedTroopIndex();
    for (int i = 0; i < index; ++i) {
        str += Troop::getTroopNameFromEnum(kTroopTypes[i]) + "\n";
        extra_lines++;
    }
    label->setString(str);

    // 动态调整面板大小
    if (extra_lines > 0) {
        float line_height = 28.0f;  // 每行大约的高度
        float added_height = extra_lines * line_height;

        // 调整背景大小
        Size bg_size = bg->getContentSize();
        bg->setContentSize(Size(bg_size.width, bg_size.height + added_height));

        // 调整内容面板大小
        Size panel_size = panel->getContentSize();
        panel->setContentSize(Size(panel_size.width, panel_size.height + added_height));

        // 调整标签位置
        label->setPosition(label->getPosition() + Vec2(0, added_height / 2));

        // 重绘边框
        bg->removeChildByName("border");
        drawBorder(bg);
    }
}

void Barracks::createUpgradeComparisonPanel()
{
    Arch::createUpgradeComparisonPanel();
    auto popup_bg = getChildByTag(1000);
    auto label = dynamic_cast<Label*>(popup_bg->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    std::string split = "\n\n";
    size_t pos = str.find(split);
    std::string add = "解锁兵种: ";
    for (const auto& troop : kBarracksTroopUnlock) {
        if (troop.first == level_ + 1) {
            add += Troop::getTroopNameFromEnum(troop.second) + " ";
        }
    }
    add += "\n";
    str.insert(pos + 1, add);
    label->setString(str);
}

void Barracks::onUpgradeFinished() { TroopConfig::getInstance()->setBarrackLevel(level_); }

void ArmyCamp::showArchPanel()
{
    if (getChildByName("ARCH_PANEL")) {
        return;
    }
    Arch::showArchPanel();
    auto panel = getChildByName("ARCH_PANEL")->getChildByName("CONTENT_PANEL");
    auto label = dynamic_cast<Label*>(panel->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    str += "兵营容量: " + std::to_string(kArmyCampCapacity[level_ - 1]) + "\n";
    label->setString(str);
}

void ArmyCamp::createUpgradeComparisonPanel()
{
    Arch::createUpgradeComparisonPanel();
    auto popup_bg = getChildByTag(1000);
    auto label = dynamic_cast<Label*>(popup_bg->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    std::string split = "\n\n";
    size_t pos = str.find(split);
    std::string add = "兵营容量: " + std::to_string(kArmyCampCapacity[level_ - 1]) + " -> " +
                      std::to_string(kArmyCampCapacity[level_]) + "\n";
    str.insert(pos + 1, add);
    label->setString(str);
}

void ArmyCamp::onUpgradeFinished() { TroopConfig::getInstance()->setArmyCampCapacity(kArmyCampCapacity[level_ - 1]); }

void Cannon::showArchPanel()
{
    if (getChildByName("ARCH_PANEL")) {
        return;
    }
    Arch::showArchPanel();
    auto panel = getChildByName("ARCH_PANEL")->getChildByName("CONTENT_PANEL");
    auto label = dynamic_cast<Label*>(panel->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    str += "每秒伤害: " +
           std::to_string(static_cast<int>(kArchInfo.at(CANNON)[level_ - 1].damage_ /
                                           (kArchInfo.at(CANNON)[level_ - 1].attack_interval_ / 1000.0))) +
           "\n";
    label->setString(str);
}

void ArcherTower::showArchPanel()
{
    if (getChildByName("ARCH_PANEL")) {
        return;
    }
    Arch::showArchPanel();
    auto panel = getChildByName("ARCH_PANEL")->getChildByName("CONTENT_PANEL");
    auto label = dynamic_cast<Label*>(panel->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    str += "每秒伤害: " +
           std::to_string(static_cast<int>(kArchInfo.at(ARCHER_TOWER)[level_ - 1].damage_ /
                                           (kArchInfo.at(ARCHER_TOWER)[level_ - 1].attack_interval_ / 1000.0))) +
           "\n";
    label->setString(str);
}

Bomb::Bomb(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine) 
{
    if (!is_mine) {
        setVisible(false);
    }
}

void Bomb::showArchPanel()
{
    if (getChildByName("ARCH_PANEL")) {
        return;
    }
    Arch::showArchPanel();
    auto panel = getChildByName("ARCH_PANEL")->getChildByName("CONTENT_PANEL");
    auto label = dynamic_cast<Label*>(panel->getChildByName("INFO_LABEL"));
    std::string str = label->getString();
    str.pop_back();
    auto pos = str.find_last_of('\n');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    str += "\n爆炸伤害: " + std::to_string(kArchInfo.at(BOMB)[level_ - 1].damage_) + "\n";
    label->setString(str);
}

void Arch::update(float dt)
{
    if (is_destroyed_) return;

    // 检查是否为防御建筑且有伤害
    const auto& info = kArchInfo.at(no_)[level_ - 1];
    if (info.damage_ > 0 && info.type_ == DEFENSE) {
        tryAttack(dt);
    }
}

void Arch::tryAttack(float dt)
{
    attack_timer_ += dt;
    const auto& info = kArchInfo.at(no_)[level_ - 1];
    float interval = info.attack_interval_ / 1000.0f;

    // 检查当前目标是否有效
    if (current_target_) {
        if (!current_target_->isAlive()) {
            current_target_ = nullptr;
        }
        else {
            // 检查距离
            float range = info.attack_range_ / 10.0f;
            float size;
            Vec2 my_pos = getCellPosition(size);
            // 简单的距离判断，未考虑目标体积
            if (my_pos.distance(current_target_->getCellPosition()) > range) {
                current_target_ = nullptr;
            }
        }
    }

    // 如果没有目标，查找新目标
    if (!current_target_) {
        float range = info.attack_range_ / 10.0f;
        float size;
        Vec2 my_pos = getCellPosition(size);
        current_target_ = ArchTargetManager::getInstance()->getNearestArchTarget(my_pos, range, info.target_type_);
    }

    // 攻击
    if (current_target_ && attack_timer_ >= interval) {
        attack_timer_ = 0;
        // 造成伤害
        current_target_->takeDamage(static_cast<float>(info.damage_));

        // 播放攻击音效
        if (no_ == CANNON) {
            cocos2d::AudioEngine::play2d("music/cannon_attack.mp3");
        }
        else if (no_ == ARCHER_TOWER) {
            cocos2d::AudioEngine::play2d("music/archer_tower_attack.mp3");
        }
    }
}

void Bomb::update(float dt)
{
    if (is_destroyed_) return;
    
    const auto& info = kArchInfo.at(no_)[level_ - 1];
    // 隐形炸弹逻辑：检测范围内是否有敌人
    float range = info.attack_range_ / 10.0f;
    float size;
    Vec2 my_pos = getCellPosition(size);

    // 查找范围内最近的敌人
    IArchTarget* target = ArchTargetManager::getInstance()->getNearestArchTarget(my_pos, range, info.target_type_);

    if (target) {
        setVisible(true);
        // 发现敌人，爆炸对目标造成伤害（理想情况应该是AOE，未实现）
        target->takeDamage(static_cast<float>(info.damage_));
        // 播放爆炸音效
        cocos2d::AudioEngine::play2d("music/bomb_hit.mp3");
        // 自身销毁
        takeDamage(static_cast<float>(current_hp_ + 1));
        // 播放音效
        int arch_hit = cocos2d::AudioEngine::play2d("music/arch_hit.mp3", false, 0.7f);
        // 检查音频的状态，直到播放完成
        this->schedule(
            [arch_hit, this](float dt) {
                if (cocos2d::AudioEngine::getState(arch_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                    // 停止音效播放并释放资源
                    cocos2d::AudioEngine::uncache("music/arch_hit.mp3");
                    this->unschedule("stop_audio_key");  // 停止检查
                }
            },
            0.1f, "stop_audio_key");
    }
}

void Bomb::onDeath()
{
    setVisible(true);
    //等待1s后执行Arch::onDeath()
    this->runAction(
        Sequence::create(DelayTime::create(1.0f), CallFunc::create([this]() { Arch::onDeath(); }), nullptr));
}

void Arch::onDeath()
{
    is_destroyed_ = true;
    health_bar_->setVisible(false);
    TroopTargetManager::getInstance()->unregisterTroopTarget(this);
    this->setTexture("arch/Arch_Destroyed.png");

    this->setLocalZOrder(5);
}

void Arch::takeDamage(float damage)
{
    if (current_hp_ <= 0) return;
    if (kArchInfo.at(no_)[level_ - 1].type_ == RESOURCE && !is_mine_) {
        float actual_damage = std::min(damage, static_cast<float>(current_hp_));
        float p = actual_damage / kArchInfo.at(no_)[level_ - 1].hp_;
        unsigned long long resource_get = static_cast<unsigned long long>(current_capacity_ * p);

        if (!CocManager::getInstance()->isReplay()) {
            if (kArchInfo.at(no_)[level_ - 1].produce_type_ == GOLD)
                ResourceManager::getInstance()->setGold(
                    std::min(ResourceManager::getInstance()->getGold() + resource_get,
                             ResourceManager::getInstance()->getMaxGold()));
            else if (kArchInfo.at(no_)[level_ - 1].produce_type_ == ELIXIR)
                ResourceManager::getInstance()->setElixir(
                    std::min(ResourceManager::getInstance()->getElixir() + resource_get,
                             ResourceManager::getInstance()->getMaxElixir()));
        }
    }
    health_bar_->takeDamage(damage);
    current_hp_ -= static_cast<UI>(damage);
    if (current_hp_ <= 0) onDeath();
}

cocos2d::Vec2 Arch::getCellPosition(float& size) const
{
    size = static_cast<float>(kArchInfo.at(no_)[level_ - 1].size_);
    return cocos2d::Vec2(x_ + size / 2.0f, y_ + size / 2.0f);
}

Arch* ArchFactory::createArch(const ArchData& data, BaseMap* base_map, bool is_mine)
{
    auto it = creaters.find(data.no_);
    if (it != creaters.end()) {
        return it->second(data, base_map, is_mine);
    }
    return nullptr;
}
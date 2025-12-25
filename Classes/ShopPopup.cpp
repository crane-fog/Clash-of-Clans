#include "ShopPopup.h"

#include <fstream>
#include <vector>

#include "Arch.h"
#include "ArchInfo.h"
#include "AudioEngine.h"
#include "MainVillageScene.h"
#include "UIcommon.h"
#include "UIparts.h"

USING_NS_CC;
using namespace ui;

void ShopPopup::setupBackground()
{
    auto visible_size = Director::getInstance()->getVisibleSize();

    // 创建背景层（继承自 Layer，可以接收触摸）
    auto background = Layer::create();
    background->setContentSize(visible_size);

    // 添加半透明黑色背景
    auto color_bg = LayerColor::create(Color4B(0, 0, 0, 180));  // 深色半透明
    color_bg->setContentSize(visible_size);
    background->addChild(color_bg);

    // 添加触摸监听器 - 拦截所有触摸事件
    auto touch_listener = EventListenerTouchOneByOne::create();
    touch_listener->setSwallowTouches(true);  // 吞噬触摸，不传递到下层

    touch_listener->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event) {
        // 点击背景的任何位置都返回 true，表示处理这个事件
        return true;
    };

    touch_listener->onTouchEnded = [this](cocos2d::Touch* touch, cocos2d::Event* event) {
        // 可以添加点击背景关闭的功能（可选）
        // auto location = touch->getLocation();
        // 检查是否点击在面板外
    };

    // 注意：监听器要附加到 background（Layer），不是 colorBg（LayerColor）
    background->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, background);

    this->addChild(background, -1);
}
bool ShopPopup::init()
{
    if (!Layer::init()) {
        return false;
    }

    auto visible_size = Director::getInstance()->getVisibleSize();

    setupBackground();

    // 面板背景
    auto panel_bg = LayerColor::create(Color4B(255, 204, 153, 255), visible_size.width * 0.8f, visible_size.height * 0.6f);
    panel_bg->setPosition(Vec2(visible_size.width * 0.1f, visible_size.height * 0.2f));
    this->addChild(panel_bg);

    // 添加边框
    drawBorder(panel_bg, 5.0f, Color4F(0.0, 0.0, 0.0, 1.0));

    // 标题
    auto title = Label::createWithSystemFont("商店", "Arial", 70);
    title->setColor(Color3B(0, 0, 0));
    title->setPosition(Vec2(panel_bg->getContentSize().width / 2, panel_bg->getContentSize().height - 50));
    panel_bg->addChild(title);

    // 计算相对于panelBg的位置
    float btn_x = panel_bg->getContentSize().width / 20;
    float btn_y = panel_bg->getContentSize().height - 50;

    // 关闭按钮背景
    auto close_btn_bg = LayerColor::create(Color4B(160, 180, 230, 255), 80, 40);
    close_btn_bg->setAnchorPoint(Vec2(0.5f, 0.5f));
    close_btn_bg->setPosition(Vec2(btn_x - 40, btn_y - 20));
    panel_bg->addChild(close_btn_bg);

    // 关闭按钮
    auto close_btn = Button::create();
    close_btn->setTitleText("关闭");
    close_btn->setTitleColor(Color3B(0, 0, 0));
    close_btn->setTitleFontSize(20);
    close_btn->setContentSize(Size(80, 40));
    close_btn->setAnchorPoint(Vec2(0.5f, 0.5f));
    close_btn->setPosition(Vec2(btn_x, btn_y));
    close_btn->addTouchEventListener(CC_CALLBACK_2(ShopPopup::onClose, this));
    panel_bg->addChild(close_btn, 9999);

    // 创建选项卡按钮
    float tab_start_y = panel_bg->getContentSize().height - 120;
    float tab_width = panel_bg->getContentSize().width / 2 - 20;

    // 建筑选项卡
    auto building_tab = ui::Button::create();

    auto building_tab_bg = LayerColor::create(Color4B::WHITE, 150, 60);
    building_tab_bg->setPosition(Vec2(-40, -20));
    building_tab->addChild(building_tab_bg, -1);

    building_tab->setTitleText("建筑");
    building_tab->setTitleFontSize(28);
    building_tab->setTitleColor(Color3B::WHITE);
    building_tab->setContentSize(Size(tab_width, 60));
    building_tab->setPosition(Vec2(tab_width / 2 + 10, tab_start_y));
    building_tab->setColor(Color3B(100, 150, 200));  // 蓝色
    building_tab->setTag(1);                         // 标记为建筑标签
    building_tab->addTouchEventListener(
        [this, building_tab, building_tab_bg](Ref* sender, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::ENDED) {
                this->switchToTab(1);  // 切换到建筑标签
                building_tab_bg->setColor(Color3B(255, 230, 200));
            }
        });
    panel_bg->addChild(building_tab, 10);
    // 法术选项卡
    auto spell_tab = ui::Button::create();

    auto spell_tab_bg = LayerColor::create(Color4B::WHITE, 150, 60);
    spell_tab_bg->setPosition(Vec2(-40, -20));
    spell_tab->addChild(spell_tab_bg, -1);

    spell_tab->setTitleText("法术");
    spell_tab->setTitleFontSize(28);
    spell_tab->setTitleColor(Color3B::WHITE);
    spell_tab->setContentSize(Size(tab_width, 60));
    spell_tab->setPosition(Vec2(tab_width + 10, tab_start_y));
    spell_tab->setColor(Color3B(100, 150, 200));  // 蓝色
    spell_tab->setTag(2);                         // 标记为法术标签
    spell_tab->addTouchEventListener([this, spell_tab, spell_tab_bg](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->switchToTab(2);  // 切换到法术标签
            spell_tab_bg->setColor(Color3B(255, 230, 200));
        }
    });
    panel_bg->addChild(spell_tab, 10);
    // 抽卡选项卡
    auto gacha_tab = ui::Button::create();

    auto gacha_tab_bg = LayerColor::create(Color4B::WHITE, 150, 60);
    gacha_tab_bg->setPosition(Vec2(-40, -20));
    gacha_tab->addChild(gacha_tab_bg, -1);

    gacha_tab->setTitleText("抽卡");
    gacha_tab->setTitleFontSize(28);
    gacha_tab->setTitleColor(Color3B::WHITE);
    gacha_tab->setContentSize(Size(tab_width, 60));
    gacha_tab->setPosition(Vec2(tab_width * 1.5f + 20, tab_start_y));
    gacha_tab->setColor(Color3B(200, 150, 100));  // 橙色
    gacha_tab->setTag(3);                         // 标记为抽卡标签
    gacha_tab->addTouchEventListener([this, gacha_tab, gacha_tab_bg](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->switchToTab(3);  // 切换到抽卡标签
            gacha_tab_bg->setColor(Color3B(255, 230, 200));
        }
    });
    panel_bg->addChild(gacha_tab, 10);

    // 创建滚动容器（初始显示建筑）
    auto scroll_view = ui::ScrollView::create();
    scroll_view->setContentSize(Size(panel_bg->getContentSize().width - 40, panel_bg->getContentSize().height));

    scroll_view->setInnerContainerSize(Size(static_cast<float>(270 * kShopItemsInfo.at(1).size()), panel_bg->getContentSize().height));
    scroll_view->setDirection(ui::ScrollView::Direction::HORIZONTAL);
    scroll_view->setPosition(Vec2(20, 100));
    scroll_view->setScrollBarEnabled(true);
    scroll_view->setScrollBarPositionFromCorner(Vec2(2, 2));
    scroll_view->setScrollBarWidth(20);
    scroll_view->setScrollBarColor(Color3B::BLACK);
    scroll_view->setTag(100);  // 给滚动容器设置tag以便后续查找
    panel_bg->addChild(scroll_view);

    // 保存商品数据供切换使用
    current_tab_ = 1;  // 默认显示建筑
    building_items_ = kShopItemsInfo.at(1);
    magic_items_ = kShopItemsInfo.at(2);
    gacha_items_ = kShopItemsInfo.at(3);
    scroll_view_ = scroll_view;  // 保存滚动容器引用

    // 初始显示建筑商品
    showItemsInScrollView(building_items_, scroll_view);
    // 初始化抽卡池
    initGachaPool();
    return true;
}
// 切换到指定标签的函数
void ShopPopup::switchToTab(int tabIndex)
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
    if (current_tab_ == tabIndex) {
        return;  // 已经是当前标签，不切换
    }

    // 更新当前标签
    current_tab_ = tabIndex;

    // 获取滚动容器
    auto scroll_view = scroll_view_;
    if (!scroll_view) return;

    // 清空滚动容器
    scroll_view->removeAllChildren();

    // 根据标签显示不同的商品
    switch (tabIndex) {
        case 1:  // 建筑
            scroll_view->setInnerContainerSize(Size(static_cast<float>(270 * building_items_.size()), scroll_view->getContentSize().height));
            showItemsInScrollView(building_items_, scroll_view, tabIndex);
            break;
        case 2:  // 法术
            scroll_view->setInnerContainerSize(Size(static_cast<float>(270 * magic_items_.size()), scroll_view->getContentSize().height));
            showItemsInScrollView(kShopItemsInfo.at(2), scroll_view, tabIndex);
            break;
        case 3:  // 抽卡
            // scrollView->setInnerContainerSize(Size(270 * gachaItems_.size(),
            // scrollView->getContentSize().height));
            // showItemsInScrollView(gachaItems_, scrollView, tabIndex);
            //  抽卡界面特殊处理
            createGachaItem();
            break;
    }

    // 滚动到最左边
    scroll_view->scrollToPercentHorizontal(0, 0.3f, true);
}
// 在滚动容器中显示商品的辅助函数
void ShopPopup::showItemsInScrollView(const std::vector<ShopItem>& items, ui::ScrollView* scrollView, int tabIndex)
{
    auto scroll_bg = LayerColor::create(Color4B(255, 230, 200, 255), static_cast<float>(270 * building_items_.size()),
                                       scrollView->getContentSize().height - 240);
    scroll_bg->setPosition(Vec2::ZERO);
    scroll_bg->setLocalZOrder(-1);  // 放在最底层
    // 将背景添加到滚动视图
    scrollView->addChild(scroll_bg);
    for (unsigned int i = 0; i < items.size(); i++) {
        const auto& item = items[i];
        // 商品背景
        auto item_bg = LayerColor::create(Color4B(160, 180, 230, 255), 250, 300);
        item_bg->setPosition(Vec2(20.0f + i * 270.0f, 20.0f));
        scrollView->addChild(item_bg);
        // 根据商品ID确定建筑类型
        unsigned char arch_no = kInvalidArchNo;
        if (tabIndex == 1) {
            switch (item.id_) {
                case 1:
                    arch_no = ARMY_CAMP;
                    break;
                case 2:
                    arch_no = WALL;
                    break;
                case 3:
                    arch_no = GOLD_STORAGE;
                    break;
                case 4:
                    arch_no = ELIXIR_STORAGE;
                    break;
                case 5:
                    arch_no = GOLD_MINE;
                    break;
                case 6:
                    arch_no = ELIXIR_COLLECTOR;
                    break;
                case 7:
                    arch_no = ARCHER_TOWER;
                    break;
                case 8:
                    arch_no = CANNON;
                    break;
                case 9:
                    arch_no = BARRACKS;
                    break;
            }
        }

        // 获取并修改金币
        unsigned long long current_gold = ResourceManager::getInstance()->getGold();
        unsigned long long current_elixir = ResourceManager::getInstance()->getElixir();

        // 检查建筑数量限制
        bool is_limit_reached = false;
        if (tabIndex == 1) {
            auto scene = dynamic_cast<MainVillage*>(Director::getInstance()->getRunningScene());
            unsigned char town_hall_level = scene->getTownHallLevel();
            if (scene->getBuildingCount(arch_no) >= kArchCount.at(arch_no)[town_hall_level - 1]) {
                is_limit_reached = true;
            }
        }

        // 设置触摸事件
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this, item_bg, item, scrollView, arch_no, current_gold, current_elixir, is_limit_reached,
                                  tabIndex](Touch* touch, Event* event) -> bool {
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
            Vec2 location_in_node = item_bg->convertToNodeSpace(touch->getLocation());
            Size size = item_bg->getContentSize();
            Rect rect = Rect(0, 0, size.width, size.height);
            auto scene = dynamic_cast<MainVillage*>(Director::getInstance()->getRunningScene());

            if (rect.containsPoint(location_in_node)) {
                if (tabIndex == 1) {
                    // 建筑：只检查数量限制和资源
                    if (is_limit_reached) {
                        this->showUnavailableBubble(item, item_bg, scrollView, "建筑数量已达上限");
                        return true;
                    }

                    bool can_afford = false;
                    if (item.p_type_ == GOLD) {
                        can_afford = current_gold >= item.price_;
                        if (!can_afford) this->showUnavailableBubble(item, item_bg, scrollView, "金币不足");
                    }
                    else {
                        can_afford = current_elixir >= item.price_;
                        if (!can_afford) this->showUnavailableBubble(item, item_bg, scrollView, "圣水不足");
                    }

                    if (can_afford) {
                        item_bg->setColor(Color3B(120, 140, 180));
                        scene->addBuildingByNO(arch_no, item.price_);
                        this->close();
                        item_bg->runAction(ScaleTo::create(0.1f, 0.95f));
                    }
                }
                else {
                    // 其他物品
                    if (item.p_type_ == GOLD) {
                        if (item.is_available_ && current_gold >= item.price_) {
                            item_bg->setColor(Color3B(120, 140, 180));
                            scene->addBuildingByNO(arch_no, item.price_);
                            this->close();
                            item_bg->runAction(ScaleTo::create(0.1f, 0.95f));
                        }
                        else {
                            if (current_gold < item.price_) {
                                this->showUnavailableBubble(item, item_bg, scrollView, "金币不足");
                            }
                            else
                                this->showUnavailableBubble(item, item_bg, scrollView, "");
                        }
                    }
                    else {
                        if (item.is_available_ && current_elixir >= item.price_) {
                            item_bg->setColor(Color3B(120, 140, 180));
                            scene->addBuildingByNO(arch_no, item.price_);
                            this->close();
                            item_bg->runAction(ScaleTo::create(0.1f, 0.95f));
                        }
                        else {
                            if (current_elixir < item.price_) {
                                this->showUnavailableBubble(item, item_bg, scrollView, "圣水不足");
                            }
                            else
                                this->showUnavailableBubble(item, item_bg, scrollView, "");
                        }
                    }
                }
                return true;
            }
            return false;
        };

        listener->onTouchEnded = [item_bg, i, item, this](Touch* touch, Event* event) {
            item_bg->setColor(Color3B(160, 180, 230));  // 恢复颜色

            Vec2 location_in_node = item_bg->convertToNodeSpace(touch->getLocation());
            Size size = item_bg->getContentSize();
            Rect rect = Rect(0, 0, size.width, size.height);

            if (rect.containsPoint(location_in_node)) {
                CCLOG("Item %d clicked: %s", i + 1, item.name_.c_str());
            }
        };

        listener->onTouchCancelled = [item_bg](Touch* touch, Event* event) { item_bg->setColor(Color3B(160, 180, 230)); };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, item_bg);

        // 灰色遮罩
        bool show_mask = false;
        if (tabIndex == 1) {
            // 建筑
            bool resource_insufficient = (item.p_type_ == GOLD && current_gold < item.price_) ||
                                        (item.p_type_ == ELIXIR && current_elixir < item.price_);
            show_mask = is_limit_reached || resource_insufficient;
        }
        else {
            // 其他
            bool resource_insufficient = (item.p_type_ == GOLD && current_gold < item.price_) ||
                                        (item.p_type_ == ELIXIR && current_elixir < item.price_);
            show_mask = !item.is_available_ || resource_insufficient;
        }

        if (show_mask) {
            Size bg_size = item_bg->getContentSize();
            auto gray_mask = LayerColor::create(Color4B(128, 128, 128, 150), bg_size.width, bg_size.height);
            gray_mask->setPosition(Vec2::ZERO);
            item_bg->addChild(gray_mask, 9999);
        }

        // 商品图片
        Sprite* item_image = nullptr;
        if (!item.image_path_.empty()) {
            item_image = Sprite::create(item.image_path_);
            if (item_image) {
                float max_size = 220.0f;  // 调整图片大小
                float scale =
                    std::min(max_size / item_image->getContentSize().width, max_size / item_image->getContentSize().height);
                item_image->setScale(scale);
                item_image->setPosition(Vec2(item_bg->getContentSize().width / 2, item_bg->getContentSize().height - 140));
                item_bg->addChild(item_image);
            }
            else {
                item_image = Sprite::create("ui/placeholder.png");
                item_image->setScale(0.5f);
                item_image->setPosition(Vec2(item_bg->getContentSize().width / 2, item_bg->getContentSize().height - 100));
                item_bg->addChild(item_image);
            }
        }

        // 商品标签
        auto item_label = Label::createWithSystemFont(item.name_, "Arial", 26);
        item_label->setPosition(Vec2(item_bg->getContentSize().width / 2, item_bg->getContentSize().height - 30));
        item_label->setColor(Color3B::BLACK);
        item_bg->addChild(item_label);
        if (item.p_type_ == ELIXIR) {
            // 商品价格图标
            auto gold_icon = Sprite::create("Elixir.png");
            gold_icon->setPosition(Vec2(item_bg->getContentSize().width / 3, 30));
            gold_icon->setScale(0.5f);
            item_bg->addChild(gold_icon);

            // 商品价格
            auto price_label = Label::createWithSystemFont("$" + std::to_string(item.price_), "Arial", 25);
            price_label->setPosition(Vec2(item_bg->getContentSize().width / 2 + 10, 30));
            price_label->setColor(Color3B::MAGENTA);
            item_bg->addChild(price_label);
        }
        else {
            // 商品价格图标
            auto gold_icon = Sprite::create("Gold.png");
            gold_icon->setPosition(Vec2(item_bg->getContentSize().width / 3, 30));
            gold_icon->setScale(0.5f);
            item_bg->addChild(gold_icon);

            // 商品价格
            auto price_label = Label::createWithSystemFont("$" + std::to_string(item.price_), "Arial", 25);
            price_label->setPosition(Vec2(item_bg->getContentSize().width / 2 + 10, 30));
            price_label->setColor(Color3B::YELLOW);
            item_bg->addChild(price_label);
        }
    }
}

// 在ShopPopup::close()函数中，确保清理放置状态
void ShopPopup::close()
{
    // 关闭逻辑...
    auto scale_to = ScaleTo::create(0.2f, 0.1f);
    auto ease_in = EaseBackIn::create(scale_to);
    auto remove = RemoveSelf::create();
    auto sequence = Sequence::create(ease_in, remove, nullptr);
    this->runAction(sequence);

    // 重新启用地图输入
    auto parent = this->getParent();
    auto map = parent->getChildByName("BaseMap");
    if (map) ((BaseMap*)map)->setInputEnabled(true);
}
void ShopPopup::show(Node* parent)
{
    parent->addChild(this, 100);

    // 显示动画
    this->setScale(0.1f);
    auto scale_to = ScaleTo::create(0.3f, 1.0f);
    auto ease_out = EaseBackOut::create(scale_to);
    this->runAction(ease_out);
    // 关闭地图输入
    auto map = parent->getChildByName("BaseMap");
    if (map) ((BaseMap*)map)->setInputEnabled(false);

    this->setScale(0.1f);
    this->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1.0f)));
}
void ShopPopup::onClose(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED) {
        close();
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
    }
}
void ShopPopup::showUnavailableBubble(const ShopItem& item, cocos2d::LayerColor* targetNode,
                                      cocos2d::ui::ScrollView* scrollView, std::string reason)
{
    // 创建提示气泡（相对于商品背景的本地坐标）
    auto bubble = Node::create();
    bubble->setPosition(Vec2(targetNode->getContentSize().width / 2,
                             targetNode->getContentSize().height + 10));  // 在商品上方显示
    bubble->setTag(8888);
    targetNode->addChild(bubble, 999);  // 添加到商品背景，而不是this

    // 气泡背景
    auto bubble_bg = LayerColor::create(Color4B(70, 70, 70, 228), 200, 80);
    bubble_bg->setPosition(Vec2(-100, 0));  // 居中
    bubble->addChild(bubble_bg);

    // 三角形箭头（指向商品）
    auto arrow = DrawNode::create();
    Vec2 arrow_points[] = {
        Vec2(90, 80),   // 左下
        Vec2(100, 90),  // 顶点
        Vec2(110, 80)   // 右下
    };
    arrow->drawSolidPoly(arrow_points, 3, Color4F(0.7f, 0.5f, 0.5f, 0.9f));
    arrow->setPosition(Vec2(0, -10));
    bubble->addChild(arrow);

    // 原因文本
    auto reason_label = Label::createWithSystemFont(reason, "Arial", 25);
    if (reason.empty()) {
        reason_label->setString(item.unavailable_reason_);
    }
    reason_label->setColor(Color3B::RED);
    reason_label->setPosition(Vec2(100, 40));
    reason_label->setWidth(180);
    reason_label->setAlignment(TextHAlignment::CENTER);
    bubble_bg->addChild(reason_label);

    // 初始缩放动画
    bubble->setScale(0.1f);
    bubble->runAction(Sequence::create(ScaleTo::create(0.2f, 1.0f), DelayTime::create(5.0f),
                                       Spawn::create(FadeOut::create(0.3f), ScaleTo::create(0.3f, 0.5f), nullptr),
                                       RemoveSelf::create(), nullptr));
}
// 在按钮点击事件中
void ShopPopup::onShopButtonClick(Ref* sender)
{
    auto popup = ShopPopup::create();
    popup->show(this);
}
// 初始化抽卡池
void ShopPopup::initGachaPool() { gacha_pool_ = kGachaItemsInfo.at(1); }

// 创建抽卡界面
void ShopPopup::createGachaItem()
{
    if (!scroll_view_) return;

    scroll_view_->removeAllChildren();

    // 抽卡界面背景
    auto gacha_bg = LayerColor::create(Color4B(50, 30, 70, 255), 350, 450);
    gacha_bg->setPosition(
        Vec2(scroll_view_->getContentSize().width / 2 - 175, scroll_view_->getContentSize().height / 2 - 400));
    scroll_view_->addChild(gacha_bg);

    // 标题
    auto title = Label::createWithSystemFont("神秘抽卡", "fonts/Marker Felt.ttf", 48);
    title->setColor(Color3B(255, 215, 0));  // 金色
    title->enableShadow(Color4B::BLACK, Size(2, -2), 0);
    title->setPosition(Vec2(175, 400));
    gacha_bg->addChild(title);

    // 描述
    auto desc = Label::createWithSystemFont("点击按钮抽取神秘物品", "Arial", 24);
    desc->setColor(Color3B::WHITE);
    desc->setPosition(Vec2(175, 350));
    gacha_bg->addChild(desc);

    // 抽卡展示区域
    auto card_area = LayerColor::create(Color4B(30, 20, 40, 255), 300, 200);
    card_area->setPosition(Vec2(25, 120));
    card_area->setTag(1001);  // 用于后续查找
    gacha_bg->addChild(card_area);

    // 问号图标（初始状态）
    auto question_mark = Sprite::create("question_mark.png");
    if (!question_mark) {
        question_mark = Sprite::create();
        auto draw = DrawNode::create();
        draw->drawCircle(Vec2(0, 0), 40, 0, 30, false, Color4F::WHITE);
        draw->drawLine(Vec2(0, 30), Vec2(0, -30), Color4F::WHITE);
        draw->drawLine(Vec2(-30, 0), Vec2(30, 0), Color4F::WHITE);
        question_mark->addChild(draw);
    }
    question_mark->setScale(0.1f);
    question_mark->setPosition(Vec2(150, 100));
    question_mark->setTag(1002);  // 用于后续替换
    card_area->addChild(question_mark);

    // 抽卡按钮
    auto gacha_button = ui::Button::create();
    gacha_button->setTitleText("开始抽卡");
    gacha_button->setTitleFontSize(32);
    gacha_button->setTitleColor(Color3B::WHITE);
    gacha_button->setContentSize(Size(200, 70));
    gacha_button->setPosition(Vec2(100, 150));
    gacha_button->setColor(Color3B(200, 50, 50));

    // 十连抽按钮
    auto ten_gacha_button = ui::Button::create();
    ten_gacha_button->setTitleText("十连抽");
    ten_gacha_button->setTitleFontSize(28);
    ten_gacha_button->setTitleColor(Color3B::WHITE);
    ten_gacha_button->setContentSize(Size(200, 70));
    ten_gacha_button->setPosition(Vec2(250, 150));
    ten_gacha_button->setColor(Color3B(180, 100, 50));

    ten_gacha_button->addTouchEventListener([this](Ref*, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->startTenGacha();
        }
    });

    gacha_bg->addChild(ten_gacha_button);

    // 按钮发光效果
    auto button_glow = Sprite::create("ui/glow_circle.png");
    if (button_glow) {
        button_glow->setScale(1.2f);
        button_glow->setPosition(Vec2(100, 35));
        button_glow->setOpacity(150);
        button_glow->runAction(
            RepeatForever::create(Sequence::create(FadeTo::create(0.8f, 200), FadeTo::create(0.8f, 100), nullptr)));
        gacha_button->addChild(button_glow, -1);
    }

    gacha_button->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->performSingleGacha(nullptr);
            ;
        }
    });
    gacha_bg->addChild(gacha_button);

    // 概率说明
    auto probability = Label::createWithSystemFont("概率: SSR 5% | SR 15% | R 30% | N 50%", "Arial", 18);
    probability->setColor(Color3B(200, 200, 200));
    probability->setPosition(Vec2(175, 100));
    gacha_bg->addChild(probability);
}

// 执行抽卡
void ShopPopup::performSingleGacha(const std::function<void(ShopItem)>& onFinished)
{
    int random_value = rand() % 100;
    Rarity rarity = RARITY_N;

    if (random_value < 5)
        rarity = RARITY_SSR;
    else if (random_value < 20)
        rarity = RARITY_SR;
    else if (random_value < 50)
        rarity = RARITY_R;
    else {  // 50% N
        rarity = RARITY_N;
    }

    showGachaAnimation(rarity);

    this->scheduleOnce(
        [this, rarity, onFinished](float) {
            std::vector<ShopItem*> items;

            for (auto& item : gacha_pool_) {
                if (item.rarity_ == rarity) {
                    items.push_back(&item);
                }
            }

            if (items.empty()) return;

            ShopItem result = *items[rand() % items.size()];

            showGachaResult(result);

            if (onFinished) {
                onFinished(result);
            }
        },
        2.5f, "single_gacha_result");
}

// 显示抽卡动画
void ShopPopup::showGachaAnimation(int rarity)
{
    // 创建全屏黑色遮罩
    auto mask = LayerColor::create(Color4B(0, 0, 0, 180));
    mask->setContentSize(Director::getInstance()->getVisibleSize());
    mask->setPosition(Vec2::ZERO);
    mask->setTag(8888);
    mask->setOpacity(0);
    this->addChild(mask, 1000);
    mask->runAction(FadeIn::create(0.3f));

    // 创建闪光效果
    auto flash = Sprite::create("flash.png");
    if (rarity == RARITY_SSR) {
    }
    else if (rarity == RARITY_SR) {
        flash->setTexture("SRflash.png");
    }
    else if (rarity == RARITY_R) {
        flash->setTexture("Rflash.png");
    }
    else {
        flash->setTexture("Nflash.png");
    }
    flash->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2,
                            Director::getInstance()->getVisibleSize().height / 2));
    flash->setScale(0.1f);
    flash->setOpacity(0);
    mask->addChild(flash);

    // 闪光动画序列
    flash->runAction(Sequence::create(
        Spawn::create(ScaleTo::create(0.5f, 3.0f), FadeIn::create(0.3f), nullptr), DelayTime::create(0.5f),
        Spawn::create(ScaleTo::create(0.5f, 0.5f), FadeOut::create(0.5f), nullptr), nullptr));

    // 创建旋转光效
    auto rotating_glow = Node::create();
    for (int i = 0; i < 8; i++) {
        auto ray = Sprite::create("flash.png");
        if (rarity == RARITY_SSR) {
            ray->setTexture("flash.png");
        }
        else if (rarity == RARITY_SR) {
            ray->setTexture("SRflash.png");
        }
        else if (rarity == RARITY_R) {
            ray->setTexture("Rflash.png");
        }
        else {
            ray->setTexture("Nflash.png");
        }
        ray->setPosition(Vec2(0, 150));
        ray->setRotation(i * 45.0f);
        ray->setOpacity(0);
        rotating_glow->addChild(ray);

        // 每个光线的淡入淡出动画
        ray->runAction(Sequence::create(DelayTime::create(i * 0.1f), FadeIn::create(0.3f), DelayTime::create(0.5f),
                                        FadeOut::create(0.3f), nullptr));
    }
    rotating_glow->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2,
                                   Director::getInstance()->getVisibleSize().height / 2));
    rotating_glow->runAction(Repeat::create(RotateBy::create(2.0f, 360), 1));
    mask->addChild(rotating_glow);

    // 抽卡中文字
    auto gacha_text = Label::createWithSystemFont("抽卡中...", "fonts/Marker Felt.ttf", 60);
    gacha_text->setColor(Color3B(255, 255, 100));
    gacha_text->enableGlow(Color4B::YELLOW);
    gacha_text->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2,
                                Director::getInstance()->getVisibleSize().height / 2 - 200));
    gacha_text->setOpacity(0);
    mask->addChild(gacha_text);

    // 文字动画
    gacha_text->runAction(Sequence::create(DelayTime::create(0.5f), FadeIn::create(0.3f), DelayTime::create(1.5f),
                                          FadeOut::create(0.3f), nullptr));

    // 2.5秒后移除遮罩
    mask->runAction(Sequence::create(DelayTime::create(2.5f), FadeOut::create(0.3f), RemoveSelf::create(), nullptr));
}

// 显示抽卡结果
void ShopPopup::showGachaResult(const ShopItem& item)
{
    // 移除之前的抽卡结果
    if (gacha_result_node_) {
        gacha_result_node_->removeFromParent();
        gacha_result_node_ = nullptr;
    }

    // 创建结果展示层
    gacha_result_node_ = Node::create();
    gacha_result_node_->setPosition(Vec2::ZERO);
    gacha_result_node_->setTag(9999);
    this->addChild(gacha_result_node_, 1001);

    // 背景遮罩
    auto result_bg = LayerColor::create(Color4B(0, 0, 0, 200));
    result_bg->setContentSize(Director::getInstance()->getVisibleSize());
    result_bg->setPosition(Vec2::ZERO);
    gacha_result_node_->addChild(result_bg);

    // 结果卡片
    auto card = LayerColor::create(Color4B(50, 50, 80, 255), 400, 500);
    card->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2 - 200,
                           Director::getInstance()->getVisibleSize().height / 2 - 250));
    gacha_result_node_->addChild(card);

    // 根据稀有度设置卡片边框颜色和光效
    Color3B border_color;
    std::string rarity_text;
    float glow_intensity = 1.0f;

    // 根据物品rarity判断稀有度
    if (item.rarity_ == RARITY_SSR) {         // SSR
        border_color = Color3B(255, 215, 0);  // 金色
        rarity_text = "SSR";
        glow_intensity = 3.0f;
    }
    else if (item.rarity_ == RARITY_SR) {       // SR
        border_color = Color3B(255, 100, 255);  // 紫色
        rarity_text = "SR";
        glow_intensity = 2.0f;
    }
    else if (item.rarity_ == RARITY_R) {        // R
        border_color = Color3B(100, 200, 255);  // 蓝色
        rarity_text = "R";
        glow_intensity = 1.5f;
    }
    else {                                     // N
        border_color = Color3B(150, 150, 150);  // 灰色
        rarity_text = "N";
        glow_intensity = 1.0f;
    }

    // 卡片边框
    auto border = DrawNode::create();
    Color4F border_color4_f(border_color);
    border->drawRect(Vec2(0, 0), Vec2(400, 500), border_color4_f);
    card->addChild(border);

    // 发光效果（根据稀有度调整强度）
    if (glow_intensity > 1.0f) {
        auto glow = Sprite::create("ui/glow_circle.png");
        if (glow) {
            glow->setScale(glow_intensity);
            glow->setPosition(Vec2(200, 250));
            glow->setColor(border_color);
            glow->setOpacity(150);
            glow->runAction(
                RepeatForever::create(Sequence::create(FadeTo::create(0.8f, 200), FadeTo::create(0.8f, 100), nullptr)));
            card->addChild(glow, -1);
        }
    }

    // 稀有度文字
    auto rarity_label = Label::createWithSystemFont(rarity_text, "fonts/Marker Felt.ttf", 72);
    rarity_label->setColor(border_color);
    rarity_label->enableGlow(Color4B(border_color.r, border_color.g, border_color.b, 255));
    rarity_label->setPosition(Vec2(200, 420));
    card->addChild(rarity_label);

    // 物品图片
    auto item_image = Sprite::create(item.image_path_);
    if (!item_image) {
        item_image = Sprite::create("ui/placeholder.png");
    }
    item_image->setScale(1.0f);
    item_image->setPosition(Vec2(200, 250));
    card->addChild(item_image);

    // 物品名称
    auto name_label = Label::createWithSystemFont(item.name_, "Arial", 36);
    name_label->setColor(Color3B::WHITE);
    name_label->setPosition(Vec2(200, 120));
    card->addChild(name_label);

    // 物品描述
    auto desc_label = Label::createWithSystemFont("恭喜获得！" + item.unavailable_reason_, "Arial", 24);
    desc_label->setColor(Color3B(200, 200, 200));
    desc_label->setPosition(Vec2(200, 80));
    desc_label->setWidth(350);
    desc_label->setAlignment(TextHAlignment::CENTER);
    card->addChild(desc_label);

    // 确定按钮
    auto ok_button = ui::Button::create();
    ok_button->setTitleText("确定");
    ok_button->setTitleFontSize(28);
    ok_button->setTitleColor(Color3B::WHITE);
    ok_button->setContentSize(Size(150, 60));
    ok_button->setPosition(Vec2(200, 30));
    ok_button->setColor(Color3B(100, 150, 200));
    ok_button->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            if (gacha_result_node_) {
                gacha_result_node_->removeFromParent();
                gacha_result_node_ = nullptr;
            }
        }
    });
    card->addChild(ok_button);

    // 5秒后自动关闭
    gacha_result_node_->runAction(Sequence::create(DelayTime::create(5.0f), CallFunc::create([this]() {
                                                     if (gacha_result_node_) {
                                                         gacha_result_node_->removeFromParent();
                                                         gacha_result_node_ = nullptr;
                                                     }
                                                 }),
                                                 nullptr));
}

void ShopPopup::startTenGacha()
{
    if (is_ten_gacha_running_) return;  // 防止重复点击

    is_ten_gacha_running_ = true;
    current_ten_index_ = 0;
    ten_results_.clear();

    runNextTenGacha();
}
void ShopPopup::runNextTenGacha()
{
    // 十次完成
    if (current_ten_index_ >= 10) {
        is_ten_gacha_running_ = false;
        CCLOG("十连抽完成");
        return;
    }

    // 执行一次普通抽卡
    performSingleGacha([this](ShopItem result) {
        ten_results_.push_back(result);
        current_ten_index_++;

        // 下一抽（给一点间隔）
        this->scheduleOnce([this](float) { runNextTenGacha(); }, 0.5f, "next_ten_gacha");
    });
}

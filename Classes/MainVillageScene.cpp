#include "MainVillageScene.h"

#include <chrono>
#include <vector>

#include "Arch.h"
#include "Archer.h"
#include "AudioEngine.h"
#include "Balloon.h"
#include "Barbarian.h"
#include "CocManager.h"
#include "cocos/ui/CocosGUI.h"
#include "CocUtility.h"
#include "Dragon.h"
#include "Giant.h"
#include "ShopPopup.h"
#include "TroopConfig.h"
#include "UIparts.h"
#include "WallBreaker.h"
USING_NS_CC;

bool MainVillage::init()
{
    if (!Village::init()) {
        return false;
    }

    unsigned long long gold = 0, elixir = 0, jewel = 0;
    // 从数据文件中读取资源数据
    if (!DataHelper::readSourceData(kSourceDataFile, gold, elixir, jewel)) {
        return false;
    }
    ResourceManager::getInstance()->setGold(gold);
    ResourceManager::getInstance()->setElixir(elixir);
    ResourceManager::getInstance()->setJewel(jewel);
    // 从数据文件中读取关卡数据
    if (!DataHelper::readLevelData(kOfflineDataFile[0], CocManager::getInstance()->level_info_list_)) {
        return false;
    }

    // 从数据文件中读取建筑数据并创建建筑对象
    time_t current_time =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    time_t data_time = 0;
    if (!DataHelper::readArchData(kMainVillageDataFile, data_time, this->arch_status_)) {
        return false;
    }

    std::vector<ArchData> arch_list;
    DataHelper::mapToList(arch_status_, arch_list);

    time_t time_diff = current_time - data_time;
    last_exit_time_ = 0;

    ArchFactory::registerCreater(TOWN_HALL, [](const ArchData& data, BaseMap* map, bool is_mine) { return new TownHall(data, map, is_mine); });
    ArchFactory::registerCreater(WALL, [](const ArchData& data, BaseMap* map, bool is_mine) { return new Wall(data, map, is_mine); });
    ArchFactory::registerCreater(GOLD_STORAGE,
                                 [](const ArchData& data, BaseMap* map, bool is_mine) { return new GoldStorage(data, map, is_mine); });
    ArchFactory::registerCreater(ELIXIR_STORAGE,
                                 [](const ArchData& data, BaseMap* map, bool is_mine) { return new ElixirStorage(data, map, is_mine); });
    ArchFactory::registerCreater(GOLD_MINE, [](const ArchData& data, BaseMap* map, bool is_mine) { return new GoldMine(data, map, is_mine); });
    ArchFactory::registerCreater(ELIXIR_COLLECTOR,
                                 [](const ArchData& data, BaseMap* map, bool is_mine) { return new ElixirCollector(data, map, is_mine); });
    ArchFactory::registerCreater(BARRACKS, [](const ArchData& data, BaseMap* map, bool is_mine) { return new Barracks(data, map, is_mine); });
    ArchFactory::registerCreater(ARMY_CAMP, [](const ArchData& data, BaseMap* map, bool is_mine) { return new ArmyCamp(data, map, is_mine); });
    ArchFactory::registerCreater(CANNON, [](const ArchData& data, BaseMap* map, bool is_mine) { return new Cannon(data, map, is_mine); });
    ArchFactory::registerCreater(ARCHER_TOWER,
                                 [](const ArchData& data, BaseMap* map, bool is_mine) { return new ArcherTower(data, map, is_mine); });
    ArchFactory::registerCreater(BOMB, [](const ArchData& data, BaseMap* map, bool is_mine) { return new Bomb(data, map, is_mine); });

    for (auto& arch : arch_list) {
        // 更新剩余升级时间
        if (arch.remaining_upgrade_time_ > 0) {
            if (arch.remaining_upgrade_time_ > time_diff) {
                arch.remaining_upgrade_time_ -= static_cast<unsigned int>(time_diff);
            }
            else {
                arch.remaining_upgrade_time_ = 0;
            }
        }

        // 离线资源生产
        const ArchInfo& info = kArchInfo.at(arch.no_)[arch.level_ - 1];
        if (info.produce_speed_ > 0) {
            float produce_speed_per_second = info.produce_speed_ / 60.0f;
            unsigned int produced_amount = static_cast<unsigned int>(produce_speed_per_second * time_diff);
            arch.current_capacity_ += produced_amount;
            if (arch.current_capacity_ > info.max_capacity_) {
                arch.current_capacity_ = info.max_capacity_;
            }
        }

        Arch::create(arch, base_map_);
    }

    /*auto barbarian2 = Barbarian::create(base_map_, 1, Vec2(22, 22));
    if (!barbarian2)return false;
    barbarian2->takeDamage(25);
    base_map_->sprites_.push_back(barbarian2);*/

    /*auto archer = Archer::create(base_map_, 1, Vec2(22, 22));
    if (!archer)return false;
    archer->takeDamage(20);
    base_map_->sprites_.push_back(archer);*/

    /*auto giant = Giant::create(base_map_, 1, Vec2(22, 22));
    if (!giant)return false;
    giant->setDead();
    giant->takeDamage(200);
    base_map_->sprites_.push_back(giant);*/

    //// 创建一个角色 Sprite
    // auto barbarian_sprite = Sprite::create("Barbarian.png");
    // if (!barbarian_sprite) {
    //     return false;
    // }
    //// 将锚点设置为底部中心
    // barbarian_sprite->setAnchorPoint(Vec2(0.5, 0));
    // barbarian_sprite->setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(0, 0)));
    //// 这个 base_map_ 从 Village 基类继承来
    // base_map_->sprites_.push_back(barbarian_sprite);
    // base_map_->addChild(barbarian_sprite, 2);

    // 创建UI层（固定UI层）
    ui_layer_ = UIBars::create();
    if (!ui_layer_) {
        return false;
    }
    // UI层直接添加到场景，不受base_map变换影响
    this->addChild(ui_layer_, 200);  // 较高的z-order，确保UI显示在最上层且固定

    // 获取屏幕尺寸
    auto visible_size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto shop_button = cocos2d::ui::Button::create("shop.png", "shopSelected.png", "shopDisabled.png");
    // 商店标签
    shop_button->setTitleText("商店");
    shop_button->setTitleAlignment(TextHAlignment::LEFT, TextVAlignment::BOTTOM);  // 居中
    shop_button->setTitleFontSize(50);

    // 商店图标
    shop_button->setPosition(Vec2(visible_size.width - 100, 90));
    shop_button->setScale(0.5f);
    shop_button->setContentSize(Size(300, 300));  // 设置足够的触摸区域
    shop_button->setTouchEnabled(true);
    shop_button->setEnabled(true);

    shop_button->addTouchEventListener([this](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
            this->onShopButtonClick(sender);
        }
    });
    this->addChild(shop_button, 300);

    // 实验室：兵种升级图标
    auto lab_button = cocos2d::ui::Button::create("Laboratory.webp");
    lab_button->setPosition(Vec2(visible_size.width - 100, 250));
    lab_button->setScale(0.5f);
    lab_button->setContentSize(Size(300, 300));
    lab_button->setTouchEnabled(true);
    lab_button->setEnabled(true);
    // 实验室标签
    lab_button->setTitleText("实验室");
    lab_button->setTitleAlignment(TextHAlignment::LEFT, TextVAlignment::BOTTOM);
    lab_button->setTitleFontSize(50);

    lab_button->addTouchEventListener([this](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
            this->onLabButtonClick(sender);
        }
    });
    this->addChild(lab_button, 200);

    auto attack_button = cocos2d::ui::Button::create("attack.png");
    // 进攻图标
    attack_button->setPosition(Vec2(100, 100));
    attack_button->setScale(0.9f);
    attack_button->setContentSize(Size(300, 300));  // 设置足够的触摸区域
    attack_button->setTouchEnabled(true);
    attack_button->setEnabled(true);

    attack_button->addTouchEventListener([this](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
            this->onAttackButtonClick(sender);
        }
    });
    this->addChild(attack_button, 200);

    // 兵种配置图标
    auto troop_config_button = cocos2d::ui::Button::create("TroopConfig.png");
    troop_config_button->setPosition(Vec2(80, 230));
    troop_config_button->setScale(0.9f);
    troop_config_button->setContentSize(Size(300, 300));
    troop_config_button->setTouchEnabled(true);
    troop_config_button->setEnabled(true);

    troop_config_button->addTouchEventListener([this](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
            this->onTroopButtonClick(sender);
        }
    });
    this->addChild(troop_config_button, 200);

    // 消息/回放图标
    auto message_button = cocos2d::ui::Button::create("Message.png");
    message_button->setPosition(Vec2(80, 330));
    message_button->setScale(0.9f);
    message_button->setContentSize(Size(300, 300));
    message_button->setTouchEnabled(true);
    message_button->setEnabled(true);

    message_button->addTouchEventListener([this](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
            this->onMessageButtonClick(sender);
        }
    });
    this->addChild(message_button, 200);

    /*auto replayItem = cocos2d::MenuItemLabel::create(
        cocos2d::Label::createWithSystemFont("回放战斗", "Arial", 72),
        CC_CALLBACK_1(MainVillage::onReplayButtonClick, this));
    replayItem->setPosition(cocos2d::Vec2(80,300));
    this->addChild(replayItem,200);
    */
    return true;
}

void MainVillage::onEnter()
{
    AudioEngine::resume(mainhome_bgm_);
    // auto currentScene = Director::getInstance()->getRunningScene();
    // addLoadingLayerToCurrentScene(currentScene, 1.5f);
    if (last_exit_time_ > 0) {
        time_t current_time =
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        time_t time_diff = current_time - last_exit_time_;

        if (time_diff > 0) {
            for (auto arch : base_map_->archs_) {
                arch->updateUpgradeTime(time_diff);
            }
        }
        last_exit_time_ = 0;
    }

    Village::onEnter();

    //// 让角色动
    // auto move_by1 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(44, 0)));
    // auto move_by2 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(0, 44)));
    // auto move_by3 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(-44, 0)));
    // auto move_by4 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(0, -44)));
    // auto seq_action = Sequence::create(move_by1, move_by2, move_by3, move_by4, nullptr);

    // float moveDuration = 5.0f;  // 移动所需时间（秒）
    // float delayTime = 2.0f;  // 停顿时间（秒）
    //// 构建动作序列
    // auto moveToMiddle1 = MoveTo::create(moveDuration, CoordAdaptor::cellToPixel(base_map_,Vec2(22.1,22-0.3)));
    // auto delay1 = DelayTime::create(delayTime);
    // auto moveToBottom = MoveTo::create(moveDuration, CoordAdaptor::cellToPixel(base_map_, Vec2(0.1, 44-0.3)));
    // auto backToMiddle1 = MoveTo::create(moveDuration, CoordAdaptor::cellToPixel(base_map_, Vec2(22.1, 22-0.3)));
    // auto delay2 = DelayTime::create(delayTime);
    // auto backToTop = MoveTo::create(moveDuration, CoordAdaptor::cellToPixel(base_map_, Vec2(44.1, -0.3)));
    //// 将所有动作串成一个 Sequence
    // auto seq_action = Sequence::create(
    //     moveToMiddle1,
    //     delay1,
    //     moveToBottom,
    //     backToMiddle1,
    //     delay2,
    //     backToTop,
    //     nullptr  // 必须以 nullptr 结尾
    //);
    // auto repeatAction = RepeatForever::create(seq_action);
    // base_map_->sprites_.back()->runAction(repeatAction);
}

void MainVillage::cleanup()
{
    // 保存当前资源数据和建筑数据
    std::vector<ArchData> arch_list;
    for (auto a : base_map_->archs_) {
        arch_list.push_back(ArchData(a));
    }
    DataHelper::listToMap(arch_list, arch_status_);
    DataHelper::writeSourceData(kSourceDataFile, ResourceManager::getInstance()->getGold(),
                                ResourceManager::getInstance()->getElixir(), ResourceManager::getInstance()->getJewel());
    DataHelper::writeArchData(
        kMainVillageDataFile,
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(),
        arch_status_);
    DataHelper::writeLevelData(kOfflineDataFile[0], CocManager::getInstance()->level_info_list_);
    Village::cleanup();
}

void MainVillage::onAttackButtonClick(Ref* sender)
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
    auto u = UICommonHelper::create();
    // stop music.
    cocos2d::AudioEngine::pause(mainhome_bgm_);
    u->showChallengeSelectionPanel(this);
}

void MainVillage::onTroopButtonClick(Ref* sender)
{  // 播放音效
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

    auto panel = cocos2d::LayerColor::create(cocos2d::Color4B(130, 130, 190, 255));
    addChild(panel, 99999);

    // 吞噬触摸事件，防止点击穿透
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, panel);

    Size visible_size = Director::getInstance()->getVisibleSize();

    // 面板标题
    auto title_label = cocos2d::Label::createWithSystemFont("兵种配置", "Arial", 56);
    title_label->setPosition(cocos2d::Vec2(visible_size.width / 2, visible_size.height - 50));
    panel->addChild(title_label, 1);

    // 退出按钮
    auto exit_button = cocos2d::ui::Button::create("attack_scene/exit.png");
    exit_button->setPosition(cocos2d::Vec2(200, 100));
    exit_button->setScale(0.8f);
    exit_button->addClickEventListener([panel](cocos2d::Ref* sender) { panel->removeFromParent(); });
    panel->addChild(exit_button);

    // 人口容量显示
    auto capacity_label = cocos2d::Label::createWithSystemFont("", "Arial", 36);
    capacity_label->setPosition(cocos2d::Vec2(visible_size.width / 2, 100));
    panel->addChild(capacity_label);

    // 确保TroopConfig有容量，如果没有则设置一个默认值
    if (TroopConfig::getInstance()->getArmyCampCapacity() == 0) {
        TroopConfig::getInstance()->setArmyCampCapacity(200);  // 默认200
    }

    // 更新人口显示的lambda
    auto update_capacity_label = [capacity_label]() {
        unsigned int current = 0;
        for (auto it : kTroopTypes) {
            current += TroopConfig::getInstance()->getTroopCount(it) * kNoHousingSpace.at(it);
        }
        unsigned int max = TroopConfig::getInstance()->getArmyCampCapacity();
        capacity_label->setString(StringUtils::format("人口: %d / %d", current, max));
    };

    update_capacity_label();

    // 兵种网格布局
    const float kGapX = 350.0f;
    const float kGapY = 350.0f;
    const float kIconSize = 200.0f;
    const int kCols = 3;
    const float kStartX = (visible_size.width - kGapX * (kCols - 1)) / 2;
    const float kStartY = visible_size.height - 300;

    int i = 0;
    for (const unsigned char kIt : kTroopTypes) {
        int row = i / kCols;
        int col = i % kCols;
        float x = kStartX + col * kGapX;
        float y = kStartY - row * kGapY;

        // 兵种图标
        std::string icon_path = kIconPaths.at(kIt);
        auto icon = Sprite::create(icon_path);
        if (icon) {
            icon->setPosition(Vec2(x, y));
            Size content_size = icon->getContentSize();
            float scale = std::min(kIconSize / content_size.width, kIconSize / content_size.height);
            icon->setScale(scale);
            panel->addChild(icon);
        }

        // 兵种名称
        auto name_label = Label::createWithSystemFont(Troop::getTroopNameFromEnum(kIt), "Arial", 32);
        name_label->setPosition(Vec2(x, y + 150));
        panel->addChild(name_label);
        // 兵种等级
        auto level_label = Label::createWithSystemFont(
            std::to_string(TroopConfig::getInstance()->getTroopLevel(kIt)) + "级", "Arial", 32);
        level_label->setPosition(Vec2(x - 120, y + 150));
        panel->addChild(level_label);

        // 人口占用提示
        auto space_label =
            Label::createWithSystemFont(StringUtils::format("人口占用: %d", kNoHousingSpace.at(kIt)), "Arial", 24);
        space_label->setPosition(Vec2(x, y + 120));
        panel->addChild(space_label);

        // 检查兵种是否解锁
        bool is_unlocked = i <= TroopConfig::getInstance()->getUnlockedTroopIndex();
        int required_level = kBarracksTroopUnlock.at(kIt);

        if (!is_unlocked) {
            icon->setColor(Color3B::GRAY);
            auto lock_label =
                Label::createWithSystemFont(StringUtils::format("需训练营Lv.%d", required_level), "Arial", 36);
            lock_label->setPosition(Vec2(x, y - 130));
            lock_label->setColor(Color3B::RED);
            panel->addChild(lock_label);
        }
        else {
            // 数量显示
            auto count_label = Label::createWithSystemFont(
                StringUtils::format("%d", TroopConfig::getInstance()->getTroopCount(kIt)), "Arial", 40);
            count_label->setPosition(Vec2(x, y - 130));
            panel->addChild(count_label);

            // 创建按钮 (- / +)
            auto create_btn = [&](const std::string& text, float offset_x) {
                auto btn = ui::Button::create();
                btn->setTitleText(text);
                btn->setTitleFontSize(60);
                btn->setTitleColor(Color3B::WHITE);
                btn->setPosition(Vec2(x + offset_x, y - 130));
                panel->addChild(btn);
                return btn;
            };

            auto minus_button = create_btn("-", -80);
            auto plus_button = create_btn("+", 80);

            // 按钮点击事件
            minus_button->addClickEventListener([kIt, count_label, update_capacity_label](Ref*) {
                int count = TroopConfig::getInstance()->getTroopCount(kIt);
                if (count > 0) {
                    TroopConfig::getInstance()->setTroopCount(kIt, count - 1);
                    count_label->setString(StringUtils::format("%d", count - 1));
                    update_capacity_label();
                }
            });

            plus_button->addClickEventListener([kIt, count_label, update_capacity_label](Ref*) {
                int count = TroopConfig::getInstance()->getTroopCount(kIt);
                int space = kNoHousingSpace.at(kIt);

                // 检查容量
                unsigned int current_total = 0;
                for (const unsigned char k_ : kTroopTypes) {
                    current_total += TroopConfig::getInstance()->getTroopCount(k_) * kNoHousingSpace.at(k_);
                }
                unsigned int max = TroopConfig::getInstance()->getArmyCampCapacity();

                if (current_total + space <= max) {
                    TroopConfig::getInstance()->setTroopCount(kIt, count + 1);
                    count_label->setString(StringUtils::format("%d", count + 1));
                    update_capacity_label();
                }
                else {
                    // 容量不足提示
                    auto seq = Sequence::create(ScaleTo::create(0.1f, 1.5f), ScaleTo::create(0.1f, 1.0f), nullptr);
                    count_label->runAction(seq);
                }
            });
        }
        i++;
    }
}

void MainVillage::onLabButtonClick(Ref* sender)
{  // 播放音效
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
    Size visible_size = Director::getInstance()->getVisibleSize();
    // 添加半透明黑色背景
    auto color_bg = LayerColor::create(Color4B(0, 0, 0, 180));  // 深色半透明
    color_bg->setContentSize(visible_size);
    addChild(color_bg, 99999);

    auto panel = LayerColor::create(Color4B(255, 204, 153, 255), visible_size.width * 0.8f, visible_size.height * 0.8f);
    panel->setPosition(Vec2(visible_size.width * 0.1f, visible_size.height * 0.1f));
    color_bg->addChild(panel);

    // 吞噬触摸事件，防止点击穿透
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, panel);

    // 面板标题
    auto title_label = cocos2d::Label::createWithSystemFont("兵种升级", "Arial", 56);
    title_label->setPosition(cocos2d::Vec2(panel->getContentSize().width / 2, panel->getContentSize().height - 50));
    panel->addChild(title_label, 1);

    // 退出按钮
    auto exit_button = cocos2d::ui::Button::create("attack_scene/exit.png");
    exit_button->setPosition(cocos2d::Vec2(50, 50));
    exit_button->setScale(0.4f);
    exit_button->addClickEventListener([color_bg](cocos2d::Ref* sender) { color_bg->removeFromParent(); });
    panel->addChild(exit_button);

    // 兵种网格布局
    const float kGapX = 400.0f;
    const float kGapY = 380.0f;
    const float kIconSize = 180.0f;
    const int kCols = 3;
    const float kStartX = (panel->getContentSize().width - kGapX * (kCols - 1)) / 2;
    const float kStartY = panel->getContentSize().height - 280;

    int i = 0;

    for (const unsigned char kIt : kTroopTypes) {
        int row = i / kCols;
        int col = i % kCols;
        float x = kStartX + col * kGapX;
        float y = kStartY - row * kGapY;

        // 兵种图标
        std::string icon_path = kIconPaths.at(kIt);
        // 兵种背景
        auto troop_btn_bg = LayerColor::create(Color4B(255, 255, 255, 255), 180, 180);
        troop_btn_bg->setPosition(Vec2(x - 100, y - 80));
        drawBorder(troop_btn_bg, 3.0f, cocos2d::Color4F::BLACK);
        panel->addChild(troop_btn_bg);
        auto icon = Sprite::create(icon_path);
        if (icon) {
            icon->setPosition(Vec2(90, 90));
            Size content_size = icon->getContentSize();
            float scale = std::min(kIconSize / content_size.width, kIconSize / content_size.height);
            icon->setScale(scale);
            troop_btn_bg->addChild(icon, 1);
        }
        int level = TroopConfig::getInstance()->getTroopLevel(kIt);
        // 兵种名称
        auto name_label = Label::createWithSystemFont(Troop::getTroopNameFromEnum(kIt), "Arial", 32);
        name_label->setPosition(Vec2(x, y + 150));
        panel->addChild(name_label);
        // 兵种等级
        auto level_label = Label::createWithSystemFont("当前等级：" + std::to_string(level) + "级", "Arial", 32);
        level_label->setPosition(Vec2(x, y - 120));
        level_label->setName(Troop::getTroopNameFromEnum(kIt) + "level_name");
        panel->addChild(level_label);
        // 升级按钮背景
        auto upgrade_btn_bg = LayerColor::create(Color4B(255, 255, 255, 255), 80, 40);
        upgrade_btn_bg->setPosition(Vec2(x - 40, y - 190));
        upgrade_btn_bg->setName(Troop::getTroopNameFromEnum(kIt) + "upbg");
        drawBorder(upgrade_btn_bg, 2.0f, cocos2d::Color4F::BLACK);
        panel->addChild(upgrade_btn_bg);

        // 升级按钮
        auto upgrade_btn = Button::create();
        upgrade_btn->setTitleText("升级");
        upgrade_btn->setName(Troop::getTroopNameFromEnum(kIt) + "up");
        upgrade_btn->setTitleColor(Color3B(0, 0, 0));
        upgrade_btn->setTitleFontSize(20);
        upgrade_btn->setContentSize(Size(80, 40));
        upgrade_btn->setPosition(Vec2(x, y - 170));
        upgrade_btn->addTouchEventListener(CC_CALLBACK_2(MainVillage::onTroopUpradeClick, this, kIt, panel));
        panel->addChild(upgrade_btn, 9999);
        // 检查兵种是否解锁
        bool is_unlocked = i <= TroopConfig::getInstance()->getUnlockedTroopIndex();
        int required_level = kBarracksTroopUnlock.at(kIt);

        // 获取金币
        unsigned long long current_gold = ResourceManager::getInstance()->getGold();
        unsigned long long current_elixir = ResourceManager::getInstance()->getElixir();

        // 商品价格图标
        auto gold_icon = Sprite::create("Elixir.png");
        gold_icon->setPosition(Vec2(x + 70, y - 170));
        gold_icon->setScale(0.5f);
        panel->addChild(gold_icon);

        // 商品价格
        int p = 0;
        if (i == 0)
            p = Barbarian::kResearchCosts[level + 1];
        else if (i == 1)
            p = Archer::kResearchCosts[level + 1];
        else if (i == 2)
            p = Giant::kResearchCosts[level + 1];
        else if (i == 3)
            p = WallBreaker::kResearchCosts[level + 1];
        else if (i == 4)
            p = Balloon::kResearchCosts[level + 1];
        else if (i == 5)
            p = Dragon::kResearchCosts[level + 1];

        auto price_label = Label::createWithSystemFont("$" + std::to_string(p), "Arial", 25);
        price_label->setAnchorPoint(Vec2(0.0f, 0.5f));
        price_label->setPosition(Vec2(x + 100, y - 170));
        price_label->setColor(Color3B::MAGENTA);
        panel->addChild(price_label);

        if (!is_unlocked) {
            icon->setColor(Color3B::GRAY);
            auto lock_label =
                Label::createWithSystemFont(StringUtils::format("需训练营Lv.%d", required_level), "Arial", 30);
            lock_label->setPosition(Vec2(x - 5, y + 45));
            lock_label->setColor(Color3B::RED);
            panel->addChild(lock_label);
            upgrade_btn->setEnabled(false);
            upgrade_btn_bg->setColor(Color3B::GRAY);
            troop_btn_bg->setColor(Color3B::GRAY);
        }
        else {
            troop_btn_bg->setColor(Color3B::WHITE);
            bool can_afford = false;
            can_afford = current_elixir >= p;
            if (!can_afford) {
                price_label->setColor(Color3B::RED);
                upgrade_btn_bg->setColor(Color3B::GRAY);
                upgrade_btn->setEnabled(false);
            }

            if (TroopConfig::getInstance()->getTroopLevel(kIt) >= MAX_TROOP_LEVEL) {
                upgrade_btn_bg->setColor(Color3B::GRAY);
                upgrade_btn->setEnabled(false);
            }
            else if (can_afford) {
                price_label->setColor(Color3B::MAGENTA);
                upgrade_btn_bg->setColor(Color3B::WHITE);
                upgrade_btn->setEnabled(true);
            }
        }
        i++;
    }
}
void MainVillage::onTroopUpradeClick(Ref* sender, Widget::TouchEventType type, unsigned char& it,
                                     cocos2d::LayerColor* panel)
{
    if (type == Widget::TouchEventType::ENDED) {
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

        // 获取金币
        unsigned long long current_elixir = ResourceManager::getInstance()->getElixir();
        // 升级
        TroopConfig::getInstance()->upgradeTroopLevel(it);

        // 商品价格
        int p = 0;

        // 当前等级
        int l = TroopConfig::getInstance()->getTroopLevel(it);
        if (it == Troop::BARBARIAN)
            p = Barbarian::kResearchCosts[l];
        else if (it == Troop::ARCHER)
            p = Archer::kResearchCosts[l];
        else if (it == Troop::GIANT)
            p = Giant::kResearchCosts[l];
        else if (it == Troop::WALL_BREAKER)
            p = WallBreaker::kResearchCosts[l];
        else if (it == Troop::BALLOON)
            p = Balloon::kResearchCosts[l];
        else if (it == Troop::DRAGON)
            p = Dragon::kResearchCosts[l];

        ResourceManager::getInstance()->setElixir(current_elixir - l);
        current_elixir = ResourceManager::getInstance()->getElixir();
        auto label = dynamic_cast<Label*>(panel->getChildByName(Troop::getTroopNameFromEnum(it) + "level_name"));
        label->setString("当前等级：" + std::to_string(l) + "级");

        if (l < MAX_TROOP_LEVEL) {
            if (it == Troop::BARBARIAN)
                p = Barbarian::kResearchCosts[l + 1];
            else if (it == Troop::ARCHER)
                p = Archer::kResearchCosts[l + 1];
            else if (it == Troop::GIANT)
                p = Giant::kResearchCosts[l + 1];
            else if (it == Troop::WALL_BREAKER)
                p = WallBreaker::kResearchCosts[l + 1];
            else if (it == Troop::BALLOON)
                p = Balloon::kResearchCosts[l + 1];
            else if (it == Troop::DRAGON)
                p = Dragon::kResearchCosts[l + 1];
        }
        // 检查是否已达最高级
        if (l == MAX_TROOP_LEVEL || (current_elixir < p)) {
            // 已经是最高级，不需要升级
            auto bg = dynamic_cast<Layer*>(panel->getChildByName(Troop::getTroopNameFromEnum(it) + "upbg"));
            bg->setColor(cocos2d::Color3B::GRAY);
            auto upbtn = dynamic_cast<Button*>(panel->getChildByName(Troop::getTroopNameFromEnum(it) + "up"));
            upbtn->setEnabled(false);
            return;
        }
    }
}
void MainVillage::onShopButtonClick(Ref* sender)
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
    // 避免重复打开
    if (this->getChildByTag(100)) {
        return;
    }
    auto popup = ShopPopup::create();
    if (popup) {
        popup->setTag(100);  // 设置 tag 便于查找
        popup->setLocalZOrder(9999);
        popup->show(this);
        CCLOG("商店弹窗打开成功");
    }
    else {
        CCLOG("错误：无法创建商店弹窗");
    }
}

// 通过商店新建建筑并加入存档 + 加入场景
bool MainVillage::addBuildingByNO(unsigned char no, int price)
{
    // 默认新建筑等级为1
    unsigned char level = 1;

    // 校验no是否存在
    if (kArchInfo.find(no) == kArchInfo.end()) {
        CCLOG("建筑编号不存在:%d", no);
        return false;
    }

    // 读取ArchInfo默认资源
    auto& info = kArchInfo.at(no)[level - 1];

    // 构建建筑数据
    ArchData data;
    data.no_ = no;
    data.level_ = level;
    data.x_ = kMapSize / 2;  // 默认左下角
    data.y_ = kMapSize / 2;
    data.remaining_upgrade_time_ = 0;
    data.current_hp_ = info.hp_;
    data.current_capacity_ = info.max_capacity_;  // 资源建筑容量

    // 创建建筑到地图
    auto arch = Arch::create(data, base_map_);
    if (!arch) return false;
    base_map_->archs_.push_back(arch);

    // 展示取消按钮和确认按钮
    createCancelButton(arch);
    createConfirmButton(arch, price, kArchInfo.at(no)[level - 1].upgrade_cost_type_);

    // 创建建筑预览（建筑会显示在默认位置）
    arch->setOpacity(150);      // 设置为半透明
    arch->setLocalZOrder(900);  // 设置在最上层

    // 立即显示放置提示和按钮
    return true;
}

void MainVillage::createCancelButton(Arch* pendingArch_)
{
    // 创建取消按钮

    auto cancel_button = ui::Button::create();
    cancel_button->setTitleText("取消放置");
    cancel_button->setTitleFontSize(24);
    cancel_button->setTitleColor(Color3B::WHITE);
    cancel_button->setContentSize(Size(300, 200));

    // 创建一个纯色背景
    auto button_bg = LayerColor::create(Color4B(200, 50, 50, 200));  // 纯红色背景，透明度255
    button_bg->setContentSize(cancel_button->getContentSize());       // 设置背景大小与按钮大小一致
    button_bg->setPosition(Vec2(0, 0));                              // 背景位置设置为按钮的位置
    button_bg->setName("CANCEL_BUTTONBG");

    // 将背景添加到按钮
    cancel_button->addChild(button_bg, -1);  // -1 确保背景在按钮下面

    cancel_button->setPosition(Vec2(10, 10));
    cancel_button->setColor(Color3B::BLACK);
    cancel_button->setName("CANCEL_BUTTON");

    cancel_button->addTouchEventListener([this, pendingArch_](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->cancelBuildingPlacement(pendingArch_);
        }
    });

    pendingArch_->addChild(cancel_button, 900);
}

void MainVillage::createConfirmButton(Arch* pendingArch_, int price, bool type_)
{
    // 创建确认按钮
    auto confirm_button = ui::Button::create();
    confirm_button->setTitleText("确认放置");
    confirm_button->setTitleFontSize(24);
    confirm_button->setTitleColor(Color3B::WHITE);
    confirm_button->setContentSize(Size(300, 200));
    // 创建一个纯色背景
    auto button_bg = LayerColor::create(Color4B(50, 200, 50, 200));  // 纯红色背景，透明度255
    button_bg->setContentSize(confirm_button->getContentSize());      // 设置背景大小与按钮大小一致
    button_bg->setPosition(Vec2(0, 0));                              // 背景位置设置为按钮的位置
    button_bg->setName("CONFIRML_BUTTONBG");
    // 将背景添加到按钮
    confirm_button->addChild(button_bg, -1);  // -1 确保背景在按钮下面
    confirm_button->setPosition(Vec2(200, 10));
    confirm_button->setColor(Color3B::BLACK);
    confirm_button->setName("CONFIRM_BUTTON");
    unsigned long long current_gold = ResourceManager::getInstance()->getGold();
    unsigned long long current_exilir = ResourceManager::getInstance()->getElixir();
    confirm_button->addTouchEventListener(
        [this, pendingArch_, current_gold, current_exilir, price, type_](Ref* sender, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::ENDED) {
                this->confirmBuildingPlacement(pendingArch_);
                if (type_ == GOLD) {
                    pendingArch_->buidingUpgrading(sender, pendingArch_, NEW_BUIDING, price, current_gold, type_);
                }
                else {
                    pendingArch_->buidingUpgrading(sender, pendingArch_, NEW_BUIDING, price, current_exilir, type_);
                }
            }
        });

    pendingArch_->addChild(confirm_button, 900);
}

void MainVillage::cancelBuildingPlacement(Arch* pendingArch_)
{
    if (!pendingArch_) return;
    // 从base_map_->archs_中移除
    auto& arr = base_map_->archs_;
    arr.erase(std::remove(arr.begin(), arr.end(), pendingArch_), arr.end());
    // 移除放置相关按钮
    removeCancelAndConfirmButtons(pendingArch_);
    // 删除已创建的建筑
    pendingArch_->removeFromParent();
    pendingArch_ = nullptr;

    // 显示提示并返回商店面板
    CCLOG("建筑放置已取消，返回商店面板");
    showShopPopupWithDelay(1.0f);
}

void MainVillage::confirmBuildingPlacement(Arch* pendingArch_)
{
    if (!pendingArch_) return;
    // 清理UI相关的按钮
    removeCancelAndConfirmButtons(pendingArch_);
    // 清理拖动状态
    pendingArch_->is_dragging_ = false;
    // 使建筑恢复正常状态
    pendingArch_->setOpacity(255);    // 恢复透明度
    pendingArch_->setLocalZOrder(2);  // 恢复正常层级

    // 播放建筑落地效果
    playBuildingDropEffect(pendingArch_);
    // 将建筑加入存档数据结构
    arch_status_[pendingArch_->getx()][pendingArch_->gety()] = ArchData(pendingArch_);
    CCLOG("建筑放置成功");
}

void MainVillage::removeCancelAndConfirmButtons(Arch* pendingArch_)
{
    pendingArch_->removeChildByName("CANCEL_BUTTON");
    pendingArch_->removeChildByName("CANCEL_BUTTONBG");
    pendingArch_->removeChildByName("CONFIRM_BUTTON");
    pendingArch_->removeChildByName("CONFIRM_BUTTONBG");
}

void MainVillage::playBuildingDropEffect(Arch* arch)
{
    if (!arch) return;

    // 创建发光效果，可以使用淡入淡出的效果
    auto fade_in = FadeTo::create(0.5f, 255);  // 使建筑恢复透明度

    // 添加光晕效果（模拟发光）
    auto glow_effect = cocos2d::Sprite::create();  // 创建一个光晕精灵
    glow_effect->setTexture("flash.png");
    glow_effect->setScale(0.1f);
    glow_effect->setOpacity(255);                                                       // 初始透明度较低
    glow_effect->setPosition(Vec2(arch->getPosition().x, arch->getPosition().y - 50));  // 设置光晕的位置与建筑相同

    arch->getParent()->addChild(glow_effect, arch->getLocalZOrder() - 1);  // 将光晕放到建筑的下面

    // 创建光晕的扩散效果
    auto glow_scale_up = ScaleTo::create(0.3f, 0.4f);  // 光晕变大
    auto glow_fade_in = FadeTo::create(0.3f, 0);       // 光晕逐渐变暗

    // 在动画结束时移除光晕
    auto remove_glow = CallFunc::create([glow_effect]() {
        glow_effect->removeFromParent();  // 移除光晕精灵
    });
    auto glow_sequence = Sequence::create(glow_scale_up, glow_fade_in, remove_glow, fade_in, nullptr);
    glow_effect->runAction(glow_sequence);
}

void MainVillage::showShopPopupWithDelay(float sec)
{
    // 创建商店面板的延迟回调函数
    this->scheduleOnce(
        [this](float dt) {
            // 创建商店面板
            auto shop_popup = ShopPopup::create();  // 创建商店面板
            if (shop_popup) {
                shop_popup->setGlobalZOrder(9999);  // 确保商店面板显示在最上层
                shop_popup->show(this);             // 将商店面板显示到当前场景（this 即为当前场景）
            }
        },
        sec, "show_shop_popup_key");  // 延迟 2 秒调用
}

void MainVillage::onExit()
{
    last_exit_time_ =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    Village::onExit();
}

unsigned char MainVillage::getTownHallLevel()
{
    for (auto arch : base_map_->archs_) {
        if (arch->getNo() == TOWN_HALL) {
            return arch->getLevel();
        }
    }
    assert(false && "主村庄中无大本营");
    return 1;  // 这里不应该被触发
}

void MainVillage::onMessageButtonClick(Ref* sender)
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
    auto u = UICommonHelper::create();
    u->showReplayPanel(this);
}

int MainVillage::getBuildingCount(unsigned char archNo)
{
    int count = 0;
    for (auto arch : base_map_->archs_) {
        if (arch->getNo() == archNo) {
            count++;
        }
    }
    return count;
}

// void MainVillage::onReplayButtonClick(cocos2d::Ref* sender,int gold_,int elixir_,bool isReplay) {
//
//     CocManager::getInstance()->changeScene(1, gold_, elixir_);
// }
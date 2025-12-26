#include "EnemyVillageScene.h"

#include <algorithm>
#include <set>
#include <utility>

#include "Arch.h"
#include "Archer.h"
#include "ArchTargetManager.h"
#include "AttackStars.h"
#include "AudioEngine.h"
#include "Balloon.h"
#include "Barbarian.h"
#include "CocManager.h"
#include "CocUtility.h"
#include "Dragon.h"
#include "Giant.h"
#include "MainVillageScene.h"
#include "TroopTargetManager.h"
#include "UIcommon.h"
#include "WallBreaker.h"
USING_NS_CC;

EnemyVillage* EnemyVillage::create(int level)
{
    EnemyVillage* p_ret = new (std::nothrow) EnemyVillage();
    if (p_ret && p_ret->myInit(level)) {
        p_ret->autorelease();
        return p_ret;
    }
    else {
        delete p_ret;
        p_ret = nullptr;
        return nullptr;
    }
};

bool EnemyVillage::myInit(int level)
{
    if (!Village::init()) {
        return false;
    }

    // 从数据文件中读取建筑数据并创建建筑对象
    time_t current_time =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    time_t data_time = 0;
    if (!DataHelper::readArchData(kOfflineDataFile[level], data_time, arch_status_)) {
        return false;
    }

    std::vector<ArchData> arch_list;
    DataHelper::mapToList(arch_status_, arch_list);
    TroopTargetManager::getInstance()->setlivingsum(0);
    Arch* p = nullptr;
    for (auto& arch : arch_list) {
        p = Arch::create(arch, base_map_, false);
        TroopTargetManager::getInstance()->registerTroopTarget(p);
        // 统计建筑总量
        int now_arch = TroopTargetManager::getInstance()->getlivingsum();
        if (arch.no_ != WALL && arch.no_ != BOMB) TroopTargetManager::getInstance()->setlivingsum(now_arch + 1);
    }
    // todo:把建筑生成也工厂化
    troop_factories_[Troop::BARBARIAN] = [](BaseMap* map, int lvl, cocos2d::Vec2 pos) {
        return Barbarian::create(map, lvl, pos);
    };
    troop_factories_[Troop::ARCHER] = [](BaseMap* map, int lvl, cocos2d::Vec2 pos) {
        return Archer::create(map, lvl, pos);
    };
    troop_factories_[Troop::GIANT] = [](BaseMap* map, int lvl, cocos2d::Vec2 pos) {
        return Giant::create(map, lvl, pos);
    };
    troop_factories_[Troop::WALL_BREAKER] = [](BaseMap* map, int lvl, cocos2d::Vec2 pos) {
        return WallBreaker::create(map, lvl, pos);
    };
    troop_factories_[Troop::BALLOON] = [](BaseMap* map, int lvl, cocos2d::Vec2 pos) {
        return Balloon::create(map, lvl, pos);
    };
    troop_factories_[Troop::DRAGON] = [](BaseMap* map, int lvl, cocos2d::Vec2 pos) {
        return Dragon::create(map, lvl, pos);
    };

    // 预计算所有建筑的距离场
    TroopTargetManager::getInstance()->precomputeDistanceFields();

    // 显示红色底色（我方不可下兵范围）
    // 包裹的范围是所有地方建筑向外延伸1格
    occupied_cells_.clear();
    for (auto arch : base_map_->archs_) {
        float size_f;
        arch->getCellPosition(size_f);
        int size = static_cast<int>(size_f);
        int x = static_cast<int>(arch->getx());
        int y = static_cast<int>(arch->gety());

        for (int i = x - 1; i < x + size + 1; ++i) {
            for (int j = y - 1; j < y + size + 1; ++j) {
                if (i >= 0 && i < kMapSize && j >= 0 && j < kMapSize) {
                    occupied_cells_.insert({i, j});
                }
            }
        }
    }

    auto red_layer = cocos2d::Node::create();
    base_map_->addChild(red_layer, 0);

    for (const auto& cell : occupied_cells_) {
        auto sprite = cocos2d::Sprite::create("SingleCellAlphaRed.png");
        if (sprite) {
            cocos2d::Vec2 pos =
                CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(cell.first + 0.5f, cell.second + 0.5f));
            sprite->setPosition(pos);
            red_layer->addChild(sprite);
        }
    }

    has_deployed_troop_ = false;
    current_replay_data_.level_ = level;
    current_replay_data_.timestamp_ = current_time;

    // 添加触摸监听器来检测玩家点击的位置
    auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
    touch_listener->onTouchBegan = CC_CALLBACK_2(EnemyVillage::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

    ui_layer_ = UIBars::create();
    if (!ui_layer_) {
        return false;
    }
    // UI层直接添加到场景，不受base_map变换影响
    this->addChild(ui_layer_, 200);  // 较高的z-order，确保UI显示在最上层且固定
    // 获取并修改金币
    unsigned long long current_gold = ResourceManager::getInstance()->getGold();
    unsigned long long current_elixir = ResourceManager::getInstance()->getElixir();
    ResourceManager::getInstance()->setGold(current_gold);

    auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
    auto origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    auto close_item = cocos2d::MenuItemLabel::create(cocos2d::Label::createWithSystemFont("退出", "Arial", 72),
                                                     CC_CALLBACK_1(EnemyVillage::onExitButtonClick, this));

    // auto replayItem = cocos2d::MenuItemLabel::create(
    //     cocos2d::Label::createWithSystemFont("回放战斗", "Arial",50),
    //     CC_CALLBACK_1(EnemyVillage::onReplayButtonClick, this, currentGold, currentElixir));

    float x = origin.x + visible_size.width - close_item->getContentSize().width / 2 - 20;
    float y = origin.y + close_item->getContentSize().height / 2 + 20;
    close_item->setPosition(cocos2d::Vec2(x, y));
    // replayItem->setPosition(cocos2d::Vec2(x, y + 150));

    auto menu = cocos2d::Menu::create(close_item, NULL);
    menu->setPosition(cocos2d::Vec2::ZERO);
    this->addChild(menu, 100);

    // 设置面板的背景
    auto bg_sprite =
        cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 150), visible_size.width, visible_size.height / 4);
    bg_sprite->setAnchorPoint(cocos2d::Vec2(0, 0));
    bg_sprite->setPosition(cocos2d::Vec2(0, 0));
    bg_sprite->setTag(999);
    this->addChild(bg_sprite, 10);
    // 创建兵种按钮
    createTroopSelectionPanel(bg_sprite);

    auto attacking_progress = AttackStars::create();
    this->addChild(attacking_progress, 100);

    selected_troop_type_ = 0;
    // UnitManager::getInstance()->clearUnits();
    return true;
}

void EnemyVillage::onExitButtonClick(cocos2d::Ref* sender)
{
    cocos2d::AudioEngine::stop(attacking_bgm_);

    bool is_replay = CocManager::getInstance()->isReplay();

    // 保存回放数据
    if (!is_replay && !current_replay_data_.deployments_.empty()) {
        DataHelper::addReplayData(kReplayDataFile, current_replay_data_);
    }

    if (is_replay) {
        CocManager::getInstance()->setReplay(false);
    }

    for (auto troop : troop_list_) {
        troop->setDead();
    }
    TroopTargetManager::getInstance()->clear();
    ArchTargetManager::getInstance()->clear();

    if (is_replay) {
        cocos2d::Director::getInstance()->popScene();
    }
    else {
        CocManager::getInstance()->changeScene();
        unsigned long long current_elixir = ResourceManager::getInstance()->getElixir();
        ResourceManager::getInstance()->setElixir(current_elixir);
    }
}

bool EnemyVillage::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    // if (ResourceManager::getInstance()->is_replay_) return false;
    cocos2d::Vec2 cell_pos = CoordAdaptor::pixelToCell(base_map_, base_map_->convertToNodeSpace(touch->getLocation()));

    // 边界检查
    if (cell_pos.x < 0 || cell_pos.x >= kMapSize || cell_pos.y < 0 || cell_pos.y >= kMapSize) {
        showInvalidSpawnMessage();
        return true;
    }

    // 检查触摸位置是否在红色区域（不可下兵区域）
    // 红色区域是基于整数格子坐标存储的
    int tx = static_cast<int>(std::floor(cell_pos.x));
    int ty = static_cast<int>(std::floor(cell_pos.y));

    if (occupied_cells_.count({tx, ty})) {
        showInvalidSpawnMessage();
        return true;
    }

    // 如果位置有效（不在红色区域），生成士兵
    int index = selected_troop_type_ - 1;
    if (index < 0) return true;

    int level = TroopConfig::getInstance()->getTroopLevel(kTroopTypes[index]);
    // 如果生成成功，更新计数
    if (spawnTroop(kTroopTypes[index], level, cell_pos)) {
        troop_placed_counts_[index]++;
        updateTroopCountLabel(index);

        // 如果达到上限，禁用按钮
        if (troop_placed_counts_[index] >= TroopConfig::getInstance()->getTroopCount(kTroopTypes[index])) {
            disableTroopButton(index);
            showInvalidSpawnMessage(Troop::getTroopNameFromEnum(kTroopTypes[index]) + "已全部放置完成");
        }
    }

    return true;  // 返回true表示已处理触摸事件
}

// 更新兵种数量标签
void EnemyVillage::updateTroopCountLabel(unsigned int index)
{
    if (index >= 0 && index < troop_count_labels_.size()) {
        auto label = troop_count_labels_[index];
        label->setString(std::to_string(troop_placed_counts_[index]) + "/" +
                         std::to_string(TroopConfig::getInstance()->getTroopCount(kTroopTypes[index])));
    }
}

// 禁用已满的兵种按钮
void EnemyVillage::disableTroopButton(unsigned int index)
{
    if (index >= 0 && index < troop_buttons_.size()) {
        auto button = troop_buttons_[index];
        button->setColor(cocos2d::Color3B(100, 100, 100));  // 灰色表示禁用
        auto label = troop_count_labels_[index];
        label->setColor(cocos2d::Color3B::RED);
        // 如果这个按钮当前被选中，取消选中
        if (selected_item_bg_ == button) {
            removeBorder(selected_item_bg_);
            selected_item_bg_ = nullptr;
            selected_troop_type_ = 0;
        }
    }
}

bool EnemyVillage::spawnTroop(unsigned char type, unsigned char lvl, cocos2d::Vec2 position)
{
    auto it = troop_factories_.find(type);
    if (it != troop_factories_.end()) {
        Troop* troop = it->second(base_map_, lvl, position);
        if (troop) {
            troop_list_.push_back(troop);
            ArchTargetManager::getInstance()->registerArchTarget(troop);

            // 记录回放数据
            if (!CocManager::getInstance()->isReplay()) {
                if (!has_deployed_troop_) {
                    first_deployment_time_ = std::chrono::steady_clock::now();
                    has_deployed_troop_ = true;
                }

                auto now = std::chrono::steady_clock::now();
                float time_diff = std::chrono::duration<float>(now - first_deployment_time_).count();

                DeploymentInfo info;
                info.type_ = type;
                info.time_ = time_diff;
                info.x_ = position.x;
                info.y_ = position.y;
                current_replay_data_.deployments_.push_back(info);
            }

            return true;
        }
    }
    return false;
}

void EnemyVillage::showInvalidSpawnMessage(std::string text)
{
    // 显示消息提示玩家不能在该位置生成士兵（可以使用弹窗或标签）
    auto label = cocos2d::Label::createWithSystemFont(text, "Arial", 36);
    label->setPosition(cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width / 2,
                                     cocos2d::Director::getInstance()->getVisibleSize().height / 2));
    this->addChild(label, 100);

    // 在短时间后隐藏消息
    auto delay = cocos2d::DelayTime::create(1.0f);
    auto remove = cocos2d::CallFunc::create([label]() { label->removeFromParent(); });
    auto sequence = cocos2d::Sequence::create(delay, remove, nullptr);
    label->runAction(sequence);
}

void EnemyVillage::createTroopSelectionPanel(cocos2d::LayerColor* bg)
{
    troop_placed_counts_ = {0, 0, 0, 0, 0, 0};  // 已放置数量（成员变量）

    auto panel_swallow_listener = cocos2d::EventListenerTouchOneByOne::create();
    panel_swallow_listener->setSwallowTouches(true);
    panel_swallow_listener->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event) {
        auto target = static_cast<cocos2d::Node*>(event->getCurrentTarget());
        cocos2d::Rect rect = target->getBoundingBox();
        if (rect.containsPoint(touch->getLocation())) {
            return true;
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(panel_swallow_listener, bg);

    // 创建按钮
    float button_width = 200;
    float button_height = 200;
    float padding = 60;

    // 清空之前的按钮数组
    troop_buttons_.clear();
    troop_count_labels_.clear();

    for (size_t i = 0; i < TROOP_TYPE_NUM; ++i) {
        // 商品背景
        auto item_bg = cocos2d::LayerColor::create(cocos2d::Color4B(140, 150, 200, 255), button_width, button_height);
        item_bg->setPosition(cocos2d::Vec2((button_width + padding) * i + 200, 30));

        // 保存按钮引用
        troop_buttons_.push_back(item_bg);

        float max_size = 180.0f;
        auto item_pic = cocos2d::Sprite::create(kIconPaths.at(kTroopTypes[i]));
        float scale =
            std::min(max_size / item_pic->getContentSize().width, max_size / item_pic->getContentSize().height);
        item_pic->setPosition(cocos2d::Vec2(button_width / 2, button_height / 2));
        item_pic->setScale(scale);

        // 显示数量标签（已放置/最大数量）
        auto count_label = cocos2d::Label::createWithSystemFont(
            std::to_string(troop_placed_counts_[i]) + "/" +
                std::to_string(TroopConfig::getInstance()->getTroopCount(kTroopTypes[i])),
            "Arial", 32);
        count_label->setPosition(cocos2d::Vec2(button_width / 2, button_height + 20));
        count_label->setTag(1000);  // 设置tag以便后续更新
        item_bg->addChild(count_label, 150);

        // 保存数量标签引用
        troop_count_labels_.push_back(count_label);

        // 将按钮添加到背景层
        item_bg->addChild(item_pic);
        bg->addChild(item_bg);

        if (TroopConfig::getInstance()->getTroopCount(kTroopTypes[i]) == 0) {
            disableTroopButton(i);
        }

        // 添加触摸事件监听器
        auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
        touch_listener->onTouchBegan = [this, item_bg, i](cocos2d::Touch* touch, cocos2d::Event* event) {
            cocos2d::Rect button_rect = item_bg->getBoundingBox();
            if (button_rect.containsPoint(touch->getLocation())) {
                this->onButtonClick(item_bg, i);
                return true;
            }
            return false;
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, item_bg);
    }
}
// 点击按钮时的处理函数
void EnemyVillage::onButtonClick(cocos2d::LayerColor* itemBg, int index)
{
    // bool is_c = ResourceManager::getInstance()->is_replay_;
    if (1) {
        // 如果点击的是同一个按钮，取消选中状态
        if (selected_item_bg_ == itemBg) {
            selected_item_bg_->setColor(cocos2d::Color3B(140, 150, 200));  // 恢复原始颜色
            removeBorder(selected_item_bg_);
            selected_item_bg_ = nullptr;
            return;  // 已经是选中的按钮，不做任何改变
        }
        // 如果已有按钮被选中，取消选中状态并恢复原始颜色
        if (selected_item_bg_) {
            selected_item_bg_->setColor(cocos2d::Color3B(140, 150, 200));  // 恢复原始颜色
            removeBorder(selected_item_bg_);
        }
        // 检查是否已达到上限
        if (index <= 5 &&
            troop_placed_counts_[index] >= TroopConfig::getInstance()->getTroopCount(kTroopTypes[index])) {
            showInvalidSpawnMessage(Troop::getTroopNameFromEnum(kTroopTypes[index]) + "已达到上限！");
            return;
        }
        // 更新当前选中的按钮
        selected_item_bg_ = itemBg;
        selected_troop_type_ = index + 1;
        // 更改选中按钮的颜色
        selected_item_bg_->setColor(cocos2d::Color3B(100, 100, 150));  // 变暗的颜色
        drawBorder(selected_item_bg_);
    }
}

EnemyVillage* EnemyVillage::createReplay(const ReplayData& data)
{
    EnemyVillage* p_ret = new (std::nothrow) EnemyVillage();
    if (p_ret && p_ret->myInit(data.level_)) {
        p_ret->current_replay_data_ = data;
        CocManager::getInstance()->setReplay(true);
        p_ret->startReplaySequence();
        p_ret->autorelease();
        return p_ret;
    }
    else {
        delete p_ret;
        p_ret = nullptr;
        return nullptr;
    }
}

void EnemyVillage::startReplaySequence()
{
    // 隐藏下兵面板
    auto bg = this->getChildByTag(999);
    if (bg) bg->setVisible(false);

    first_deployment_time_ = std::chrono::steady_clock::now();

    this->schedule(
        [this](float dt) {
            if (current_replay_data_.deployments_.empty()) {
                return;
            }

            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - first_deployment_time_).count();

            auto it = current_replay_data_.deployments_.begin();
            while (it != current_replay_data_.deployments_.end()) {
                if (elapsed >= it->time_) {
                    spawnTroop(it->type_, 1, cocos2d::Vec2(it->x_, it->y_));
                    it = current_replay_data_.deployments_.erase(it);
                }
                else {
                    break;
                }
            }
        },
        0.1f, "replay_update");
}

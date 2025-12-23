#include "EnemyVillageScene.h"
#include "DataHelper.h"
#include "MainVillageScene.h"
#include "CocController.h"
#include "TroopTargetManager.h"
#include "Arch.h"
#include "CoordAdaptor.h"
#include <set>
#include <utility>
#include "Barbarian.h"
#include "Archer.h"
#include "Giant.h"
#include "Balloon.h"
#include "Dragon.h"
#include"UIcommon.h"
#include "AudioEngine.h"
#include"AttackStars.h"
#include"ReplayAttack.h"
USING_NS_CC;
int selectedTroopType = 0;  // -1表示未选择任何兵种 todo: 最好不要全局变量

EnemyVillage* EnemyVillage::create(int level)
{
    EnemyVillage* pRet = new(std::nothrow) EnemyVillage(); if (pRet && pRet->myInit(level)) {
        pRet->autorelease(); return pRet;
    }
    else {
        delete pRet; pRet = nullptr; return nullptr;
    }
};

bool EnemyVillage::myInit(int level)
{
    if (!Village::init()) {
        return false;
    }
    
    // 从数据文件中读取建筑数据并创建建筑对象
    time_t current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
        //统计建筑总量
        int nowArch = TroopTargetManager::getInstance()->getlivingsum();
        if (arch.no_ != WALL)TroopTargetManager::getInstance()->setlivingsum(nowArch+1);
    }
    /*auto barbarian = Barbarian::create(base_map_, 1, cocos2d::Vec2(40, 20));
    if (!barbarian)return false;
	troop_list_.push_back(barbarian);*/

    /*auto dragon = Dragon::create(base_map_, 1, cocos2d::Vec2(30, 20));
    if (!dragon)return false;
    troop_list_.push_back(dragon);
*/
    auto balloon = Balloon::create(base_map_, 1, cocos2d::Vec2(10, 20));
    if (!balloon)return false;
    troop_list_.push_back(balloon);

    // 预计算所有建筑的距离场
    TroopTargetManager::getInstance()->precomputeDistanceFields();


    
    // 显示红色底色（我方不可下兵范围）
    // 包裹的范围是所有地方建筑向外延伸1格
    std::set<std::pair<int, int>> occupied_cells;
    for (auto arch : base_map_->archs_) {
        float size_f;
        arch->getCellPosition(size_f);
        int size = static_cast<int>(size_f);
        int x = static_cast<int>(arch->getx());
        int y = static_cast<int>(arch->gety());

        for (int i = x - 1; i < x + size + 1; ++i) {
            for (int j = y - 1; j < y + size + 1; ++j) {
                if (i >= 0 && i < MAP_SIZE && j >= 0 && j < MAP_SIZE) {
                    occupied_cells.insert({ i, j });
                }
            }
        }
    }

    auto red_layer = cocos2d::Node::create();
    base_map_->addChild(red_layer, 0);

    for (const auto& cell : occupied_cells) {
        auto sprite = cocos2d::Sprite::create("SingleCellAlphaRed.png");
        if (sprite) {
            cocos2d::Vec2 pos = CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(cell.first + 0.5f, cell.second + 0.5f));
            sprite->setPosition(pos);
            red_layer->addChild(sprite);
        }
    }





    auto barbarian2 = Barbarian::create(base_map_, 1, cocos2d::Vec2(0.5, 0.5));
    if (!barbarian2)return false;
    barbarian2->takeDamage(500);
    base_map_->sprites_.push_back(barbarian2);


    // 添加触摸监听器来检测玩家点击的位置
    auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(EnemyVillage::onTouchBegan, this, occupied_cells);
    //touchListener->onTouchMoved = CC_CALLBACK_2(EnemyVillage::onTouchMoved, this);
    //touchListener->onTouchEnded = CC_CALLBACK_2(EnemyVillage::onTouchEnded, this);
    // touchListener->onTouchCancelled = CC_CALLBACK_2(EnemyVillage::onTouchCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    ui_layer_ = UIBars::create();
    if (!ui_layer_) {
        return false;
    }
    // UI层直接添加到场景，不受base_map变换影响
    this->addChild(ui_layer_, 200);  // 较高的z-order，确保UI显示在最上层且固定
    // 获取并修改金币
    unsigned long long currentGold = GameManager::getInstance()->getGold();
    unsigned long long currentElixir = GameManager::getInstance()->getElixir();
    GameManager::getInstance()->setGold(currentGold );

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    auto origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    auto closeItem = cocos2d::MenuItemLabel::create(
        cocos2d::Label::createWithSystemFont("退出", "Arial", 72),
        CC_CALLBACK_1(EnemyVillage::onExitButtonClick, this));

    auto replayItem = cocos2d::MenuItemLabel::create(
        cocos2d::Label::createWithSystemFont("回放战斗", "Arial",50),
        CC_CALLBACK_1(EnemyVillage::onReplayButtonClick, this, currentGold, currentElixir));

    float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2 - 20;
    float y = origin.y + closeItem->getContentSize().height / 2 + 20;
    closeItem->setPosition(cocos2d::Vec2(x, y));
    replayItem->setPosition(cocos2d::Vec2(x, y + 150));

    auto menu = cocos2d::Menu::create(closeItem, replayItem, NULL);
    menu->setPosition(cocos2d::Vec2::ZERO);
    this->addChild(menu, 100);

    // 设置面板的背景
    auto bgSprite = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 150), visibleSize.width, visibleSize.height/4);
    bgSprite->setAnchorPoint(cocos2d::Vec2(0, 0));
    bgSprite->setPosition(cocos2d::Vec2(0, 0));
    this->addChild(bgSprite,10);
    // 创建兵种按钮
    createTroopSelectionPanel(bgSprite);

    auto Attacking_progress=AttackStars::create();
    this->addChild(Attacking_progress,100);
    //UnitManager::getInstance()->clearUnits();
    return true;
}

void EnemyVillage::onExitButtonClick(cocos2d::Ref* sender)
{
    cocos2d::AudioEngine::stop(attackingBGM);
    
    for(auto troop : troop_list_) {
        troop->setDead();
	}
    TroopTargetManager::getInstance()->clear();
    CocController::getInstance()->changeScene();

}

bool EnemyVillage::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event, std::set<std::pair<int, int>> occupied_cells)
{
    if (GameManager::getInstance()->isReplay)return false;
    // 获取触摸位置（屏幕坐标）
    cocos2d::Vec2 touchLocation = touch->getLocation();

    // 转换为本地坐标（考虑Y轴翻转）
    //touchLocation.y = cocos2d::Director::getInstance()->getWinSize().height - touchLocation.y;
    touchLocation = this->convertToNodeSpace(touchLocation); // 转换为当前节点的本地坐标

    // 获取当前BaseMap的缩放因子
    float scale = base_map_->getScale();  
    bool isValidLocation = true;

    // 将像素坐标转换为格子坐标
    touchLocation = CoordAdaptor::pixelToCell(base_map_, touchLocation);
    touchLocation.x = (touchLocation.x-10)*2;
    touchLocation.y =(touchLocation.y+20)*2;
    if (touchLocation.x < 0)touchLocation.x = 0;
    if (touchLocation.x > 44)touchLocation.x = 43.9;
    if (touchLocation.y < 0)touchLocation.y = 0;
    if (touchLocation.y > 44)touchLocation.y = 43.9;
    // 检查触摸位置是否在红色区域之外
    for (const auto& cell : occupied_cells) {
        // 将格子坐标转为像素坐标
        cocos2d::Vec2 cellPosition =cocos2d::Vec2(cell.first , cell.second );

        // 检查触摸位置是否接近该单元格
        if (touchLocation.distance(cellPosition) < 0.5) { // 如果触摸位置接近红色范围
            isValidLocation = false;  // 如果触摸位置在红色范围内，则不允许生成
            break;
        }
    }
    // 如果位置有效（不在红色区域），生成士兵
    if (isValidLocation) {
        int index = selectedTroopType - 1;
        if (index < 0)return 1;
        // 检查是否达到上限
        if (troopPlacedCounts_[index] >= troopMaxCounts_[index]) {
            showInvalidSpawnMessage(troopNames_[index] + "已达到上限！");
            return true;
        }

        // 生成士兵
        bool spawnSuccess = false;
        if (selectedItemBg) {
            if (selectedTroopType == 1) {
                spawnSuccess = spawnBarbarian(touchLocation);
                //在回放中记录
                UnitManager::getInstance()->addUnit(0, touchLocation);
            }
            else if (selectedTroopType == 2) {
                spawnSuccess = spawnArcher(touchLocation);
            }
            else if (selectedTroopType == 3) {
                spawnSuccess = spawnGiant(touchLocation);
            }
            else if (selectedTroopType == 4) {
                spawnSuccess = spawnBomb(touchLocation);
            }
            else if (selectedTroopType == 5) {
                spawnSuccess = spawnBalloon(touchLocation);
            }
            else if (selectedTroopType == 6) {
                spawnSuccess = spawnDragon(touchLocation);
            }
        }
        // 如果生成成功，更新计数
        if (spawnSuccess) {
            troopPlacedCounts_[index]++;
            updateTroopCountLabel(index);

            // 如果达到上限，禁用按钮
            if (troopPlacedCounts_[index] >= troopMaxCounts_[index]) {
                disableTroopButton(index);
                showInvalidSpawnMessage(troopNames_[index] + "已全部放置完成");
            }
        }

        
    }
    else {
        // 位置无效，显示相应的提示
        showInvalidSpawnMessage();
    }

    return true; // 返回true表示已处理触摸事件
}

// 更新兵种数量标签
void EnemyVillage::updateTroopCountLabel(int index) {
    if (index >= 0 && index < troopCountLabels_.size()) {
        auto label = troopCountLabels_[index];
        label->setString(std::to_string(troopPlacedCounts_[index]) + "/" +
            std::to_string(troopMaxCounts_[index]));

        // 如果达到上限，改变文字颜色
        if (troopPlacedCounts_[index] >= troopMaxCounts_[index]) {
            label->setColor(cocos2d::Color3B::RED);
        }
        else {
            label->setColor(cocos2d::Color3B::WHITE);
        }
    }
}

// 禁用已满的兵种按钮
void EnemyVillage::disableTroopButton(int index) {
    if (index >= 0 && index < troopButtons_.size()) {
        auto button = troopButtons_[index];
        button->setColor(cocos2d::Color3B(100, 100, 100)); // 灰色表示禁用

        // 如果这个按钮当前被选中，取消选中
        if (selectedItemBg == button) {
            remove_border(selectedItemBg);
            selectedItemBg = nullptr;
            selectedTroopType = 0;
        }
    }
}

bool EnemyVillage::spawnBarbarian(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto barbarian = Barbarian::create(base_map_, 1, position);
    if (barbarian) {
        troop_list_.push_back(barbarian);
        base_map_->sprites_.push_back(barbarian);

        return true;
        
    }
    return false;
    
}
bool EnemyVillage::spawnArcher(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto archer = Archer::create(base_map_, 1, position);
    if (archer) {
        troop_list_.push_back(archer);
        base_map_->sprites_.push_back(archer);
        return true;
    }
    return false;
}
bool EnemyVillage::spawnGiant(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto Giant = Giant::create(base_map_, 1, position);
    if (Giant) {
        troop_list_.push_back(Giant);
        base_map_->sprites_.push_back(Giant);
        return true;
    }
    return false;
}
bool EnemyVillage::spawnDragon(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto Dragon = Dragon::create(base_map_, 1, position);
    if (Dragon) {
        troop_list_.push_back(Dragon);
        base_map_->sprites_.push_back(Dragon);
        return true;
    }
    return false;
}
bool EnemyVillage::spawnBomb(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto Giant = Giant::create(base_map_, 1, position);
    if (Giant) {
        troop_list_.push_back(Giant);
        base_map_->sprites_.push_back(Giant);
        return true;
    }
    return false;
}
bool EnemyVillage::spawnBalloon(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto Balloon = Balloon::create(base_map_, 1, position);
    if (Balloon) {
        troop_list_.push_back(Balloon);
        base_map_->sprites_.push_back(Balloon);
        return true;
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
    std::vector<std::string> troopNames = { "野蛮人", "弓箭手", "巨人","炸弹人","气球兵","飞龙" };
    std::vector<std::string> troopImages = { "troop/babarian_icon.png","troop/archer_icon.png","troop/Giant_icon.png","troop/bomb_icon.png" ,"troop/balloon_icon.png" ,"troop/dragonBaby_icon.png" };
    troopMaxCounts_ = { 5, 10, 3 ,10,4,1 };  // 兵种最大数量
    troopPlacedCounts_ = { 0, 0, 0 ,0,0,0 };  // 已放置数量（成员变量）

    auto panelSwallowListener = cocos2d::EventListenerTouchOneByOne::create();
    panelSwallowListener->setSwallowTouches(true);
    panelSwallowListener->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event) {
        auto target = static_cast<cocos2d::Node*>(event->getCurrentTarget());
        cocos2d::Rect rect = target->getBoundingBox();
        if (rect.containsPoint(touch->getLocation())) {
            return true;
        }
        return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(panelSwallowListener, bg);

    // 创建按钮
    float buttonWidth = 200;
    float buttonHeight = 200;
    float padding = 60;

    // 清空之前的按钮数组
    troopButtons_.clear();
    troopCountLabels_.clear();

    for (size_t i = 0; i < troopNames.size(); ++i) {
        // 商品背景
        auto itemBg = cocos2d::LayerColor::create(cocos2d::Color4B(140, 150, 200, 255), buttonWidth, buttonHeight);
        itemBg->setPosition(cocos2d::Vec2((buttonWidth + padding) * i + buttonWidth, 30));

        // 保存按钮引用
        troopButtons_.push_back(itemBg);

        float maxSize = 200.0f;
        auto itemPic = cocos2d::Sprite::create(troopImages[i]);
        float scale = std::min(maxSize / itemPic->getContentSize().width,
            maxSize / itemPic->getContentSize().height);
        itemPic->setPosition(cocos2d::Vec2(buttonWidth / 2, buttonHeight / 2));
        itemPic->setScale(scale);

        // 显示兵种名称
        auto nameLabel = cocos2d::Label::createWithSystemFont(troopNames[i], "Arial", 30);
        nameLabel->setPosition(cocos2d::Vec2(buttonWidth / 2, 15));
        itemBg->addChild(nameLabel, 150);

        // 显示数量标签（已放置/最大数量）
        auto countLabel = cocos2d::Label::createWithSystemFont(
            std::to_string(troopPlacedCounts_[i]) + "/" + std::to_string(troopMaxCounts_[i]),
            "Arial", 25);
        countLabel->setPosition(cocos2d::Vec2(buttonWidth / 2, buttonHeight - 10));
        countLabel->setTag(1000); // 设置tag以便后续更新
        itemBg->addChild(countLabel, 150);

        // 保存数量标签引用
        troopCountLabels_.push_back(countLabel);

        // 将按钮添加到背景层
        itemBg->addChild(itemPic);
        bg->addChild(itemBg);

        // 添加触摸事件监听器
        auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = [this, itemBg, i](cocos2d::Touch* touch, cocos2d::Event* event) {
            cocos2d::Rect buttonRect = itemBg->getBoundingBox();
            if (buttonRect.containsPoint(touch->getLocation())) {
                this->onButtonClick(itemBg, i);
                return true;
            }
            return false;
            };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, itemBg);
    }
}
// 点击按钮时的处理函数
void EnemyVillage::onButtonClick(cocos2d::LayerColor* itemBg, int index) {
    bool isC = GameManager::getInstance()->isReplay;
    if (!isC) {
        // 如果点击的是同一个按钮，取消选中状态
        if (selectedItemBg == itemBg) {
            selectedItemBg->setColor(cocos2d::Color3B(140, 150, 200));  // 恢复原始颜色
            remove_border(selectedItemBg);
            selectedItemBg = nullptr;
            return;  // 已经是选中的按钮，不做任何改变
        }
        // 如果已有按钮被选中，取消选中状态并恢复原始颜色
        if (selectedItemBg) {
            selectedItemBg->setColor(cocos2d::Color3B(140, 150, 200));  // 恢复原始颜色
            remove_border(selectedItemBg);
        }
        // 检查是否已达到上限
        if (troopPlacedCounts_[index] >= troopMaxCounts_[index]) {
            showInvalidSpawnMessage(troopNames_[index] + "已达到上限！");
            return;
        }
        // 更新当前选中的按钮
        selectedItemBg = itemBg;
        selectedTroopType = index + 1;
        // 更改选中按钮的颜色
        selectedItemBg->setColor(cocos2d::Color3B(100, 100, 150));  // 变暗的颜色
        draw_border(selectedItemBg);
    }
}

bool EnemyVillage::onReplayButtonClick(cocos2d::Ref* sender, int gold_, int elixir_) {
    AudioEngine::stopAll();
    if (GameManager::getInstance()->isReplay)return false;
    for (auto troop : troop_list_) {
        troop->setDead();
    }
    TroopTargetManager::getInstance()->clear();
    CocController::getInstance()->changeScene();
    CocController::getInstance()->changeScene(1, gold_, elixir_);
    GameManager::getInstance()->isReplay = true;
    // 在UI层之后添加回放检查
    /*if (GameManager::getInstance()->isReplay) {
        CCLOG("检测到回放模式，准备回放");

        // 使用scheduleOnce确保场景完全初始化
        this->scheduleOnce([=](float dt) {
            this->startReplaySequence();
            }, 0.0f, "init_replay");
    }*/
    return true;
}
void EnemyVillage::ReplayBegin() {
    bool isR = GameManager::getInstance()->isReplay;
    if (isR) {
        int sum = UnitManager::getInstance()->units.size();
        int maxSpawnCount = sum; // 设置上限


        // 定义一个定时器，每隔1秒执行一次
        this->schedule([=](float dt) {
            static int currentSpawnCount = 0;
            CCLOG("投放士兵");
            if (currentSpawnCount < maxSpawnCount) {
                Unit unit = UnitManager::getInstance()->units[currentSpawnCount];
                if (unit.type == 0) {
                    spawnBarbarian(unit.position);
                }
                currentSpawnCount++;
            }

            }, 1.0f, "spawnUnitsWithInterval"); // 每隔1秒调用一次
    }
    //GameManager::getInstance()->isReplay = false;
}

void EnemyVillage::startReplaySequence() {
    CCLOG("启动回放序列");

    auto& units = UnitManager::getInstance()->units;
    CCLOG("回放单位数量: %zu", units.size());

    if (units.empty()) {
        CCLOG("没有单位需要回放");
        return;
    }

    // 创建序列动作：逐个生成士兵
    Vector<FiniteTimeAction*> actions;

    for (size_t i = 0; i < units.size(); ++i) {
        // 延迟
        auto delay = DelayTime::create(1.0f);

        // 生成士兵的动作
        auto spawn = CallFunc::create([this, i, &units]() {
            Unit unit = units[i];
            CCLOG("回放生成士兵 %zu: type=%d", i, unit.type);

            switch (unit.type) {
                case 0: spawnBarbarian(unit.position); break;
                case 1: spawnArcher(unit.position); break;
                case 2: spawnGiant(unit.position); break;
                case 3: spawnBomb(unit.position); break;
                case 4: spawnBalloon(unit.position); break;
                case 5: spawnDragon(unit.position); break;
            }
            });

        actions.pushBack(delay);
        actions.pushBack(spawn);
    }

    // 添加完成回调
    auto finish = CallFunc::create([this]() {
        CCLOG("回放完成");
        GameManager::getInstance()->isReplay = false;
        });
    actions.pushBack(finish);

    // 运行序列
    auto sequence = Sequence::create(actions);
    this->runAction(sequence);
}
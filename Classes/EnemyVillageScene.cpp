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
#include"Archer.h"
#include"Giant.h"
#include"UIcommon.h"
EnemyVillage* EnemyVillage::create(int level, unsigned long long gold, unsigned long long elixir)
{
    EnemyVillage* pRet = new(std::nothrow) EnemyVillage(); if (pRet && pRet->myInit(level, gold, elixir)) {
        pRet->autorelease(); return pRet;
    }
    else {
        delete pRet; pRet = nullptr; return nullptr;
    }
};

bool EnemyVillage::myInit(int level, unsigned long long gold, unsigned long long elixir)
{
    if (!Village::init()) {
        return false;
    }
    // 转移主村庄资源存储数据
    Village::gold_ = gold;
    Village::elixir_ = elixir;

    // 从数据文件中读取建筑数据并创建建筑对象
    time_t current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    time_t data_time = 0;
    if (!DataHelper::readArchData(kOfflineDataFile[level], data_time, arch_status_)) {
        return false;
    }

    std::vector<ArchData> arch_list;
    DataHelper::mapToList(arch_status_, arch_list);

    Arch* p = nullptr;
    for (auto& arch : arch_list) {
        p = Arch::create(arch, base_map_, false);
        TroopTargetManager::getInstance()->registerTroopTarget(p);
    }
    auto barbarian = Barbarian::create(base_map_, 1, cocos2d::Vec2(40, 20));
    if (!barbarian)return false;
	troop_list_.push_back(barbarian);
    // 预计算所有建筑的距离场
    TroopTargetManager::getInstance()->precomputeDistanceFields(troop_list_);


    
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



    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    auto origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    auto closeItem = cocos2d::MenuItemLabel::create(
        cocos2d::Label::createWithSystemFont("退出", "Arial", 72),
        CC_CALLBACK_1(EnemyVillage::onExitButtonClick, this));

    float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2 - 20;
    float y = origin.y + closeItem->getContentSize().height / 2 + 20;
    closeItem->setPosition(cocos2d::Vec2(x, y));

    auto menu = cocos2d::Menu::create(closeItem, NULL);
    menu->setPosition(cocos2d::Vec2::ZERO);
    this->addChild(menu, 100);

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


    // 设置面板的背景
    auto bgSprite = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 150), visibleSize.width, visibleSize.height/4);
    bgSprite->setAnchorPoint(cocos2d::Vec2(0, 0));
    bgSprite->setPosition(cocos2d::Vec2(0, 0));
    this->addChild(bgSprite,10);
    // 创建兵种按钮
   // bgSprite->TroopPanel::createTroopOptions( bgSprite);
    createTroopSelectionPanel(bgSprite);
    return true;
}

void EnemyVillage::onExitButtonClick(cocos2d::Ref* sender)
{
    for(auto troop : troop_list_) {
        troop->setDead();
	}
    TroopTargetManager::getInstance()->clear();
    CocController::getInstance()->changeScene();
}


bool EnemyVillage::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event, std::set<std::pair<int, int>> occupied_cells)
{
    // 将触摸位置转换为地图坐标
    cocos2d::Vec2 touchLocation = touch->getLocation();
    touchLocation = this->convertToNodeSpace(touchLocation); // 转换为本地坐标

    // 检查触摸位置是否在有效的单元格内
    bool isValidLocation = true;
    for (const auto& cell : occupied_cells) {
        cocos2d::Vec2 cellPosition = CoordAdaptor::cellToPixel(base_map_, cocos2d::Vec2(cell.first + 0.5f, cell.second + 0.5f));

        // 检查触摸是否足够接近该单元格
        if (touchLocation.distance(cellPosition) < 100) { // 根据需要调整距离
            isValidLocation = true;
            break;
        }
    }

    // 如果位置有效，生成士兵
    if (isValidLocation) {
        spawnBarbarian(touchLocation);
    }
    else {
        showInvalidSpawnMessage();
    }

    return true; // 返回true表示已处理触摸事件
}



void EnemyVillage::spawnBarbarian(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto barbarian = Barbarian::create(base_map_, 1, cocos2d::Vec2(0.5, 0.5));
    if (barbarian) {
        troop_list_.push_back(barbarian);
        base_map_->sprites_.push_back(barbarian);
    }
    barbarian->takeDamage(500);
}

void EnemyVillage::showInvalidSpawnMessage()
{
    // 显示消息提示玩家不能在该位置生成士兵（可以使用弹窗或标签）
    auto label = cocos2d::Label::createWithSystemFont("Cannot spawn here!", "Arial", 36);
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

    std::vector<std::string> troopNames = { "野蛮人", "弓箭手", "巨人","炸弹人","气球兵","飞龙"};
    std::vector<std::string> troopImages = { "troop/babarian_icon.png","troop/archer_icon.png","troop/Giant_icon.png","troop/bomb_icon.png" ,"troop/balloon_icon.png" ,"troop/dragonBaby_icon.png" };
    std::vector<int> troopCounts = { 5, 10, 3 ,10,4,1};  // 兵种数量

    // 创建按钮
    float buttonWidth =200;  // 每个按钮的宽度
    float buttonHeight = 200;  // 每个按钮的高度
    float padding = 60;  // 按钮间隔


    for (size_t i = 0; i < troopNames.size(); ++i) {
        // 商品背景
        auto itemBg = cocos2d::LayerColor::create(cocos2d::Color4B(140, 150, 200, 255), buttonWidth, buttonHeight);
        itemBg->setPosition(cocos2d::Vec2((buttonWidth + padding) * i + buttonWidth, 30));
        
        float maxSize = 200.0f; // 调整图片大小
        auto itemPic = cocos2d::Sprite::create(troopImages[i]);
        float scale = std::min(maxSize / itemPic->getContentSize().width,
            maxSize / itemPic->getContentSize().height);
        itemPic->setPosition(cocos2d::Vec2(buttonWidth / 2, buttonHeight / 2));
        itemPic->setScale(scale);

        // 显示兵种名称和数量
        auto nameLabel = cocos2d::Label::createWithSystemFont(troopNames[i], "Arial", 30);
        nameLabel->setPosition(cocos2d::Vec2(buttonWidth / 2, 15));  // 名字位置
        //nameLabel->setColor(cocos2d::Color3B::BLACK);
        itemBg->addChild(nameLabel,150);

        auto countLabel = cocos2d::Label::createWithSystemFont("x" + std::to_string(troopCounts[i]), "Arial", 25);
        countLabel->setPosition(cocos2d::Vec2(buttonWidth / 2, buttonHeight-10));  // 数量位置
        //countLabel->setColor(cocos2d::Color3B::BLACK);
        itemBg->addChild(countLabel,150);
        // 将按钮添加到背景层
        itemBg->addChild(itemPic);
        bg->addChild(itemBg);

        // 添加触摸事件监听器
        auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = [this, itemBg, i](cocos2d::Touch* touch, cocos2d::Event* event) {
            // 获取触摸点并判断是否点击了按钮
            cocos2d::Rect buttonRect = itemBg->getBoundingBox();
            if (buttonRect.containsPoint(touch->getLocation())) {
                this->onButtonClick(itemBg, i);
                return true;  // 阻止事件继续传播
            }
            return false;
            };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, itemBg);  // 为按钮添加触摸事件

    }
}
// 点击按钮时的处理函数
void EnemyVillage::onButtonClick(cocos2d::LayerColor* itemBg, int index) {
    // 如果点击的是同一个按钮，保持选中状态
    if (selectedItemBg == itemBg) {
        return;  // 已经是选中的按钮，不做任何改变
    }
    // 如果已有按钮被选中，取消选中状态并恢复原始颜色
    if (selectedItemBg) {
        selectedItemBg->setColor(cocos2d::Color3B(140, 150, 200));  // 恢复原始颜色
        remove_border(selectedItemBg);
    }

    // 更新当前选中的按钮
    selectedItemBg = itemBg;

    // 更改选中按钮的颜色
    selectedItemBg->setColor(cocos2d::Color3B(100, 100, 150));  // 变暗的颜色
    draw_border(selectedItemBg);
    // 触发兵种选择操作，可以根据选中的index进行相应的操作
    //spawnSelectedTroop(index);
}


void EnemyVillage::spawnSelectedTroop(const std::string& troopName)
{
    cocos2d::Vec2 spawnPosition = cocos2d::Vec2(0,0) ;  // 获取玩家点击的位置getSpawnPosition()

    if (troopName == "Barbarian") {
        spawnBarbarian(spawnPosition);  // 创建并投放 Barbarian
    }
    else if (troopName == "Archer") {
        //spawnArcher(spawnPosition);  // 创建并投放 Archer（需要实现这个函数）
    }
    else if (troopName == "Wizard") {
        //spawnWizard(spawnPosition);  // 创建并投放 Wizard（需要实现这个函数）
    }
}


// 为其他兵种创建类似的函数 spawnArcher 和 spawnWizard...

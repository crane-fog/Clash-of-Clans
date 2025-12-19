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
USING_NS_CC;
int selectedTroopType =0;  // -1表示未选择任何兵种
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



    // 获取触摸位置（屏幕坐标）
    cocos2d::Vec2 touchLocation = touch->getLocation();

    // 转换为本地坐标（考虑Y轴翻转）
    //touchLocation.y = cocos2d::Director::getInstance()->getWinSize().height - touchLocation.y;
    touchLocation = this->convertToNodeSpace(touchLocation); // 转换为当前节点的本地坐标

    // 获取当前BaseMap的缩放因子
    float scale = base_map_->getScale();

    // 计算调整后的位置：考虑缩放因子
    touchLocation.x /= scale;
    touchLocation.y /= scale;
    touchLocation.x += 10;
    touchLocation.y += 20;
    bool isValidLocation = true;



    // 将像素坐标转换为格子坐标
    touchLocation = CoordAdaptor::pixelToCell(base_map_, touchLocation);
    // 检查触摸位置是否在红色区域之外
    for (const auto& cell : occupied_cells) {
        // 将格子坐标转为像素坐标
        cocos2d::Vec2 cellPosition =cocos2d::Vec2(cell.first , cell.second );

        // 检查触摸位置是否接近该单元格
        if (touchLocation.distance(cellPosition) < 1) { // 如果触摸位置接近红色范围
            isValidLocation = false;  // 如果触摸位置在红色范围内，则不允许生成
            break;
        }
    }
    // 如果位置有效（不在红色区域），生成士兵
    if (isValidLocation) {
        if (selectedTroopType == 1) {
            spawnBarbarian(touchLocation);
        }
        else if (selectedTroopType == 2)spawnArcher(touchLocation);
        else if (selectedTroopType == 3)spawnGiant(touchLocation);
        else if (selectedTroopType == 4)spawnArcher(touchLocation);
        else if (selectedTroopType == 5)spawnArcher(touchLocation);
        else if (selectedTroopType == 6)spawnArcher(touchLocation);
        else showInvalidSpawnMessage("请选择兵种");
    }
    else {
        // 位置无效，显示相应的提示
        showInvalidSpawnMessage();
    }

    return true; // 返回true表示已处理触摸事件
}




void EnemyVillage::spawnBarbarian(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto barbarian = Barbarian::create(base_map_, 1, position);
    if (barbarian) {
        troop_list_.push_back(barbarian);
        base_map_->sprites_.push_back(barbarian);
        // 使用 std::to_string() 转换 x 和 y 为字符串
        std::string positionStr = "Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")";
        showInvalidSpawnMessage(positionStr);
    }
    
}
void EnemyVillage::spawnArcher(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto archer = Archer::create(base_map_, 1, position);
    if (archer) {
        troop_list_.push_back(archer);
        base_map_->sprites_.push_back(archer);
        // 使用 std::to_string() 转换 x 和 y 为字符串
        std::string positionStr = "Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")";
        showInvalidSpawnMessage(positionStr);
    }

}
void EnemyVillage::spawnGiant(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto Giant = Giant::create(base_map_, 1, position);
    if (Giant) {
        troop_list_.push_back(Giant);
        base_map_->sprites_.push_back(Giant);
        // 使用 std::to_string() 转换 x 和 y 为字符串
        std::string positionStr = "Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")";
        showInvalidSpawnMessage(positionStr);
    }

}
void EnemyVillage::spawnDragon(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto Giant = Giant::create(base_map_, 1, position);
    if (Giant) {
        troop_list_.push_back(Giant);
        base_map_->sprites_.push_back(Giant);
        // 使用 std::to_string() 转换 x 和 y 为字符串
        std::string positionStr = "Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")";
        showInvalidSpawnMessage(positionStr);
    }

}
void EnemyVillage::spawnBomb(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto Giant = Giant::create(base_map_, 1, position);
    if (Giant) {
        troop_list_.push_back(Giant);
        base_map_->sprites_.push_back(Giant);
        // 使用 std::to_string() 转换 x 和 y 为字符串
        std::string positionStr = "Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")";
        showInvalidSpawnMessage(positionStr);
    }

}
void EnemyVillage::spawnBalloon(cocos2d::Vec2 position)
{
    // 在触摸位置生成士兵
    auto Giant = Giant::create(base_map_, 1, position);
    if (Giant) {
        troop_list_.push_back(Giant);
        base_map_->sprites_.push_back(Giant);
        // 使用 std::to_string() 转换 x 和 y 为字符串
        std::string positionStr = "Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")";
        showInvalidSpawnMessage(positionStr);
    }

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
    selectedTroopType = index + 1;
    // 更改选中按钮的颜色
    selectedItemBg->setColor(cocos2d::Color3B(100, 100, 150));  // 变暗的颜色
    draw_border(selectedItemBg);
    // 触发兵种选择操作，可以根据选中的index进行相应的操作
    spawnSelectedTroop(index);
    // 在场景中添加鼠标监听器
    addMouseListener(this);  // 将监听器添加到当前场景（或节点）中
}


void EnemyVillage::spawnSelectedTroop(int index)
{
    cocos2d::Vec2 spawnPosition = cocos2d::Vec2(0,0) ;  // 获取玩家点击的位置getSpawnPosition()

    if (index==0) {
        spawnBarbarian(spawnPosition);  // 创建并投放 Barbarian
    }
    else if (index == 1) {
        //spawnArcher(spawnPosition);  // 创建并投放 Archer（需要实现这个函数）
    }
    else if (index == 3) {
        //spawnWizard(spawnPosition);  // 创建并投放 Wizard（需要实现这个函数）
    }
}
// 鼠标点击事件回调函数
void EnemyVillage::onMouseClick(cocos2d::Event* event) {
    // 获取触摸事件对象
    cocos2d::EventMouse* mouseEvent = dynamic_cast<cocos2d::EventMouse*>(event);

    // 获取鼠标点击的世界坐标位置
    cocos2d::Vec2 mousePos = mouseEvent->getLocation();

    // 将世界坐标转换为视图坐标（可选）
    cocos2d::Vec2 viewPos = cocos2d::Director::getInstance()->convertToUI(mousePos);

    // 输出点击位置
    CCLOG("Mouse clicked at: (%.2f, %.2f)", mousePos.x, mousePos.y);

    // 这里可以调用你想执行的函数，比如根据点击位置生成Barbarian
    spawnBarbarian(mousePos);  // 根据鼠标点击的位置生成Barbarian
}

// 添加鼠标事件监听器
void EnemyVillage::addMouseListener(cocos2d::Node* parentNode) {
    auto mouseListener = cocos2d::EventListenerMouse::create();

    // 设置鼠标点击事件的回调函数
    mouseListener->onMouseDown = CC_CALLBACK_1(EnemyVillage::onMouseClick, this);

    // 将监听器添加到事件派发器中
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, parentNode);
}

// 为其他兵种创建类似的函数 spawnArcher 和 spawnWizard...

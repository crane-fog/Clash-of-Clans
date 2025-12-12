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


    
    // 显示红色底色（我方可下兵范围）
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

    return true;
}

void EnemyVillage::onExitButtonClick(cocos2d::Ref* sender)
{
    CocController::getInstance()->changeScene();
}
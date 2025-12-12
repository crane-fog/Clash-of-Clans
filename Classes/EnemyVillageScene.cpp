#include "EnemyVillageScene.h"
#include "DataHelper.h"
#include "MainVillageScene.h"
#include "CocController.h"
#include "TroopTargetManager.h"

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
    if (!DataHelper::readArchData(kOfflineDataFile[level - 1], data_time, arch_status_)) {
        return false;
    }

    std::vector<ArchData> arch_list;
    DataHelper::mapToList(arch_status_, arch_list);

    Arch* p = nullptr;
    for (auto& arch : arch_list) {
        p = Arch::create(arch, base_map_);
        TroopTargetManager::getInstance()->registerTroopTarget(p);
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

    return true;
}

void EnemyVillage::onExitButtonClick(cocos2d::Ref* sender)
{
    CocController::getInstance()->changeScene();
}
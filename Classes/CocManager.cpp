#include "CocManager.h"

#include "EnemyVillageScene.h"
#include "MainVillageScene.h"
#include "Arch.h"
#include "ArchInfo.h"
#include "Troop.h"
#include "Barbarian.h"
#include "Archer.h"
#include "Giant.h"
#include "WallBreaker.h"
#include "Balloon.h"
#include "Dragon.h"

USING_NS_CC;

CocManager::CocManager()
{
    current_scene_ = 0;  // 0=MainVillage
    ArchFactory::registerCreater(
        TOWN_HALL, [](const ArchData& data, BaseMap* map, bool is_mine) { return new TownHall(data, map, is_mine); });
    ArchFactory::registerCreater(
        WALL, [](const ArchData& data, BaseMap* map, bool is_mine) { return new Wall(data, map, is_mine); });
    ArchFactory::registerCreater(GOLD_STORAGE, [](const ArchData& data, BaseMap* map, bool is_mine) {
        return new GoldStorage(data, map, is_mine);
    });
    ArchFactory::registerCreater(ELIXIR_STORAGE, [](const ArchData& data, BaseMap* map, bool is_mine) {
        return new ElixirStorage(data, map, is_mine);
    });
    ArchFactory::registerCreater(
        GOLD_MINE, [](const ArchData& data, BaseMap* map, bool is_mine) { return new GoldMine(data, map, is_mine); });
    ArchFactory::registerCreater(ELIXIR_COLLECTOR, [](const ArchData& data, BaseMap* map, bool is_mine) {
        return new ElixirCollector(data, map, is_mine);
    });
    ArchFactory::registerCreater(
        BARRACKS, [](const ArchData& data, BaseMap* map, bool is_mine) { return new Barracks(data, map, is_mine); });
    ArchFactory::registerCreater(
        ARMY_CAMP, [](const ArchData& data, BaseMap* map, bool is_mine) { return new ArmyCamp(data, map, is_mine); });
    ArchFactory::registerCreater(
        CANNON, [](const ArchData& data, BaseMap* map, bool is_mine) { return new Cannon(data, map, is_mine); });
    ArchFactory::registerCreater(ARCHER_TOWER, [](const ArchData& data, BaseMap* map, bool is_mine) {
        return new ArcherTower(data, map, is_mine);
    });
    ArchFactory::registerCreater(
        BOMB, [](const ArchData& data, BaseMap* map, bool is_mine) { return new Bomb(data, map, is_mine); });

    TroopFactory::registerCreater(
        Troop::BARBARIAN, [](BaseMap* map, int lvl, cocos2d::Vec2 pos) { return Barbarian::create(map, lvl, pos); });
    TroopFactory::registerCreater(
        Troop::ARCHER, [](BaseMap* map, int lvl, cocos2d::Vec2 pos) { return Archer::create(map, lvl, pos); });
    TroopFactory::registerCreater(
        Troop::GIANT, [](BaseMap* map, int lvl, cocos2d::Vec2 pos) { return Giant::create(map, lvl, pos); });
    TroopFactory::registerCreater(Troop::WALL_BREAKER, [](BaseMap* map, int lvl, cocos2d::Vec2 pos) {
        return WallBreaker::create(map, lvl, pos);
    });
    TroopFactory::registerCreater(
        Troop::BALLOON, [](BaseMap* map, int lvl, cocos2d::Vec2 pos) { return Balloon::create(map, lvl, pos); });
    TroopFactory::registerCreater(
        Troop::DRAGON, [](BaseMap* map, int lvl, cocos2d::Vec2 pos) { return Dragon::create(map, lvl, pos); });
}

void CocManager::run()
{
    // 创建并运行主村庄场景
    auto scene = MainVillage::create();
    Director::getInstance()->runWithScene(scene);
}

CocManager* CocManager::getInstance()
{
    static CocManager instance;
    return &instance;
}

void CocManager::endGame() { Director::getInstance()->end(); }

void CocManager::changeScene(int level_no)
{
    // 在自己的村庄时
    if (current_scene_ == 0) {
        current_scene_ = level_no;
        Director::getInstance()->pushScene(EnemyVillage::create(level_no));
    }
    // 在敌人村庄时
    else {
        current_scene_ = 0;
        Director::getInstance()->popScene();
    }
}
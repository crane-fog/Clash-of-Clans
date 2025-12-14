#include "CocController.h"
#include "MainVillageScene.h"
#include "EnemyVillageScene.h"

USING_NS_CC;

CocController::CocController()
{
    // 创建并运行主村庄场景
    auto scene = MainVillage::create();
    Director::getInstance()->runWithScene(scene);
    current_scene_ = 0; // 0=MainVillage
}

CocController* CocController::getInstance()
{
    static CocController instance;
    return &instance;
}

void CocController::endGame()
{
    Director::getInstance()->end();
}

void CocController::changeScene(int level_no, unsigned long long gold, unsigned long long elixir)
{
    // 在自己的村庄时
    if (current_scene_ == 0) {
        current_scene_ = 1;
        Director::getInstance()->pushScene(EnemyVillage::create(level_no, gold, elixir));
    }
    // 在敌人村庄时
    else if (current_scene_ == 1) {
        current_scene_ = 0;
        Director::getInstance()->popScene();
    }
}
#include "CocController.h"
#include "MainVillageScene.h"

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

void CocController::changeScene()
{
    // 在自己的村庄时
    if (current_scene_ == 0) {
        // Director::getInstance()->pushScene(EnemyVillage::create());
    }
    // 在敌人村庄时
    else if (current_scene_ == 1) {
        // Director::getInstance()->popScene();
    }
}
#include "CocManager.h"

#include "EnemyVillageScene.h"
#include "MainVillageScene.h"

USING_NS_CC;

CocManager::CocManager()
{
    current_scene_ = 0;  // 0=MainVillage
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
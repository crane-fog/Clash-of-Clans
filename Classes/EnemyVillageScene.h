#ifndef __ENEMY_VILLAGE_SCENE_H__
#define __ENEMY_VILLAGE_SCENE_H__

#include "VillageScene.h"

// 敌人村庄场景类
class EnemyVillage : public Village {
public:
    virtual bool myInit(int level, unsigned long long gold, unsigned long long elixir);
    static EnemyVillage* create(int level, unsigned long long gold, unsigned long long elixir);
    void onExitButtonClick(cocos2d::Ref* sender);
};

#endif // __ENEMY_VILLAGE_SCENE_H__
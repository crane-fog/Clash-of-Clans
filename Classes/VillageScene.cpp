#include "VillageScene.h"

USING_NS_CC;

bool Village::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // 创建 BaseMap 容器
    base_map_ = BaseMap::create();
    if (!base_map_) {
        return false;
    }
    this->addChild(base_map_, 0);

    return true;
}

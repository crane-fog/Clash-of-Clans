#include "VillageScene.h"
#include "UIparts.h"
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
    // 创建UI层（固定UI层）
    ui_layer_ = UI::create();
    if (!ui_layer_) {
        return false;
    }
    // UI层直接添加到场景，不受base_map变换影响
    this->addChild(ui_layer_, 10);  // 较高的z-order，确保UI显示在最上层且固定
    return true;
}
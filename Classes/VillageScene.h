#ifndef __VILLAGE_SCENE_H__
#define __VILLAGE_SCENE_H__

#include "cocos2d.h"
#include "BaseMap.h"
#include "UIparts.h"

// 村庄场景基类
class Village : public cocos2d::Scene {
protected:
    // 用于所有场景中非固定对象的容器，允许界面整体缩放、移动
    BaseMap* base_map_;
    UIBars* ui_layer_;  // 在基类中添加UI层
    // 村庄中的建筑状态数据
    ArchData arch_status_[MAP_SIZE][MAP_SIZE];

    // 金币储量
    unsigned long long gold_;
    // 圣水储量
    unsigned long long elixir_;

public:

    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;
    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(Village);
};

#endif // __VILLAGE_SCENE_H__
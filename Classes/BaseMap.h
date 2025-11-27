#ifndef __BASE_MAP_H__
#define __BASE_MAP_H__

#include "cocos2d.h"

// 继承自 Node，作为整个地图世界的容器
class BaseMap : public cocos2d::Node {
private:
    // 初始化
    virtual bool init();
    // 当节点进入舞台时调用
    virtual void onEnter() override;
    // 用于存储地图上所有精灵的容器，其中0固定为背景图
    // todo: 怎么管理这些精灵/找到某个具体的精灵
    std::vector<cocos2d::Sprite*> sprites_;

protected:
    // 鼠标事件回调
    void onMouseScroll(cocos2d::Event* event);
    void onMouseDown(cocos2d::Event* event);
    void onMouseUp(cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);
    // 检查边界并修正位置
    void checkAndClampPosition();

public:
    // 替代构造函数
    static BaseMap* create();

    // 用于鼠标监听相关的变量
    bool is_dragging_ = false;
    cocos2d::Vec2 last_mouse_pos_;
    cocos2d::EventListenerMouse* mouse_listener_ = nullptr;
};

#endif // __BASE_MAP_H__

#ifndef __BASE_MAP_H__
#define __BASE_MAP_H__

#include "cocos2d.h"

// 作为整个地图的容器，允许整体缩放和移动
class BaseMap : public cocos2d::Node {
private:
    // 鼠标事件回调
    void onMouseScroll(cocos2d::Event* event);
    void onMouseDown(cocos2d::Event* event);
    void onMouseUp(cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);
    // 检查边界并修正位置
    void checkAndClampPosition();

    // 用于鼠标监听相关的变量
    bool is_dragging_ = false;
    cocos2d::Vec2 last_mouse_pos_;
    cocos2d::EventListenerMouse* mouse_listener_ = nullptr;

public:
    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;
    // 当对象被渲染时被自动调用
    virtual void onEnter() override;
    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(BaseMap);

    // 切换有线框/无线框背景图
    void changeLinedMap();

    // todo: 索引与管理
    // 用于存储地图上精灵的容器，其中0和1固定为两张背景图
    std::vector<cocos2d::Sprite*> sprites_;
};

#endif // __BASE_MAP_H__

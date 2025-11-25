#ifndef __BASE_MAP_H__
#define __BASE_MAP_H__

#include "cocos2d.h"

class BaseMap : public cocos2d::Sprite {
private:
    // 初始化
    virtual bool initWithFile(const std::string& filename) override;

    // 当节点进入舞台时调用（用于设置初始位置等）
    virtual void onEnter() override;

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
    static BaseMap* create(const std::string& filename);

    bool is_dragging_ = false;
    cocos2d::Vec2 last_mouse_pos_;
    cocos2d::EventListenerMouse* mouse_listener_;
};

#endif // __BASE_MAP_H__

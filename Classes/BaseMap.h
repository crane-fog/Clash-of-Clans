#ifndef __BASE_MAP_H__
#define __BASE_MAP_H__

#include "cocos2d.h"

class Arch;

const int kMapSize = 44;

// 作为整个地图的容器，允许整体缩放和移动
class BaseMap : public cocos2d::Node {
private:
    // 触摸事件回调
    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    // 鼠标滚轮回调
    void onMouseScroll(cocos2d::Event* event);
#endif

    // 检查边界并修正位置
    void checkAndClampPosition();

    // 用于触摸监听相关的变量
    std::vector<cocos2d::Touch*> current_touches_;
    cocos2d::EventListenerTouchAllAtOnce* touch_listener_ = nullptr;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    cocos2d::EventListenerMouse* mouse_listener_ = nullptr;
#endif

    // 背景图
    cocos2d::Sprite* lined_map_ = nullptr;
    cocos2d::Sprite* unlined_map_ = nullptr;

public:
    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;

    // 当对象被渲染时被自动调用
    virtual void onEnter() override;

    // 当对象退出时被自动调用
    virtual void onExit() override;

    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(BaseMap);

    // 切换有线框/无线框背景图
    void changeLinedMap();

    // 存储地图上所有建筑的容器
    std::vector<Arch*> archs_;

    // 鼠标调用
    void setInputEnabled(bool enabled);

    // 当前打开的建筑面板
    cocos2d::Node* current_arch_panel_ = nullptr;
};

#endif  // __BASE_MAP_H__

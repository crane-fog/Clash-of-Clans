#include "BaseMap.h"
#include <algorithm>

USING_NS_CC;

bool BaseMap::init()
{
    // 调用父类初始化
    if (!Node::init()) {
        return false;
    }

    // 创建背景图 Sprite
    sprites_.push_back(Sprite::create("BaseMap.jpg"));
    if (!sprites_.front()) {
        return false;
    }
    // 背景图左下角对齐容器左下角
    sprites_.front()->setAnchorPoint(Vec2::ZERO);
    sprites_.front()->setPosition(Vec2::ZERO);
    // 将背景图设定为 BaseMap 的子对象
    this->addChild(sprites_.front(), -1);

    // 容器的 ContentSize 设置为背景图大小
    this->setContentSize(sprites_.front()->getContentSize());
    // 锚点(0,0)
    this->setAnchorPoint(Vec2::ZERO);

    // 创建线框背景图 Sprite
    sprites_.push_back(Sprite::create("LinedBaseMap.jpg"));
    if (!sprites_.back()) {
        return false;
    }
    sprites_.back()->setAnchorPoint(Vec2::ZERO);
    sprites_.back()->setPosition(Vec2::ZERO);
    this->addChild(sprites_.back(), -1);
    sprites_.back()->setVisible(false); // 默认隐藏线框图

    // 初始化变量
    is_dragging_ = false;

    return true;
}

void BaseMap::onEnter()
{
    Node::onEnter();

    // 获取屏幕大小
    auto visible_size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size map_size = sprites_.front()->getContentSize();
    // 计算并应用最小缩放 (填满屏幕)
    float scale_x = visible_size.width / map_size.width;
    float scale_y = visible_size.height / map_size.height;
    float min_scale = std::max(scale_x, scale_y);
    if (this->getScale() < min_scale) {
        this->setScale(min_scale);
    }
    // 计算缩放后的地图尺寸
    float final_w = map_size.width * this->getScale();
    float final_h = map_size.height * this->getScale();
    // 计算居中所需的位置 (屏幕中心 - 地图中心)
    // 因为容器锚点是(0,0)，Position就是容器左下角在屏幕的位置
    float x = origin.x + (visible_size.width - final_w) / 2.0f;
    float y = origin.y + (visible_size.height - final_h) / 2.0f;
    this->setPosition(Vec2(x, y));

    // 创建并绑定鼠标监听器
    mouse_listener_ = EventListenerMouse::create();
    mouse_listener_->onMouseScroll = CC_CALLBACK_1(BaseMap::onMouseScroll, this);
    mouse_listener_->onMouseDown = CC_CALLBACK_1(BaseMap::onMouseDown, this);
    mouse_listener_->onMouseUp = CC_CALLBACK_1(BaseMap::onMouseUp, this);
    mouse_listener_->onMouseMove = CC_CALLBACK_1(BaseMap::onMouseMove, this);
    // 将监听器绑定到当前节点
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse_listener_, this);
}

void BaseMap::changeLinedMap()
{
    sprites_[0]->setVisible(!sprites_[0]->isVisible());
    sprites_[1]->setVisible(!sprites_[1]->isVisible());
}

void BaseMap::checkAndClampPosition()
{
    auto visible_size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 当前容器的缩放比例
    float scale = this->getScale();
    Size map_size = sprites_.front()->getContentSize();
    float actual_width = map_size.width * scale;
    float actual_height = map_size.height * scale;

    Vec2 current_pos = this->getPosition();

    // --- X轴边界计算 ---
    // 容器位置(左下角) 最大不能超过 origin.x
    float max_x = origin.x;
    // 容器位置 最小不能小于 (屏幕宽 - 地图实际宽)
    float min_x = origin.x + visible_size.width - actual_width;

    if (min_x > max_x) {
        current_pos.x = origin.x + (visible_size.width - actual_width) / 2.0f;
    }
    else {
        current_pos.x = std::max(min_x, std::min(current_pos.x, max_x));
    }

    // --- Y轴边界计算 ---
    float max_y = origin.y;
    float min_y = origin.y + visible_size.height - actual_height;

    if (min_y > max_y) {
        current_pos.y = origin.y + (visible_size.height - actual_height) / 2.0f;
    }
    else {
        current_pos.y = std::max(min_y, std::min(current_pos.y, max_y));
    }

    this->setPosition(current_pos);
}

void BaseMap::onMouseScroll(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    float scroll_y = e->getScrollY();
    if (scroll_y == 0) return;

    Vec2 mouse_location = e->getLocation();
    mouse_location.y = Director::getInstance()->getWinSize().height - mouse_location.y; // 转换Y轴坐标

    // 将屏幕坐标转换成 BaseMap 内部坐标
    Vec2 location_in_map = this->convertToNodeSpace(mouse_location);


    float old_scale = this->getScale();
    float factor = 1.1f;
    float new_scale = (scroll_y < 0) ? (old_scale * factor) : (old_scale / factor);

    // 动态计算最小缩放
    auto visible_size = Director::getInstance()->getVisibleSize();
    Size map_size = sprites_.front()->getContentSize();
    float min_scale = std::max(visible_size.width / map_size.width, visible_size.height / map_size.height);

    new_scale = std::max(min_scale, std::min(new_scale, 3.0f));

    // 应用缩放
    this->setScale(new_scale);

    // 位置补偿
    Vec2 new_world_pos_of_point = this->convertToWorldSpace(location_in_map);
    Vec2 offset = mouse_location - new_world_pos_of_point;
    this->setPosition(this->getPosition() + offset);

    checkAndClampPosition();
}

void BaseMap::onMouseDown(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
        is_dragging_ = true;
        last_mouse_pos_ = e->getLocation();
        last_mouse_pos_.y = Director::getInstance()->getWinSize().height - last_mouse_pos_.y; // 转换Y轴坐标
    }
}

void BaseMap::onMouseUp(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
        is_dragging_ = false;
    }
}

void BaseMap::onMouseMove(Event* event)
{
    if (!is_dragging_) return;

    EventMouse* e = (EventMouse*)event;
    Vec2 currentMousePos = e->getLocation();
    currentMousePos.y = Director::getInstance()->getWinSize().height - currentMousePos.y; // 转换Y轴坐标
    Vec2 delta = currentMousePos - last_mouse_pos_;

    this->setPosition(this->getPosition() + delta);
    checkAndClampPosition();

    last_mouse_pos_ = currentMousePos;
}
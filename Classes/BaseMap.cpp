#include "BaseMap.h"
#include <algorithm>

USING_NS_CC;

BaseMap* BaseMap::create(const std::string& filename)
{
    BaseMap* sprite = new (std::nothrow) BaseMap();
    if (sprite && sprite->initWithFile(filename)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool BaseMap::initWithFile(const std::string& filename)
{
    // 调用父类初始化
    if (!Sprite::initWithFile(filename)) {
        return false;
    }

    // 初始化变量
    is_dragging_ = false;
    this->setAnchorPoint(Vec2(0.5f, 0.5f)); // 强制中心锚点

    // 创建并绑定监听器
    mouse_listener_ = EventListenerMouse::create();
    mouse_listener_->onMouseScroll = CC_CALLBACK_1(BaseMap::onMouseScroll, this);
    mouse_listener_->onMouseDown = CC_CALLBACK_1(BaseMap::onMouseDown, this);
    mouse_listener_->onMouseUp = CC_CALLBACK_1(BaseMap::onMouseUp, this);
    mouse_listener_->onMouseMove = CC_CALLBACK_1(BaseMap::onMouseMove, this);

    // 将监听器绑定到当前节点
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse_listener_, this);

    return true;
}

void BaseMap::onEnter()
{
    Sprite::onEnter();

    // 获取屏幕大小
    auto visible_size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 初始居中
    this->setPosition(Vec2(visible_size.width / 2 + origin.x, visible_size.height / 2 + origin.y));

    // 计算并应用最小缩放 (填满屏幕)
    Size map_size = this->getContentSize();
    float scale_x = visible_size.width / map_size.width;
    float scale_y = visible_size.height / map_size.height;
    float min_scale = std::max(scale_x, scale_y);

    if (this->getScale() < min_scale) {
        this->setScale(min_scale);
    }

    // 确保初始状态不出界
    checkAndClampPosition();
}

void BaseMap::checkAndClampPosition()
{
    auto visible_size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 获取当前实际尺寸
    Size map_size = this->getContentSize();
    float scale = this->getScale();
    float actual_width = map_size.width * scale;
    float actual_height = map_size.height * scale;

    // 计算中心点允许的范围
    float max_x = origin.x + actual_width / 2.0f;
    float min_x = origin.x + visible_size.width - actual_width / 2.0f;
    float max_y = origin.y + actual_height / 2.0f;
    float min_y = origin.y + visible_size.height - actual_height / 2.0f;

    Vec2 current_pos = this->getPosition();

    // X轴钳制
    if (min_x > max_x) current_pos.x = origin.x + visible_size.width / 2.0f; // 如果图比屏小(理论不应发生)，居中
    else current_pos.x = std::max(min_x, std::min(current_pos.x, max_x));

    // Y轴钳制
    if (min_y > max_y) current_pos.y = origin.y + visible_size.height / 2.0f;
    else current_pos.y = std::max(min_y, std::min(current_pos.y, max_y));

    this->setPosition(current_pos);
}

void BaseMap::onMouseScroll(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    float scroll_y = e->getScrollY();
    if (scroll_y == 0) return;

    Vec2 mouse_location = e->getLocation();
    mouse_location.y = Director::getInstance()->getWinSize().height - mouse_location.y; // 转换Y轴坐标
    Vec2 location_in_node = this->convertToNodeSpace(mouse_location);

    float oldScale = this->getScale();
    float factor = 1.1f;
    float newScale = (scroll_y < 0) ? (oldScale * factor) : (oldScale / factor);

    // 动态计算最小缩放
    auto visible_size = Director::getInstance()->getVisibleSize();
    Size map_size = this->getContentSize();
    float min_scale = std::max(visible_size.width / map_size.width, visible_size.height / map_size.height);

    newScale = std::max(min_scale, std::min(newScale, 3.0f));

    this->setScale(newScale);

    // 位置补偿
    Vec2 newLocationInWorld = this->convertToWorldSpace(location_in_node);
    Vec2 offset = mouse_location - newLocationInWorld;
    this->setPosition(this->getPosition() + offset);

    checkAndClampPosition();
}

void BaseMap::onMouseDown(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
        is_dragging_ = true;
        last_mouse_pos_ = e->getLocationInView();
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
#include <vector>
#include "Arch.h"
#include "CoordAdaptor.h"

#include "ui/CocosGUI.h"
USING_NS_CC;

Arch* Arch::create(const ArchData& data, BaseMap* base_map)
{
    Arch* pRet = new(std::nothrow) Arch(data, base_map);
    if (pRet && pRet->initWithFile(kArchInfo.at(data.no_)[data.level_ - 1].image_)) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool Arch::initWithFile(const std::string& filename)
{
    if (!Sprite::initWithFile(filename)) {
        return false;
    }
    setAnchorPoint(Vec2(0.5f, 0.4f));
    unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
    float scale = 1.5f * CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(size, 0)).x / this->getContentSize().width;
    setScale(scale);
    setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(x_ + size / 2.0f, y_ + size / 2.0f)));
    base_map_->sprites_.push_back(this);
    base_map_->addChild(base_map_->sprites_.back(), 2);
    

    // 添加触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(Arch::onTouchDown, this);
    listener->onTouchMoved = CC_CALLBACK_2(Arch::onTouchMove, this);
    listener->onTouchEnded = CC_CALLBACK_2(Arch::onTouchUp, this);
    listener->onTouchCancelled = CC_CALLBACK_2(Arch::onTouchCancel, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

bool Arch::onTouchDown(Touch* touch, Event* event)
{
    Vec2 pos = this->convertToNodeSpace(touch->getLocation());

    if (pos.x >= 0 && pos.x <= getContentSize().width && pos.y >= 0 && pos.y <= getContentSize().height) {
        is_dragging_ = false;
        touch_start_pos_ = touch->getLocation();
        this->setLocalZOrder(100); // 拖动时置顶
        base_map_->setInputEnabled(false); // 临时禁用地图拖动
        return true;
    }
    return false;
}

void Arch::onTouchUp(Touch* touch, Event* event)
{
    this->setLocalZOrder(2); // 恢复层级
    base_map_->setInputEnabled(true); // 恢复地图拖动
    if (!is_dragging_) {
        showArchPanel(this);
    }
    else {
        is_dragging_ = false;
    }
}

void Arch::onTouchMove(Touch* touch, Event* event)
{
    if (touch->getLocation().distance(touch_start_pos_) > 10.0f) {
        is_dragging_ = true;
    }

    if (is_dragging_) {
        // 获取触摸点在 BaseMap 中的位置
        Vec2 touchInMap = base_map_->convertToNodeSpace(touch->getLocation());

        // 转换为格子坐标
        Vec2 cellPos = CoordAdaptor::pixelToCell(base_map_, touchInMap);

        // 建筑大小
        unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;

        // 计算新的左下角坐标 (四舍五入吸附)
        int newX = static_cast<int>(std::round(cellPos.x - size / 2.0f));
        int newY = static_cast<int>(std::round(cellPos.y - size / 2.0f));

        // 边界检查
        if (newX < 0) newX = 0;
        if (newY < 0) newY = 0;
        if (newX > MAP_SIZE - size) newX = MAP_SIZE - size;
        if (newY > MAP_SIZE - size) newY = MAP_SIZE - size;

        // 更新位置
        if (newX != x_ || newY != y_) {
            x_ = static_cast<unsigned char>(newX);
            y_ = static_cast<unsigned char>(newY);
            this->setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(x_ + size / 2.0f, y_ + size / 2.0f)));
        }
    }
}

void Arch::onTouchCancel(Touch* touch, Event* event)
{
    this->setLocalZOrder(2);
    base_map_->setInputEnabled(true);
    is_dragging_ = false;
}

void Arch::showArchPanel(Arch* arch)
{
    auto bg = LayerColor::create(Color4B(220, 220, 200, 180));
    bg->setContentSize(Size(400, 300));
    bg->setPosition(Vec2(90,130));
    bg->setGlobalZOrder(99);
    this->addChild(bg, 10, "ARCH_PANEL");

    // 创建面板容器
    auto panel = cocos2d::ui::Layout::create();
    panel->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    panel->setBackGroundColor(Color3B(50, 50, 50));
    panel->setBackGroundColorOpacity(200);
    panel->setContentSize(Size(340, 250));
    panel->setPosition(Vec2(30, 10));
    panel->setScale(0.8f); // 初始缩小
    panel->setOpacity(0);  // 初始透明
    panel->setGlobalZOrder(100);
    bg->addChild(panel);

    const auto& info = kArchInfo.at(arch->no_)[arch->level_ - 1];
    
    auto label = Label::createWithSystemFont(
        getArchNameFromEnum(arch->no_)+"\n------------------\n" + 
        ("等级: " + std::to_string(arch->level_) + "\n") +
        ("生命值: " + std::to_string(arch->current_hp_) + "\n") +
        (info.type_ == RESOURCE ?
            "容量: " + std::to_string(arch->current_capacity_) : "\n"),
        "Arial", 22);
    label->setPosition(Vec2(160, 150));
    panel->addChild(label);

    //关闭按钮
    auto closeBtn = cocos2d::ui::Button::create();
    closeBtn->setTitleText("关闭");
    closeBtn->setTitleFontSize(24);
    closeBtn->setPosition(Vec2(160, 40));
    closeBtn->addClickEventListener([=](Ref*) {
        this->removeChildByName("ARCH_PANEL");
        });
    panel->addChild(closeBtn);
    // 执行显示动画序列
    auto showSequence = Sequence::create(
        // 第一步：淡入遮罩
        CallFunc::create([bg]() {
            bg->setOpacity(255);
            }),

        // 第二步：面板缩放和淡入动画
        Spawn::create(
            ScaleTo::create(0.2f, 1.0f),      // 放大到正常大小
            FadeIn::create(0.2f),            // 淡入
            EaseBackOut::create(MoveBy::create(0.2f, Vec2(0, 20))), // 轻微弹跳效果
            nullptr
        ),

        // 第三步：添加轻微抖动（模拟弹出效果）
        Sequence::create(
            ScaleTo::create(0.05f, 1.02f),
            ScaleTo::create(0.05f, 1.0f),
            nullptr
        ),

        nullptr
    );

    panel->runAction(showSequence);
}

void Arch::closeArchPanel()
{
    // 移除面板
    this->removeChildByName("ARCH_PANEL");
}

std::string Arch::getArchNameFromEnum(unsigned char archNo)
{
    switch (archNo) {
        case TOWN_HALL: return "大本营";
        case WALL: return "城墙";
        case GOLD_STORAGE: return "金库";
        case ELIXIR_STORAGE: return "圣水罐";
        case GOLD_MINE: return "金矿";
        case ELIXIR_COLLECTOR: return "圣水收集器";
        case BARRACKS: return "训练营";
        case ARMY_CAMP: return "兵营";
        case CANNON: return "加农炮";
        case ARCHER_TOWER: return "箭塔";
        default: return "未知建筑";
    }
}
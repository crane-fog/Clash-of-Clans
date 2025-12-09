  #include <vector>
#include "Arch.h"
#include "BaseMap.h"
#include "CoordAdaptor.h"
#include "UIcommon.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

ArchData::ArchData(Arch* a) : no_(a->no_), level_(a->level_), x_(a->x_), y_(a->y_),
current_hp_(a->current_hp_), current_capacity_(a->current_capacity_) {}

Arch* Arch::create(const ArchData& data, BaseMap* base_map)
{
    Arch* pRet;
    if (data.no_ == WALL) {
        pRet = new(std::nothrow) Wall(data, base_map);
    }
    else {
        pRet = new(std::nothrow) Arch(data, base_map);
    }
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
    Vec2 middle_pos = Vec2(x_ + size / 2.0f, y_ + size / 2.0f);
    setScale(scale);
    setPosition(CoordAdaptor::cellToPixel(base_map_, middle_pos));
    base_map_->archs_.push_back(this);
    base_map_->addChild(this, CoordAdaptor::calcOrder(middle_pos));

    return true;
}

// todo:规范化各类中的init和onEnter
void Arch::onEnter()
{
    Sprite::onEnter();

    updateWall();

    // 添加触摸监听
    // todo:BaseMap里使用了鼠标监听，与此处的触摸监听统一化？
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(Arch::onTouchDown, this);
    listener->onTouchMoved = CC_CALLBACK_2(Arch::onTouchMove, this);
    listener->onTouchEnded = CC_CALLBACK_2(Arch::onTouchUp, this);
    listener->onTouchCancelled = CC_CALLBACK_2(Arch::onTouchCancel, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Arch::createHighlight()
{
    if (highlight_node_) return;

    highlight_node_ = Node::create();
    base_map_->addChild(highlight_node_, 0); // 层级低于建筑

    unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            auto sprite = Sprite::create("SingleCellGreen.png");
            if (sprite) {
                highlight_node_->addChild(sprite);
            }
        }
    }
    updateHighlightPos();
}

void Arch::updateHighlightPos()
{
    if (!highlight_node_) return;

    unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
    auto children = highlight_node_->getChildren();
    
    int index = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (index < children.size()) {
                auto sprite = children.at(index);
                Vec2 pos = CoordAdaptor::cellToPixel(base_map_, Vec2(x_ + i + 0.5f, y_ + j + 0.5f));
                sprite->setPosition(pos);
                index++;
            }
        }
    }
}

void Arch::updateHighlightColor(bool collision)
{
    if (!highlight_node_) return;

    std::string textureName = collision ? "SingleCellRed.png" : "SingleCellGreen.png";
    
    for (auto child : highlight_node_->getChildren()) {
        auto sprite = dynamic_cast<Sprite*>(child);
        if (sprite) {
            sprite->setTexture(textureName);
        }
    }
}

bool Arch::checkCollision(int checkX, int checkY)
{
    unsigned char my_size = kArchInfo.at(no_)[level_ - 1].size_;

    for (auto other : base_map_->archs_) {
        if (other == this) continue;

        unsigned char other_size = kArchInfo.at(other->no_)[other->level_ - 1].size_;
        unsigned char other_x = other->x_;
        unsigned char other_y = other->y_;

        bool intersect = !(checkX >= other_x + other_size ||
            checkX + my_size <= other_x ||
            checkY >= other_y + other_size ||
            checkY + my_size <= other_y);

        if (intersect) {
            return true;
        }
    }
    return false;
}

void Arch::removeHighlight()
{
    if (highlight_node_) {
        highlight_node_->removeFromParent();
        highlight_node_ = nullptr;
    }
}

bool Arch::onTouchDown(Touch* touch, Event* event)
{
    Vec2 pos = this->convertToNodeSpace(touch->getLocation());

    if (pos.x >= 0 && pos.x <= getContentSize().width && pos.y >= 0 && pos.y <= getContentSize().height) {
        is_dragging_ = false;
        touch_start_pos_ = touch->getLocation();
        original_x_ = x_;
        original_y_ = y_;
        this->setLocalZOrder(100); // 拖动时置顶
        base_map_->setInputEnabled(false); // 临时禁用地图拖动
        return true;
    }
    return false;
}

void Arch::onTouchUp(Touch* touch, Event* event)
{
    base_map_->setInputEnabled(true); // 恢复地图拖动
    removeHighlight();
    if (!is_dragging_) {
        showArchPanel(this);
    }
    else {
        // 检查碰撞
        bool collision = checkCollision(x_, y_);
        unsigned char my_size = kArchInfo.at(no_)[level_ - 1].size_;

        if (collision) {
            // 发生碰撞，回到原位
            x_ = original_x_;
            y_ = original_y_;
            this->setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(x_ + my_size / 2.0f, y_ + my_size / 2.0f)));
        }
        unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
        this->setLocalZOrder(CoordAdaptor::calcOrder(Vec2(x_ + size / 2.0f, y_ + size / 2.0f))); // 恢复并设置新层级
        is_dragging_ = false;

        if (this->no_ == WALL) {
            updateSurroundingWalls(x_, y_);
            this->updateWall();
        }
    }
}

void Arch::onTouchMove(Touch* touch, Event* event)
{
    if (touch->getLocation().distance(touch_start_pos_) > 10.0f) {
        if (!is_dragging_) {
            is_dragging_ = true;
            if (this->no_ == WALL) {
                updateSurroundingWalls(x_, y_, true);
                this->updateWall(nullptr, true);
            }
            createHighlight();
        }
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
        // todo:在上层的[44][44]中更新位置？
        if (newX != x_ || newY != y_) {
            x_ = static_cast<unsigned char>(newX);
            y_ = static_cast<unsigned char>(newY);
            this->setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(x_ + size / 2.0f, y_ + size / 2.0f)));
            updateHighlightPos();
            
            bool collision = checkCollision(x_, y_);
            updateHighlightColor(collision);
        }
    }
}

void Arch::onTouchCancel(Touch* touch, Event* event)
{
    unsigned char size = kArchInfo.at(no_)[level_ - 1].size_;
    if (is_dragging_) {
        this->setLocalZOrder(CoordAdaptor::calcOrder(Vec2(x_ + size / 2.0f, y_ + size / 2.0f)));
    }
    base_map_->setInputEnabled(true);
    is_dragging_ = false;
    removeHighlight();
}

void Arch::showArchPanel(Arch* arch)
{
    auto bg = LayerColor::create(Color4B(220, 220, 200, 180));
    bg->setContentSize(Size(400, 300));
    bg->setPosition(Vec2(90, 130));
    bg->setGlobalZOrder(99);
    this->addChild(bg, 10, "ARCH_PANEL");

    // 添加边框
    auto border = DrawNode::create();
    float borderWidth = 3.0f;  // 边框粗细

    // 基于bg的实际尺寸
    Size bgSize = bg->getContentSize();

    // 绘制边框
    draw_border(bg);

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

void Wall::updateSurroundingWalls(int x, int y, bool is_moving)
{
    for (auto arch : base_map_->archs_) {
        if (arch->getTargetType() != WALLT) continue;
        if (arch == this) continue;

        int dx = abs(static_cast<int>(arch->getx()) - x);
        int dy = abs(static_cast<int>(arch->gety()) - y);

        if (dx + dy == 1) {
            arch->updateWall(this, is_moving);
        }
    }
}

void Wall::updateWall(Arch* moving_wall, bool is_moving)
{
    // 清理旧的连接节点
    for (auto node : connection_nodes_) {
        if (node) node->removeFromParent();
    }
    connection_nodes_.clear();

    if (moving_wall == nullptr && is_moving) return;

    // 创建副本避免冲突
    std::vector<Arch*> archs_copy = base_map_->archs_;

    for (auto arch : archs_copy) {
        if (arch->getTargetType() != WALLT) continue;
        if (arch == this) continue;

        Wall* other = static_cast<Wall*>(arch);
        int dx = abs(other->x_ - x_);
        int dy = abs(other->y_ - y_);

        if (dx + dy == 1 && (other != moving_wall || !is_moving)) {
            int my_order = this->getLocalZOrder();
            int other_order = other->getLocalZOrder();

            bool i_am_owner = false;
            if (my_order < other_order) {
                i_am_owner = true;
            }
            else if (my_order == other_order) {
                if (this < other) i_am_owner = true;
            }

            if (i_am_owner) {
                auto connection_node = Node::create();

                Vec2 mid_cell((x_ + other->x_) / 2.0f + 0.5f, (y_ + other->y_) / 2.0f + 0.5f);
                Vec2 my_cell(x_ + 0.5f, y_ + 0.5f);

                Vec2 mid_pixel = CoordAdaptor::cellToPixel(base_map_, mid_cell);
                Vec2 my_pixel = CoordAdaptor::cellToPixel(base_map_, my_cell);

                float scale = this->getScale();
                if (scale == 0.0f) scale = 1.0f;

                Vec2 local_pos = (mid_pixel - my_pixel) / scale;

                Size size = this->getContentSize();
                Vec2 anchor_offset(size.width * 0.5f, size.height * 0.4f);

                connection_node->setPosition(anchor_offset + local_pos);

                this->addChild(connection_node);
                connection_nodes_.push_back(connection_node);

                Vec2 dir(static_cast<float>(other->x_ - x_), static_cast<float>(other->y_ - y_));
                Vec2 pixel_delta = CoordAdaptor::cellDeltaToPixelDelta(base_map_, dir);
                Vec2 local_delta = pixel_delta / scale;

                for (int k = 1; k <= 3; ++k) {
                    auto sprite = Sprite::create(kArchInfo.at(WALL)[level_ - 1].image_);
                    if (sprite) {
                        sprite->setScale(0.6f);
                        sprite->setPosition(local_delta * (k / 4.0f - 0.5f));
                        connection_node->addChild(sprite);
                    }
                }
            }
        }
    }
}

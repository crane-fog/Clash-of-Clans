#include <vector>
#include "Arch.h"
#include "BaseMap.h"
#include "CoordAdaptor.h"
#include "UIcommon.h"
#include "ui/CocosGUI.h"
#include"MainVillageScene.h"
USING_NS_CC;


ArchData::ArchData(Arch* a) : no_(a->no_), level_(a->level_), x_(a->x_), y_(a->y_),
current_hp_(a->current_hp_), current_capacity_(a->current_capacity_) {}

Arch* Arch::create(const ArchData& data, BaseMap* base_map, bool is_mine)
{
    Arch* pRet;
    if (data.no_ == WALL) {
        pRet = new(std::nothrow) Wall(data, base_map);
    }
    else {
        pRet = new(std::nothrow) Arch(data, base_map);
    }
    if (pRet && pRet->initWithFile(kArchInfo.at(data.no_)[data.level_ - 1].image_)) {
        pRet->is_mine_ = is_mine;
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
    if (is_mine_) {
        touch_listener_ = EventListenerTouchOneByOne::create();
        touch_listener_->setSwallowTouches(true);

        touch_listener_->onTouchBegan = CC_CALLBACK_2(Arch::onTouchDown, this);
        touch_listener_->onTouchMoved = CC_CALLBACK_2(Arch::onTouchMove, this);
        touch_listener_->onTouchEnded = CC_CALLBACK_2(Arch::onTouchUp, this);
        touch_listener_->onTouchCancelled = CC_CALLBACK_2(Arch::onTouchCancel, this);

        _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener_, this);
    }
}

void Arch::onExit()
{
    if (is_mine_ && touch_listener_) {
        _eventDispatcher->removeEventListener(touch_listener_);
        touch_listener_ = nullptr;
    }
    Sprite::onExit();
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
            this->setLocalZOrder(100); // 开始拖动时置顶
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
    // 检查面板是否已经存在，如果存在就不再创建
    if (this->getChildByName("ARCH_PANEL")) {
        CCLOG("面板已经存在，不能重复打开！");
        return; // 面板已经存在，直接返回
    }
    auto bg = LayerColor::create(Color4B(220, 220, 200, 180));
    bg->setContentSize(Size(400, 300));
    bg->setPosition(Vec2(150, 170));
    
    this->addChild(bg, 100, "ARCH_PANEL");

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
    bg->addChild(panel,100);

    const auto& info = kArchInfo.at(arch->no_)[arch->level_-1];
    
    auto label = Label::createWithSystemFont(
        getArchNameFromEnum(arch->no_)+"\n------------------\n" + 
        ("等级: " + std::to_string(arch->level_) + "\n") +
        ("当前生命值: " + std::to_string(arch->current_hp_) + "/" + std::to_string(info.hp_)+ "\n") +
        (info.type_ == RESOURCE ?
        ("当前容量: " + std::to_string(arch->current_capacity_) + "/" + std::to_string(info.max_capacity_)) : "\n") ,
        "Arial", 22);
    label->setPosition(Vec2(160, 150));
    panel->addChild(label);

    //关闭按钮
    auto closeBtn = cocos2d::ui::Button::create();
    closeBtn->setTitleText("关闭");
    closeBtn->setTitleFontSize(24);
    closeBtn->setPosition(Vec2(70, 30));
    closeBtn->addClickEventListener([=](Ref*) {
        this->removeChildByName("ARCH_PANEL");
        });
    panel->addChild(closeBtn);


    // 创建升级按钮
    auto upgradeBtn = cocos2d::ui::Button::create();
    upgradeBtn->setTitleText("升级");
    upgradeBtn->setTitleFontSize(24);
    upgradeBtn->setPosition(Vec2(230, 30));  // 设置按钮位置在左下角
    upgradeBtn->setContentSize(Size(100, 40));  // 设置按钮大小
    upgradeBtn->addClickEventListener([=](Ref*) {
        // 处理升级操作
        CCLOG("升级按钮点击");
        archUpgrade(arch);
        });
    panel->addChild(upgradeBtn);

    // 吞噬所有触摸
    panel->setTouchEnabled(true); // 启用触摸事件
    panel->setSwallowTouches(true); // 吞噬触摸事件

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

void Arch::archUpgrade(Arch* arch) {
    
    unsigned char max_ = 4;
    if (arch->level_ < max_) {
        // 创建一个新的面板显示升级前后的数据和金币提示
        createUpgradeComparisonPanel(arch);
    }
    else {
        // 弹出提示窗显示“当前已是最高等级”
        showRefusePopup("当前已是最高等级");
    }
}
// 创建显示"当前已是最高等级"的弹窗
void Arch::showRefusePopup(std::string text_) {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建背景遮罩
    auto popupBg = LayerColor::create(Color4B(0, 0, 0, 180)); // 半透明背景
    popupBg->setContentSize(Size(400, 200));
    popupBg->setPosition(Vec2(visibleSize.width / 2 - 200, visibleSize.height / 2 - 100));
    this->addChild(popupBg, 1000);  // 设置层级

    // 创建提示标签
    auto label = Label::createWithSystemFont(text_, "Arial", 30);
    label->setPosition(Vec2(popupBg->getContentSize().width / 2, popupBg->getContentSize().height / 2));
    label->setTextColor(Color4B::RED);
    popupBg->addChild(label);

    // 弹窗消失动画
    auto fadeOut = FadeOut::create(1.0f);  // 设置渐隐动画
    auto removePopup = RemoveSelf::create();  // 移除弹窗
    auto sequence = Sequence::create(fadeOut, DelayTime::create(4.0f),removePopup, nullptr);  // 延迟4秒再消失

    popupBg->runAction(sequence);  // 应用到整个弹窗（背景和文字）
    label->runAction(sequence->clone());
    
}

void Arch::createUpgradeComparisonPanel(Arch* arch) {

    // 创建背景遮罩
    auto popupBg = LayerColor::create(Color4B(255, 255, 255,255)); // 半透明背景
    popupBg->setContentSize(Size(400, 300));
    popupBg->setPosition(Vec2(150,170));
    popupBg->setTag(1000);
    this->addChild(popupBg, 1000);  // 设置层级
    // 绘制边框
    draw_border(popupBg);
    // 创建标题标签
    auto titleLabel = Label::createWithSystemFont("确认升级", "Arial", 30);
    titleLabel->setPosition(Vec2(popupBg->getContentSize().width / 2, popupBg->getContentSize().height - 40));
    titleLabel->setTextColor(Color4B::BLACK);
    popupBg->addChild(titleLabel);

    //数据
    unsigned char no_ = arch->no_;
    unsigned char currentLevel = arch->level_;

    // 创建升级前后的数据对比标签
    auto infoLabel = Label::createWithSystemFont(
        "当前等级: " + std::to_string(arch->level_) + " -> "  +std::to_string(arch->level_ +1)+ "\n" +
        "生命值  : " + std::to_string(kArchInfo.at(no_)[currentLevel-1].hp_) + " -> " + std::to_string(kArchInfo.at(no_)[currentLevel ].hp_) + "\n" +
        "容量    : " + std::to_string(kArchInfo.at(no_)[currentLevel -1].max_capacity_) + " -> " + std::to_string(kArchInfo.at(no_)[currentLevel].max_capacity_) + "\n" + "\n" + "\n" +
        "金币需求: " + std::to_string(kArchInfo.at(no_)[currentLevel ].upgrade_cost_amount_)+ "\n" +
        "升级用时: " + std::to_string(kArchInfo.at(no_)[currentLevel].upgrade_time_), "Arial", 24);
    if (kArchInfo.at(no_)[currentLevel].upgrade_cost_type_ == ELIXIR) {
        infoLabel->setString("当前等级: " + std::to_string(arch->level_) + " -> " + std::to_string(arch->level_ + 1) + "\n" +
            "生命值  : " + std::to_string(kArchInfo.at(no_)[currentLevel - 1].hp_) + " -> " + std::to_string(kArchInfo.at(no_)[currentLevel].hp_) + "\n" +
            "容量    : " + std::to_string(kArchInfo.at(no_)[currentLevel - 1].max_capacity_) + " -> " + std::to_string(kArchInfo.at(no_)[currentLevel].max_capacity_) + "\n" + "\n" + "\n" +
            "圣水需求: " + std::to_string(kArchInfo.at(no_)[currentLevel].upgrade_cost_amount_) + "\n" +
            "升级用时: " + std::to_string(kArchInfo.at(no_)[currentLevel].upgrade_time_));
    }
    infoLabel->setPosition(Vec2(popupBg->getContentSize().width / 2, popupBg->getContentSize().height / 2));
    infoLabel->setTextColor(Color4B::BLACK);
    popupBg->addChild(infoLabel);

    unsigned int cost_ = kArchInfo.at(no_)[currentLevel].upgrade_cost_amount_;
    unsigned long long current_=0;
    if (kArchInfo.at(no_)[currentLevel].upgrade_cost_type_==GOLD) {
        current_ = GameManager::getInstance()->getGold();
    }
    else {
        current_ = GameManager::getInstance()->getExilir();
        unsigned long long current_ = 0;
    }
    // 创建取消按钮
    auto cancelLabel = Label::createWithSystemFont("取消", "Arial", 30);
    cancelLabel->setTextColor(Color4B::RED);  // 设置字体颜色为红色
    auto cancelButton = MenuItemLabel::create(
        cancelLabel,
        CC_CALLBACK_1(Arch::onUpgradeCancel, this));
    cancelButton->setPosition(Vec2(popupBg->getContentSize().width / 3, 30));

    // 创建确认按钮
    auto confirmLabel = Label::createWithSystemFont("确认", "Arial", 30);
    confirmLabel->setTextColor(Color4B::GREEN);  // 设置字体颜色为红色
    auto confirmButton = MenuItemLabel::create(
        confirmLabel,
        CC_CALLBACK_1(Arch::Buiding_Upgrading, this, arch, UPGRADING, cost_, current_, kArchInfo.at(no_)[currentLevel].upgrade_cost_type_));
    confirmButton->setPosition(Vec2(popupBg->getContentSize().width * 2 / 3, 30));

    // 将按钮添加到菜单中
    auto menu = Menu::create(cancelButton, confirmButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    popupBg->addChild(menu);
}

void Arch::onUpgradeCancel(Ref* sender) {
    // 关闭升级面板
    this->removeChildByTag(1000);  // 1000是面板的tag，可以根据需要调整
}

void Arch::Buiding_Upgrading(Ref* sender, Arch* arch,bool a, unsigned int cost, unsigned long long currentGold,bool type) {
    if (cost > currentGold) {
        if (type == GOLD) {
            // 如果金币不足，显示金币不足的弹窗
            showRefusePopup("金币不足，无法升级！");
        }
        else {
            showRefusePopup("圣水不足，无法升级！");
        }
    }
    else {
        // 如果金币足够，进行升级操作
        if (type == GOLD) {
            GameManager::getInstance()->setGold(currentGold - cost);  // 减少金币
        }
        else {
            GameManager::getInstance()->setElixir(currentGold - cost);  // 减少圣水
        }
        std::string Notice_;
        // 执行升级逻辑
        if (a) {
            arch->level_++;
            Notice_ = "升级";
        }
        else {
            Notice_ = "建造";
        }

        arch->current_hp_ = kArchInfo.at(arch->no_)[arch->level_ - 1].hp_;
        //更新图片纹理
        auto newImg = kArchInfo.at(no_)[level_ - 1].image_;
        // 获取升级时间（持续的总时长）
        unsigned int upgradeTime = kArchInfo.at(arch->no_)[arch->level_ - 1].upgrade_time_;

        // 添加亮暗效果的动画
        auto fadeOut = FadeTo::create(0.5f, 50);  // 1秒内从亮变暗
        auto fadeIn = FadeTo::create(0.5f, 255);   // 1秒内从暗变亮
        auto sequence = Sequence::create(fadeOut, fadeIn, nullptr);  // 交替执行淡入淡出

        // 关闭面板
        this->removeChildByTag(1000);
        this->removeChildByName("ARCH_PANEL");
        // 执行动画
        this->runAction(Repeat::create(sequence, upgradeTime));
        if (upgradeTime) {
            // 创建升级标签
            auto upgradeLabel = Label::createWithSystemFont(
                Notice_ + "中...还需: " + std::to_string(upgradeTime) + " 秒 ", "Arial", 22);
            upgradeLabel->setPosition(Vec2(120, 200));
            upgradeLabel->setTextColor(Color4B::BLACK);
            upgradeLabel->setName("upgrading");
            this->addChild(upgradeLabel);
            auto timer = CountdownTimer::create();
            this->addChild(timer);
            timer->start(upgradeTime,
                [Notice_, this, upgradeLabel](int remaining) {
                    // 每秒回调
                    upgradeLabel->setString(Notice_ + "中...还需: " + std::to_string(remaining) + " 秒");
                },
                [Notice_, this, upgradeLabel]() {
                    // 完成回调
                    upgradeLabel->setString(Notice_ + "完成！");
                    this->removeChildByName("upgrading");
                }
            );
        }
        // 执行升级动画完成后的操作
        scheduleOnce([=](float) {
            // 更新建筑纹理
            arch->setTexture(newImg);
            // 更新UI显示
            showArchPanel(arch);
            }, upgradeTime, "UpdateArchPanel");
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

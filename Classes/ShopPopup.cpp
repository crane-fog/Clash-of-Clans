#include "ShopPopup.h"
#include"UIparts.h"
#include"UIcommon.h"
USING_NS_CC;
using namespace ui;
// 商品数据结构
ui::Button* createShopButton(const ShopItem& item, int index);

void ShopPopup::setupBackground() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建背景层（继承自 Layer，可以接收触摸）
    auto background = Layer::create();
    background->setContentSize(visibleSize);

    // 添加半透明黑色背景
    auto colorBg = LayerColor::create(Color4B(0, 0, 0, 180)); // 深色半透明
    colorBg->setContentSize(visibleSize);
    background->addChild(colorBg);

    // 添加触摸监听器 - 拦截所有触摸事件
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true); // 吞噬触摸，不传递到下层

    touchListener->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event) {
        // 点击背景的任何位置都返回 true，表示处理这个事件
        return true;
        };

    touchListener->onTouchEnded = [this](cocos2d::Touch* touch, cocos2d::Event* event) {
        // 可以添加点击背景关闭的功能（可选）
        // auto location = touch->getLocation();
        // 检查是否点击在面板外
        };

    // 注意：监听器要附加到 background（Layer），不是 colorBg（LayerColor）
    background->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, background);

    this->addChild(background, -1);
}
// 购买商品的处理函数
void onPurchaseItem(int itemId) {
    CCLOG("Processing purchase for item %d", itemId);

    // 这里可以添加实际的购买逻辑，如：
    // - 扣除金币
    // - 更新UI
    // - 发送网络请求等

    // 示例：显示购买成功提示
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto successLabel = Label::createWithSystemFont(
        "Purchase successful! Item " + std::to_string(itemId),
        "Arial", 24
    );
    successLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 100));
    successLabel->setColor(Color3B::GREEN);

    // 2秒后消失
    successLabel->runAction(Sequence::create(
        DelayTime::create(2.0f),
        RemoveSelf::create(),
        nullptr
    ));
}
bool ShopPopup::init()
{
    if (!Layer::init()) {
        return false;
    }

    // 定义三个板块的商品数据
    std::vector<ShopItem> buildingItems = {
        {1, "大本营", 100, true, "", "arch/Town_Hall1.webp"},
        {2, "城墙", 150, false, "等级不足", "arch/Wall1.webp"},
        {3, "金库", 50, true, "", "arch/Gold_Storage1.webp"},
        {4, "圣水罐", 200, false, "金币不足", "arch/Elixir_Storage1.webp"},
        {5, "金矿", 120, true, "", "arch/Gold_Mine1.webp"},
        {6, "圣水收集器", 180, false, "需要完成前置任务", "arch/Elixir_Collector1.webp"},
        {7, "箭塔", 250, false, "VIP only", "arch/Archer_Tower1.webp"},
        {8, "加农炮", 250, false, "VIP only", "arch/Cannon1.webp"}
    };

    std::vector<ShopItem> soldierItems = {
        {101, "野蛮人", 30, true, "", "Barbarian.png"},
        {102, "弓箭手", 45, true, "", "Barbarian.png"},
        {103, "巨人", 150, false, "需要2级兵营", "Barbarian.png"},
        {104, "哥布林", 25, true, "", "Barbarian.png"},
        {105, "炸弹人", 50, false, "需要完成训练", "Barbarian.png"},

    };

    std::vector<ShopItem> gachaItems = {
        {201, "神秘宝箱", 1000, true, "有机会获得稀有物品！", "lucky.png"}
    };

    auto visibleSize = Director::getInstance()->getVisibleSize();

    setupBackground();

    // 面板背景
    auto panelBg = LayerColor::create(Color4B(255, 204, 153, 255),
        visibleSize.width * 0.8f,
        visibleSize.height * 0.6f);
    panelBg->setPosition(Vec2(visibleSize.width * 0.1f, visibleSize.height * 0.2f));
    this->addChild(panelBg);

    // 添加边框
    draw_border(panelBg, 5.0f, Color4F(0.0, 0.0, 0.0, 1.0));

    // 标题
    auto title = Label::createWithSystemFont("商店", "Arial", 70);
    title->setColor(Color3B(0, 0, 0));
    title->setPosition(Vec2(panelBg->getContentSize().width / 2,
        panelBg->getContentSize().height - 50));
    panelBg->addChild(title);

    // 计算相对于panelBg的位置
    float btnX = panelBg->getContentSize().width / 20;
    float btnY = panelBg->getContentSize().height - 50;

    // 关闭按钮背景
    auto closeBtnBg = LayerColor::create(Color4B(160, 180, 230, 255), 80, 40);
    closeBtnBg->setAnchorPoint(Vec2(0.5f, 0.5f));
    closeBtnBg->setPosition(Vec2(btnX - 40, btnY - 20));
    panelBg->addChild(closeBtnBg);

    // 关闭按钮
    auto closeBtn = Button::create();
    closeBtn->setTitleText("关闭");
    closeBtn->setTitleColor(Color3B(0, 0, 0));
    closeBtn->setTitleFontSize(20);
    closeBtn->setContentSize(Size(80, 40));
    closeBtn->setAnchorPoint(Vec2(0.5f, 0.5f));
    closeBtn->setPosition(Vec2(btnX, btnY));
    closeBtn->addTouchEventListener(CC_CALLBACK_2(ShopPopup::onClose, this));
    panelBg->addChild(closeBtn, 9999);

    // 创建选项卡按钮
    float tabStartY = panelBg->getContentSize().height - 120;
    float tabWidth = panelBg->getContentSize().width / 3 - 20;

    // 建筑选项卡
    auto buildingTab = ui::Button::create();
    buildingTab->setTitleText("建筑");
    buildingTab->setTitleFontSize(28);
    buildingTab->setTitleColor(Color3B::WHITE);
    buildingTab->setContentSize(Size(tabWidth, 60));
    buildingTab->setPosition(Vec2(tabWidth / 2 + 10, tabStartY));
    buildingTab->setColor(Color3B(100, 150, 200)); // 蓝色
    buildingTab->setTag(1); // 标记为建筑标签
    buildingTab->addTouchEventListener([this, buildingTab](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->switchToTab(1); // 切换到建筑标签
        }
        });
    panelBg->addChild(buildingTab, 10);

    // 士兵选项卡
    auto soldierTab = ui::Button::create();
    soldierTab->setTitleText("士兵");
    soldierTab->setTitleFontSize(28);
    soldierTab->setTitleColor(Color3B::WHITE);
    soldierTab->setContentSize(Size(tabWidth, 60));
    soldierTab->setPosition(Vec2(tabWidth * 1.5 + 20, tabStartY));
    soldierTab->setColor(Color3B(150, 100, 200)); // 紫色
    soldierTab->setTag(2); // 标记为士兵标签
    soldierTab->addTouchEventListener([this, soldierTab](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->switchToTab(2); // 切换到士兵标签
        }
        });
    panelBg->addChild(soldierTab, 10);

    // 抽卡选项卡
    auto gachaTab = ui::Button::create();
    gachaTab->setTitleText("抽卡");
    gachaTab->setTitleFontSize(28);
    gachaTab->setTitleColor(Color3B::WHITE);
    gachaTab->setContentSize(Size(tabWidth, 60));
    gachaTab->setPosition(Vec2(tabWidth * 2.5 + 30, tabStartY));
    gachaTab->setColor(Color3B(200, 150, 100)); // 橙色
    gachaTab->setTag(3); // 标记为抽卡标签
    gachaTab->addTouchEventListener([this, gachaTab](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->switchToTab(3); // 切换到抽卡标签
        }
        });
    panelBg->addChild(gachaTab, 10);

    // 创建滚动容器（初始显示建筑）
    auto scrollView = ui::ScrollView::create();
    scrollView->setContentSize(Size(panelBg->getContentSize().width - 40,
        panelBg->getContentSize().height)); // 调整高度为选项卡留出空间

    scrollView->setInnerContainerSize(Size(270 * buildingItems.size(),
        panelBg->getContentSize().height));
    scrollView->setDirection(ui::ScrollView::Direction::HORIZONTAL);
    scrollView->setPosition(Vec2(20, 100));
    scrollView->setScrollBarEnabled(true);
    scrollView->setScrollBarPositionFromCorner(Vec2(2, 2));
    scrollView->setScrollBarWidth(20);
    scrollView->setScrollBarColor(Color3B::BLACK);
    scrollView->setTag(100); // 给滚动容器设置tag以便后续查找
    panelBg->addChild(scrollView);

    // 保存商品数据供切换使用
    currentTab_ = 1; // 默认显示建筑
    buildingItems_ = buildingItems;
    soldierItems_ = soldierItems;
    gachaItems_ = gachaItems;
    scrollView_ = scrollView; // 保存滚动容器引用

    // 初始显示建筑商品
    showItemsInScrollView(buildingItems, scrollView);

    return true;
}

// 切换到指定标签的函数
void ShopPopup::switchToTab(int tabIndex) {
    if (currentTab_ == tabIndex) {
        return; // 已经是当前标签，不切换
    }

    // 更新当前标签
    currentTab_ = tabIndex;

    // 获取滚动容器
    auto scrollView = scrollView_;
    if (!scrollView) return;

    // 清空滚动容器
    scrollView->removeAllChildren();

    // 根据标签显示不同的商品
    switch (tabIndex) {
        case 1: // 建筑
            scrollView->setInnerContainerSize(Size(270 * buildingItems_.size(),
                scrollView->getContentSize().height));
            showItemsInScrollView(buildingItems_, scrollView, tabIndex);
            break;
        case 2: // 士兵
            scrollView->setInnerContainerSize(Size(270 * soldierItems_.size(),
                scrollView->getContentSize().height));
            showItemsInScrollView(soldierItems_, scrollView, tabIndex);
            break;
        case 3: // 抽卡
            scrollView->setInnerContainerSize(Size(270 * gachaItems_.size(),
                scrollView->getContentSize().height));
            showItemsInScrollView(gachaItems_, scrollView, tabIndex);
            break;
    }

    // 滚动到最左边
    scrollView->scrollToPercentHorizontal(0, 0.3f, true);
}

// 在滚动容器中显示商品的辅助函数
void ShopPopup::showItemsInScrollView(const std::vector<ShopItem>& items, ui::ScrollView* scrollView, int tabIndex) {
    auto scrollBg = LayerColor::create(Color4B(255, 255, 255, 255), 270 * buildingItems_.size(),
                scrollView->getContentSize().height-250);
    scrollBg->setPosition(Vec2::ZERO);
    scrollBg->setLocalZOrder(-1); // 放在最底层
    // 将背景添加到滚动视图
    scrollView->addChild(scrollBg);
    for (int i = 0; i < items.size(); i++) {
        const auto& item = items[i];

        // 商品背景
        auto itemBg = LayerColor::create(Color4B(160, 180, 230, 255), 250, 300);
        itemBg->setPosition(Vec2(20 + i * 270, 20));
        scrollView->addChild(itemBg);

        // 设置触摸事件
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);

        listener->onTouchBegan = [this, itemBg, item,scrollView](Touch* touch, Event* event) -> bool {
            Vec2 locationInNode = itemBg->convertToNodeSpace(touch->getLocation());
            Size size = itemBg->getContentSize();
            Rect rect = Rect(0, 0, size.width, size.height);

            if (rect.containsPoint(locationInNode)) {
                if (item.isAvailable) {
                    itemBg->setColor(Color3B(120, 140, 180)); // 按下变暗
                }
                else {
                    this->showUnavailableBubble(item, itemBg, scrollView);
                }
                return true;
            }
            return false;
            };

        listener->onTouchEnded = [itemBg, i, item](Touch* touch, Event* event) {
            itemBg->setColor(Color3B(160, 180, 230)); // 恢复颜色

            Vec2 locationInNode = itemBg->convertToNodeSpace(touch->getLocation());
            Size size = itemBg->getContentSize();
            Rect rect = Rect(0, 0, size.width, size.height);

            if (rect.containsPoint(locationInNode)) {
                CCLOG("Item %d clicked: %s", i + 1, item.name.c_str());
                if (item.isAvailable) {
                    onPurchaseItem(item.id);
                }
            }
            };

        listener->onTouchCancelled = [itemBg](Touch* touch, Event* event) {
            itemBg->setColor(Color3B(160, 180, 230));
            };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, itemBg);

        // 如果商品不可用，添加灰色遮罩
        if (!item.isAvailable) {
            Size bgSize = itemBg->getContentSize();
            auto grayMask = LayerColor::create(Color4B(128, 128, 128, 150), bgSize.width, bgSize.height);
            grayMask->setPosition(Vec2::ZERO);
            itemBg->addChild(grayMask, 9999);
        }

        // 商品图片
        Sprite* itemImage = nullptr;
        if (!item.imagePath.empty()) {
            itemImage = Sprite::create(item.imagePath);
            if (itemImage) {
                float maxSize = 220.0f; // 调整图片大小
                float scale = std::min(maxSize / itemImage->getContentSize().width,
                    maxSize / itemImage->getContentSize().height);
                itemImage->setScale(scale);
                itemImage->setPosition(Vec2(itemBg->getContentSize().width / 2,
                    itemBg->getContentSize().height - 140));
                itemBg->addChild(itemImage);
            }
            else {
                itemImage = Sprite::create("ui/placeholder.png");
                itemImage->setScale(0.5f);
                itemImage->setPosition(Vec2(itemBg->getContentSize().width / 2,
                    itemBg->getContentSize().height - 100));
                itemBg->addChild(itemImage);
            }
        }

        // 商品标签
        auto itemLabel = Label::createWithSystemFont(item.name, "Arial", 26);
        itemLabel->setPosition(Vec2(itemBg->getContentSize().width / 2,
            itemBg->getContentSize().height - 30));
        itemLabel->setColor(Color3B::BLACK);
        itemBg->addChild(itemLabel);
        if (tabIndex == 3) {
            // 商品价格图标
            auto goldIcon = Sprite::create("Elixir.png");
            goldIcon->setPosition(Vec2(itemBg->getContentSize().width / 3, 30));
            goldIcon->setScale(0.5f);
            itemBg->addChild(goldIcon);

            // 商品价格
            auto priceLabel = Label::createWithSystemFont("$" + std::to_string(item.price), "Arial", 25);
            priceLabel->setPosition(Vec2(itemBg->getContentSize().width / 2 + 10, 30));
            priceLabel->setColor(Color3B::MAGENTA);
            itemBg->addChild(priceLabel);
        }
        else {
            // 商品价格图标
            auto goldIcon = Sprite::create("Gold.png");
            goldIcon->setPosition(Vec2(itemBg->getContentSize().width / 3, 30));
            goldIcon->setScale(0.5f);
            itemBg->addChild(goldIcon);

            // 商品价格
            auto priceLabel = Label::createWithSystemFont("$" + std::to_string(item.price), "Arial", 25);
            priceLabel->setPosition(Vec2(itemBg->getContentSize().width / 2 + 10, 30));
            priceLabel->setColor(Color3B::YELLOW);
            itemBg->addChild(priceLabel);
        }
        // 购买按钮
        auto buyButton = createShopButton(item, i);
        buyButton->setPosition(Vec2(itemBg->getContentSize().width / 2, 80));
        itemBg->addChild(buyButton);
    }
}
// 创建商品购买按钮的函数
ui::Button* createShopButton(const ShopItem& item, int index) {
    auto button = ui::Button::create();
    button->setTitleText("BUY");
    button->setTitleFontSize(25);
    button->setContentSize(Size(100, 40));

    // 根据可用性设置样式
    if (item.isAvailable) {
        button->setTitleColor(Color3B::WHITE);
        button->setColor(Color3B(0, 200, 0)); // 绿色
    }
    else {
        button->setTitleColor(Color3B::GRAY);
        button->setColor(Color3B(100, 100, 100)); // 灰色
        button->setEnabled(false);
    }

    // 触摸事件
    button->addTouchEventListener([item, index](Ref* sender, ui::Widget::TouchEventType type) {
        auto button = static_cast<ui::Button*>(sender);

        switch (type) {
            case ui::Widget::TouchEventType::BEGAN:
                if (button->isEnabled()) {
                    button->runAction(ScaleTo::create(0.1f, 0.9f));
                }
                break;

            case ui::Widget::TouchEventType::ENDED:
                if (button->isEnabled()) {
                    // 回弹动画
                    button->runAction(Sequence::create(
                        ScaleTo::create(0.1f, 1.1f),
                        ScaleTo::create(0.1f, 1.0f),
                        nullptr
                    ));

                    // 触发购买事件
                    onPurchaseItem(index);
                }
                else {
                    // 显示禁用提示
                    
                }
                break;

            case ui::Widget::TouchEventType::CANCELED:
                if (button->isEnabled()) {
                    button->runAction(ScaleTo::create(0.1f, 1.0f));
                }
                break;

            default:
                break;
        }
        });

    return button;
}
void ShopPopup::show(Node* parent)
{
    parent->addChild(this, 100);

    // 显示动画
    this->setScale(0.1f);
    auto scaleTo = ScaleTo::create(0.3f, 1.0f);
    auto easeOut = EaseBackOut::create(scaleTo);
    this->runAction(easeOut);
    // 关闭地图输入
    auto map = parent->getChildByName("BaseMap");
    if (map) ((BaseMap*)map)->setInputEnabled(false);

    this->setScale(0.1f);
    this->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1.0f)));
}
//关闭商店面板
void ShopPopup::close()
{
    auto scaleTo = ScaleTo::create(0.2f, 0.1f);
    auto easeIn = EaseBackIn::create(scaleTo);
    auto remove = RemoveSelf::create();
    auto sequence = Sequence::create(easeIn, remove, nullptr);
    this->runAction(sequence);
    // 重新启用地图输入 
    auto parent = this->getParent();
    auto map = parent->getChildByName("BaseMap");
    if (map) ((BaseMap*)map)->setInputEnabled(true);

    this->runAction(Sequence::create(
        EaseBackIn::create(ScaleTo::create(0.2f, 0.1f)),
        RemoveSelf::create(), nullptr));
}

void ShopPopup::onClose(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED) {
        close();
    }
}

void ShopPopup::showUnavailableBubble(const ShopItem& item, cocos2d::LayerColor* targetNode, cocos2d::ui::ScrollView* scrollView) {

    // 创建提示气泡（相对于商品背景的本地坐标）
    auto bubble = Node::create();
    bubble->setPosition(Vec2(targetNode->getContentSize().width / 2,
        targetNode->getContentSize().height + 10)); // 在商品上方显示
    bubble->setTag(8888);
    targetNode->addChild(bubble, 999); // 添加到商品背景，而不是this

    // 气泡背景
    auto bubbleBg = LayerColor::create(Color4B(70, 70,70, 2020), 200, 80);
    bubbleBg->setPosition(Vec2(-100, 0)); // 居中
    bubble->addChild(bubbleBg);

    // 三角形箭头（指向商品）
    auto arrow = DrawNode::create();
    Vec2 arrowPoints[] = {
        Vec2(90, 80),    // 左下
        Vec2(100, 90),   // 顶点
        Vec2(110, 80)    // 右下
    };
    arrow->drawSolidPoly(arrowPoints, 3, Color4F(0.2f, 0.2f, 0.2f, 0.9f));
    arrow->setPosition(Vec2(0, 0));
    bubble->addChild(arrow);

    // 原因文本
    auto reasonLabel = Label::createWithSystemFont(
        item.unavailableReason,
        "Arial", 25
    );
    reasonLabel->setColor(Color3B::RED);
    reasonLabel->setPosition(Vec2(100, 40));
    reasonLabel->setWidth(180);
    reasonLabel->setAlignment(TextHAlignment::CENTER);
    bubbleBg->addChild(reasonLabel);

    // 初始缩放动画
    bubble->setScale(0.1f);
    bubble->runAction(Sequence::create(
        ScaleTo::create(0.2f, 1.0f),
        DelayTime::create(5.0f),
        Spawn::create(
            FadeOut::create(0.3f),
            ScaleTo::create(0.3f, 0.5f),
            nullptr
        ),
        RemoveSelf::create(),
        nullptr
    ));
}
// 在按钮点击事件中
void ShopPopup::onShopButtonClick(Ref* sender)
{
    auto popup = ShopPopup::create();
    popup->show(this);
}
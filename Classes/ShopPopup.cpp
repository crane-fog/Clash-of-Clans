#include "ShopPopup.h"
#include"UIparts.h"
#include"UIcommon.h"
USING_NS_CC;
using namespace ui;
// 商品数据结构


ui::Button* createShopButton(const ShopItem& item, int index);
// 显示禁用提示的函数
void showUnavailablePrompt(int itemId) {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建提示背景
    auto promptBg = LayerColor::create(Color4B(0, 0, 0, 200), 300, 100);
    promptBg->setPosition(Vec2(visibleSize.width / 2 - 150, visibleSize.height / 2 - 50));
    promptBg->setTag(999); // 设置tag便于移除

    // 提示文字
    auto promptLabel = Label::createWithSystemFont(
        "Item " + std::to_string(itemId) + " is unavailable!",
        "Arial", 20
    );
    promptLabel->setPosition(Vec2(150, 60));
    promptLabel->setColor(Color3B::WHITE);
    promptBg->addChild(promptLabel);

    // 确定按钮
    auto okButton = ui::Button::create();
    okButton->setTitleText("OK");
    okButton->setTitleFontSize(16);
    okButton->setContentSize(Size(80, 30));
    okButton->setPosition(Vec2(150, 25));
    okButton->addTouchEventListener([promptBg](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            // 移除提示
            promptBg->removeFromParent();
        }
        });
    promptBg->addChild(okButton);


    // 3秒后自动消失
    promptBg->runAction(Sequence::create(
        DelayTime::create(3.0f),
        RemoveSelf::create(),
        nullptr
    ));
}
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

    // 模拟商品数据
    std::vector<ShopItem> shopItems = {
        {1, "大本营", 100, true, "","arch/Archer_Tower1.webp"},
        {2, "城墙", 150, false, "Level too low","arch/Wall1.webp"},
        {3, "金库", 50, true, "","arch/Gold_Storage1.webp"},
        {4, "圣水罐", 200, false, "Insufficient gold","arch/Elixir_Storage1.webp"},
        {5, "金矿", 120, true, "","arch/Gold_Mine1.webp"},
        {6, "圣水收集器", 180, false, "Quest required","arch/Elixir_Collector1.webp"},
        {7, "训练营", 80, true, "","arch/Barracks1.webp"},
        {8, "兵营", 250, false, "VIP only","arch/Army_Camp1.webp"},
        {9, "加农炮", 250, false, "VIP only","arch/Cannon1.webp"},
        {10, "箭塔", 250, false, "VIP only","arch/Archer_Tower1.webp"}

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
    draw_border(panelBg, 5.0f,Color4F(0.0,0.0,0.0,1.0));
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
    closeBtnBg->setPosition(Vec2(btnX-40, btnY-20));
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
    panelBg->addChild(closeBtn,9999);


    // 创建滚动容器
    auto scrollView = ui::ScrollView::create();
    scrollView->setContentSize(Size(panelBg->getContentSize().width - 40, panelBg->getContentSize().height-200)); // 滚动区域大小
    scrollView->setInnerContainerSize(Size(270 * shopItems.size(), panelBg->getContentSize().height -200)); // 内部容器大小，5个商品宽度
    scrollView->setDirection(ui::ScrollView::Direction::HORIZONTAL); // 水平滚动

    scrollView->setPosition(Vec2(20, panelBg->getContentSize().height / 4)); // 位置
    scrollView->setScrollBarEnabled(true); // 显示滚动条
    scrollView->setScrollBarPositionFromCorner(Vec2(2, 2)); // 滚动条位置
    scrollView->setScrollBarWidth(20); // 滚动条宽度
    scrollView->setScrollBarColor(Color3B::WHITE); // 滚动条颜色
    panelBg->addChild(scrollView);


    // 在滚动容器内创建横向排列的商品
    for (int i = 0; i < shopItems.size(); i++) {
        const auto& item = shopItems[i];

        // 商品背景
        auto itemBg = LayerColor::create(Color4B(160, 180, 230, 255), 250, 300);
        itemBg->setPosition(Vec2(20 + i * 270, 10));
        scrollView->addChild(itemBg);

        // 设置触摸事件
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);

        listener->onTouchBegan = [itemBg,item](Touch* touch, Event* event) -> bool {
            Vec2 locationInNode = itemBg->convertToNodeSpace(touch->getLocation());
            Size size = itemBg->getContentSize();
            Rect rect = Rect(0, 0, size.width, size.height);

            if (rect.containsPoint(locationInNode)&& item.isAvailable) {
                itemBg->setColor(Color3B(120, 140, 180)); // 按下变暗
                return true;
            }
            return false;
            };

        listener->onTouchEnded = [itemBg, i](Touch* touch, Event* event) {
            itemBg->setColor(Color3B(160, 180, 230)); // 恢复颜色

            Vec2 locationInNode = itemBg->convertToNodeSpace(touch->getLocation());
            Size size = itemBg->getContentSize();
            Rect rect = Rect(0, 0, size.width, size.height);

            if (rect.containsPoint(locationInNode)) {
                CCLOG("Item %d background clicked!", i + 1);
                // 处理商品点击
            }
            };

        listener->onTouchCancelled = [itemBg](Touch* touch, Event* event) {
            itemBg->setColor(Color3B::WHITE);
            };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, itemBg);
        if (!item.isAvailable) {
            // 获取itemBg的尺寸
            Size bgSize = itemBg->getContentSize();

            // 创建与商品背景同样大小的遮罩
            auto grayMask = LayerColor::create(Color4B(128, 128, 128, 150), bgSize.width, bgSize.height);

            // 设置位置为(0,0) - 相对于itemBg的本地坐标系
            grayMask->setPosition(Vec2::ZERO);

            // 添加到itemBg，并设置高z-order确保在最上面
            itemBg->addChild(grayMask, 9999);  // 使用更大的数字
        }
        // 商品图片（添加在名称上面）
        Sprite* itemImage = nullptr;
        if (!item.imagePath.empty()) {
            itemImage = Sprite::create(item.imagePath);
            if (itemImage) {
                // 调整图片大小以适应商品框
                float maxSize = 250.0f;
                float scale = std::min(maxSize / itemImage->getContentSize().width,
                    maxSize / itemImage->getContentSize().height);
                itemImage->setScale(scale);
                itemImage->setPosition(Vec2(itemBg->getContentSize().width / 2,
                    itemBg->getContentSize().height - 130));
                itemBg->addChild(itemImage);
            }
            else {
                // 如果图片加载失败，显示占位符
                itemImage = Sprite::create("ui/placeholder.png");
                itemImage->setScale(0.5f);
                itemImage->setPosition(Vec2(itemBg->getContentSize().width / 2,
                    itemBg->getContentSize().height - 70));
                itemBg->addChild(itemImage);
            }
        }

        // 商品标签
        auto itemLabel = Label::createWithSystemFont(item.name, "Arial", 30);
        itemLabel->setPosition(Vec2(itemBg->getContentSize().width / 2, itemBg->getContentSize().height - 30));
        itemLabel->setColor(Color3B::BLACK);
        itemBg->addChild(itemLabel);

        // 商品价格图标
        auto goldIcon = Sprite::create("Gold.png");
        goldIcon->setPosition(Vec2(itemBg->getContentSize().width / 3,30));
        goldIcon->setScale(0.5f);
        itemBg->addChild(goldIcon);
        //商品价格
        auto priceLabel = Label::createWithSystemFont("$" + std::to_string(item.price), "Arial", 25);
        priceLabel->setPosition(Vec2(itemBg->getContentSize().width / 2 + 10, 30));
        priceLabel->setColor(Color3B::YELLOW);
        itemBg->addChild(priceLabel);

        // 购买按钮
        auto buyButton = createShopButton(item, i);
        buyButton->setPosition(Vec2(itemBg->getContentSize().width / 2, 80));
        itemBg->addChild(buyButton);

        // 如果商品不可用，显示原因
        if (!item.isAvailable) {
            this->showUnavailableBubble(item, itemBg);
        }
    }


    return true;
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
                    showUnavailablePrompt(index);
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
// 添加显示气泡提示的函数
void ShopPopup::showUnavailableBubble(const ShopItem& item, LayerColor* targetNode) {
    // 获取目标节点的世界坐标
    Vec2 worldPos = targetNode->getParent()->convertToWorldSpace(targetNode->getPosition());

    // 创建提示气泡
    auto bubble = Node::create();
    bubble->setPosition(worldPos + Vec2(0, 120)); // 在商品上方显示
    bubble->setTag(8888);
    this->addChild(bubble, 999);

    // 气泡背景
    auto bubbleBg = LayerColor::create(Color4B(50, 50, 50, 220), 200, 80);
    bubbleBg->setPosition(Vec2::ZERO);
    bubble->addChild(bubbleBg);

    // 三角形箭头
    auto arrow = DrawNode::create();
    Vec2 arrowPoints[] = {
        Vec2(90, 0),
        Vec2(100, -10),
        Vec2(110, 0)
    };
    arrow->drawSolidPoly(arrowPoints, 3, Color4F(0.2f, 0.2f, 0.2f, 0.9f));
    arrow->setPosition(Vec2(0, 80));
    bubble->addChild(arrow);

    // 原因文本
    auto reasonLabel = Label::createWithSystemFont(
        item.unavailableReason,
        "Arial", 16
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
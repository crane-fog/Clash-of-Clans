#include "ShopPopup.h"
#include"UIparts.h"
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
// 购买商品的处理函数（示例）
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
        {1, "Sword", 100, true, ""},
        {2, "Shield", 150, false, "Level too low"},
        {3, "Potion", 50, true, ""},
        {4, "Armor", 200, false, "Insufficient gold"},
        {5, "Bow", 120, true, ""},
        {6, "Staff", 180, false, "Quest required"},
        {7, "Ring", 80, true, ""},
        {8, "Amulet", 250, false, "VIP only"},
        {9, "Amulet", 250, false, "VIP only"},
        {10, "Amulet", 250, false, "VIP only"}

    };
    auto visibleSize = Director::getInstance()->getVisibleSize();

    setupBackground();
    // 面板背景
    auto panelBg = LayerColor::create(Color4B(50, 50, 100, 255),
        visibleSize.width * 0.8f,
        visibleSize.height * 0.6f);
    panelBg->setPosition(Vec2(visibleSize.width * 0.1f, visibleSize.height * 0.2f));
    this->addChild(panelBg);

    // 标题
    auto title = Label::createWithSystemFont("商店", "Arial", 50);
    title->setPosition(Vec2(panelBg->getContentSize().width / 2,
        panelBg->getContentSize().height - 50));
    panelBg->addChild(title);

    // 关闭按钮
    auto closeBtn = Button::create();
    closeBtn->setTitleText("关闭");
    closeBtn->setTitleFontSize(20);
    closeBtn->setContentSize(Size(80, 40));
    closeBtn->setPosition(Vec2(panelBg->getContentSize().width / 10, panelBg->getContentSize().height - 100));
    closeBtn->addTouchEventListener(CC_CALLBACK_2(ShopPopup::onClose, this));
    panelBg->addChild(closeBtn);



    // 创建滚动容器
    auto scrollView = ui::ScrollView::create();
    scrollView->setContentSize(Size(panelBg->getContentSize().width - 40, panelBg->getContentSize().height / 2)); // 滚动区域大小
    scrollView->setInnerContainerSize(Size(220 * shopItems.size(), panelBg->getContentSize().height / 2)); // 内部容器大小，5个商品宽度
    scrollView->setDirection(ui::ScrollView::Direction::HORIZONTAL); // 水平滚动

    scrollView->setPosition(Vec2(20, panelBg->getContentSize().height / 4)); // 位置
    scrollView->setScrollBarEnabled(true); // 显示滚动条
    scrollView->setScrollBarPositionFromCorner(Vec2(2, 2)); // 滚动条位置
    scrollView->setScrollBarWidth(4); // 滚动条宽度
    scrollView->setScrollBarColor(Color3B::WHITE); // 滚动条颜色
    panelBg->addChild(scrollView);




    // 在滚动容器内创建横向排列的商品
    for (int i = 0; i < shopItems.size(); i++) {
        const auto& item = shopItems[i];

        // 商品背景
        auto itemBg = LayerColor::create(Color4B(100, 100, 200, 255), 200, 200);
        itemBg->setPosition(Vec2(20 + i * 220, 20));
        scrollView->addChild(itemBg);

        // 设置触摸事件
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);

        listener->onTouchBegan = [itemBg](Touch* touch, Event* event) -> bool {
            Vec2 locationInNode = itemBg->convertToNodeSpace(touch->getLocation());
            Size size = itemBg->getContentSize();
            Rect rect = Rect(0, 0, size.width, size.height);

            if (rect.containsPoint(locationInNode)) {
                itemBg->setColor(Color3B(80, 80, 180)); // 按下变暗
                return true;
            }
            return false;
            };

        listener->onTouchEnded = [itemBg, i](Touch* touch, Event* event) {
            itemBg->setColor(Color3B(100, 100, 200)); // 恢复颜色

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
        // 商品标签
        auto itemLabel = Label::createWithSystemFont(item.name, "Arial", 30);
        itemLabel->setPosition(Vec2(itemBg->getContentSize().width / 2, itemBg->getContentSize().height - 30));
        itemLabel->setColor(Color3B::WHITE);
        itemBg->addChild(itemLabel);

        // 商品价格
        auto goldIcon = Sprite::create("Gold.png");
        goldIcon->setPosition(Vec2(itemBg->getContentSize().width / 3, 30));
        goldIcon->setScale(0.5f);
        itemBg->addChild(goldIcon);
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
            auto reasonLabel = Label::createWithSystemFont(item.unavailableReason, "Arial", 12);
            reasonLabel->setPosition(Vec2(itemBg->getContentSize().width / 2, 10));
            reasonLabel->setColor(Color3B::RED);
            itemBg->addChild(reasonLabel);
        }
    }


    return true;
}
// 创建商店按钮的函数
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
// 在按钮点击事件中
void ShopPopup::onShopButtonClick(Ref* sender)
{
    auto popup = ShopPopup::create();
    popup->show(this);
}
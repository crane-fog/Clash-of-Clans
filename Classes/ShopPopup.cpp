#include <fstream>
#include <vector>

#include "ShopPopup.h"
#include"UIparts.h"
#include"UIcommon.h"
#include"Arch.h"
#include"ArchInfo.h"
#include"MainVillageScene.h"
USING_NS_CC;
using namespace ui;


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
bool ShopPopup::init()
{
    if (!Layer::init()) {
        return false;
    }

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

    auto buildingTabBg = LayerColor::create(Color4B::WHITE,150,60);
    buildingTabBg->setPosition(Vec2(-40, -20));
    buildingTab->addChild(buildingTabBg,-1);

    buildingTab->setTitleText("建筑");
    buildingTab->setTitleFontSize(28);
    buildingTab->setTitleColor(Color3B::WHITE);
    buildingTab->setContentSize(Size(tabWidth, 60));
    buildingTab->setPosition(Vec2(tabWidth / 2 + 10, tabStartY));
    buildingTab->setColor(Color3B(100, 150, 200)); // 蓝色
    buildingTab->setTag(1); // 标记为建筑标签
    buildingTab->addTouchEventListener([this, buildingTab, buildingTabBg](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->switchToTab(1); // 切换到建筑标签
            buildingTabBg->setColor(Color3B(255, 230, 200));
        }
        });
    panelBg->addChild(buildingTab, 10);

    // 士兵选项卡
    auto soldierTab = ui::Button::create();

    auto soldierTabBg = LayerColor::create(Color4B::WHITE, 150, 60);
    soldierTabBg->setPosition(Vec2(-40, -20));
    soldierTab->addChild(soldierTabBg, -1);

    soldierTab->setTitleText("士兵");
    soldierTab->setTitleFontSize(28);
    soldierTab->setTitleColor(Color3B::WHITE);
    soldierTab->setContentSize(Size(tabWidth, 60));
    soldierTab->setPosition(Vec2(tabWidth * 1.5 + 20, tabStartY));
    soldierTab->setColor(Color3B(150, 100, 200)); // 紫色
    soldierTab->setTag(2); // 标记为士兵标签
    soldierTab->addTouchEventListener([this, soldierTab, soldierTabBg](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->switchToTab(2); // 切换到士兵标签
            soldierTabBg->setColor(Color3B(255, 230, 200));
        }
        });
    panelBg->addChild(soldierTab, 10);

    // 抽卡选项卡
    auto gachaTab = ui::Button::create();

    auto gachaTabBg = LayerColor::create(Color4B::WHITE, 150, 60);
    gachaTabBg->setPosition(Vec2(-40, -20));
    gachaTab->addChild(gachaTabBg, -1);

    gachaTab->setTitleText("抽卡");
    gachaTab->setTitleFontSize(28);
    gachaTab->setTitleColor(Color3B::WHITE);
    gachaTab->setContentSize(Size(tabWidth, 60));
    gachaTab->setPosition(Vec2(tabWidth * 2.5 + 30, tabStartY));
    gachaTab->setColor(Color3B(200, 150, 100)); // 橙色
    gachaTab->setTag(3); // 标记为抽卡标签
    gachaTab->addTouchEventListener([this, gachaTab, gachaTabBg](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->switchToTab(3); // 切换到抽卡标签
            gachaTabBg->setColor(Color3B(255, 230, 200));
        }
        });
    panelBg->addChild(gachaTab, 10);

    // 创建滚动容器（初始显示建筑）
    auto scrollView = ui::ScrollView::create();
    scrollView->setContentSize(Size(panelBg->getContentSize().width - 40,
        panelBg->getContentSize().height)); 

    scrollView->setInnerContainerSize(Size(270 * kShopItemsInfo.at(1).size(),
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
    buildingItems_ = kShopItemsInfo.at(1);
    soldierItems_ = kShopItemsInfo.at(2);
    gachaItems_ = kShopItemsInfo.at(3);
    scrollView_ = scrollView; // 保存滚动容器引用

    // 初始显示建筑商品
    showItemsInScrollView(buildingItems_, scrollView);
    // 初始化抽卡池
    initGachaPool();
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
            showItemsInScrollView(kShopItemsInfo.at(2), scrollView, tabIndex);
            break;
        case 3: // 抽卡
            //scrollView->setInnerContainerSize(Size(270 * gachaItems_.size(),
                //scrollView->getContentSize().height));
            //showItemsInScrollView(gachaItems_, scrollView, tabIndex);
            // 抽卡界面特殊处理
            createGachaItem();
            break;
    }

    // 滚动到最左边
    scrollView->scrollToPercentHorizontal(0, 0.3f, true);
}
// 在滚动容器中显示商品的辅助函数
void ShopPopup::showItemsInScrollView(const std::vector<ShopItem>& items, ui::ScrollView* scrollView, int tabIndex) {

    auto scrollBg = LayerColor::create(Color4B(255, 230, 200, 255), 270 * buildingItems_.size(),
                scrollView->getContentSize().height-240);
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
        // 根据商品ID确定建筑类型
        unsigned char archNo = INVALID_ARCH_NO;
        switch (item.id) {
            case 1: // 兵营
                archNo = ARMY_CAMP;
                break;
            case 2: // 城墙
                archNo = WALL;
                break;
            case 3: // 金库
                archNo = GOLD_STORAGE;
                break;
            case 4: // 圣水罐
                archNo = ELIXIR_STORAGE;
                break;
            case 5: // 金矿
                archNo = GOLD_MINE;
                break;
            case 6: // 圣水收集器
                archNo = ELIXIR_COLLECTOR;
                break;
            case 7: // 箭塔
                archNo = ARCHER_TOWER;
                break;
            case 8: // 加农炮
                archNo = CANNON;
                break;
            case 9: // 训练营
                archNo = BARRACKS;
                break;

                return;
        }
        // 获取并修改金币
        unsigned long long currentGold = GameManager::getInstance()->getGold();
        CCLOG("当前金币: %llu", currentGold);
        // 设置触摸事件
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this, itemBg, item,scrollView,archNo, currentGold](Touch* touch, Event* event) -> bool {

            Vec2 locationInNode = itemBg->convertToNodeSpace(touch->getLocation());
            Size size = itemBg->getContentSize();
            Rect rect = Rect(0, 0, size.width, size.height);
            auto scene = dynamic_cast<MainVillage*>(Director::getInstance()->getRunningScene());  


            if (rect.containsPoint(locationInNode)) {
                if (item.isAvailable&& currentGold >item.price) {//
                    itemBg->setColor(Color3B(120, 140, 180)); // 按下变暗
                    // 按下即购买
                    scene->addBuildingByNO(archNo, item.price);
                    GameManager::getInstance()->setGold(currentGold - item.price);  // 减少金币

                    this->close();
                    // 添加购买反馈效果
                    auto scaleDown = ScaleTo::create(0.1f, 0.95f);
                    itemBg->runAction(scaleDown);
                }
                else {
                    if (currentGold < item.price) {
                        this->showUnavailableBubble(item, itemBg, scrollView,"金币不足");
                    }
                    else this->showUnavailableBubble(item, itemBg, scrollView,"");
                }
                return true;
            }
            return false;
            };

        listener->onTouchEnded = [itemBg, i, item,this](Touch* touch, Event* event) {
            itemBg->setColor(Color3B(160, 180, 230)); // 恢复颜色

            Vec2 locationInNode = itemBg->convertToNodeSpace(touch->getLocation());
            Size size = itemBg->getContentSize();
            Rect rect = Rect(0, 0, size.width, size.height);

            if (rect.containsPoint(locationInNode)) {
                CCLOG("Item %d clicked: %s", i + 1, item.name.c_str());

            }
            };

        listener->onTouchCancelled = [itemBg](Touch* touch, Event* event) {
            itemBg->setColor(Color3B(160, 180, 230));
            };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, itemBg);

        // 如果商品不可用，添加灰色遮罩
        if (!item.isAvailable|| currentGold < item.price) {
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
        if (tabIndex == 3||kArchInfo.at(archNo)[0].upgrade_cost_type_) {
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

    }
}   

// 在ShopPopup::close()函数中，确保清理放置状态
void ShopPopup::close() {

    // 关闭逻辑...
    auto scaleTo = ScaleTo::create(0.2f, 0.1f);
    auto easeIn = EaseBackIn::create(scaleTo);
    auto remove = RemoveSelf::create();
    auto sequence = Sequence::create(easeIn, remove, nullptr);
    this->runAction(sequence);

    // 重新启用地图输入 
    auto parent = this->getParent();
    auto map = parent->getChildByName("BaseMap");
    if (map) ((BaseMap*)map)->setInputEnabled(true);
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
void ShopPopup::onClose(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED) {
        close();
    }
}
void ShopPopup::showUnavailableBubble(const ShopItem& item, cocos2d::LayerColor* targetNode, cocos2d::ui::ScrollView* scrollView,std::string reason) {

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
    arrow->drawSolidPoly(arrowPoints, 3, Color4F(0.7f, 0.5f, 0.5f, 0.9f));
    arrow->setPosition(Vec2(0, -10));
    bubble->addChild(arrow);

    // 原因文本
    auto reasonLabel = Label::createWithSystemFont(
        reason,
        "Arial", 25
    );
    if (reason.empty()) {
        reasonLabel->setString(item.unavailableReason);
    }
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
// 初始化抽卡池
void ShopPopup::initGachaPool() {
    gachaPool_ = kGachaItemsInfo.at(1);
}

// 创建抽卡界面
void ShopPopup::createGachaItem() {
    if (!scrollView_) return;

    scrollView_->removeAllChildren();

    // 抽卡界面背景
    auto gachaBg = LayerColor::create(Color4B(50, 30, 70, 255), 350, 450);
    gachaBg->setPosition(Vec2(
        scrollView_->getContentSize().width / 2 - 175,
        scrollView_->getContentSize().height / 2 - 400
    ));
    scrollView_->addChild(gachaBg);

    // 标题
    auto title = Label::createWithSystemFont("神秘抽卡", "fonts/Marker Felt.ttf", 48);
    title->setColor(Color3B(255, 215, 0)); // 金色
    title->enableShadow(Color4B::BLACK, Size(2, -2), 0);
    title->setPosition(Vec2(175, 400));
    gachaBg->addChild(title);

    // 描述
    auto desc = Label::createWithSystemFont("点击按钮抽取神秘物品", "Arial", 24);
    desc->setColor(Color3B::WHITE);
    desc->setPosition(Vec2(175, 350));
    gachaBg->addChild(desc);

    // 抽卡展示区域
    auto cardArea = LayerColor::create(Color4B(30, 20, 40, 255), 300,200);
    cardArea->setPosition(Vec2(25, 120));
    cardArea->setTag(1001); // 用于后续查找
    gachaBg->addChild(cardArea);

    // 问号图标（初始状态）
    auto questionMark = Sprite::create("question_mark.png");
    if (!questionMark) {
        questionMark = Sprite::create();
        auto draw = DrawNode::create();
        draw->drawCircle(Vec2(0, 0), 40, 0, 30, false, Color4F::WHITE);
        draw->drawLine(Vec2(0, 30), Vec2(0, -30), Color4F::WHITE);
        draw->drawLine(Vec2(-30, 0), Vec2(30, 0), Color4F::WHITE);
        questionMark->addChild(draw);
    }
    questionMark->setScale(0.1f);
    questionMark->setPosition(Vec2(150, 100));
    questionMark->setTag(1002); // 用于后续替换
    cardArea->addChild(questionMark);

    // 抽卡按钮
    auto gachaButton = ui::Button::create();
    gachaButton->setTitleText("开始抽卡");
    gachaButton->setTitleFontSize(32);
    gachaButton->setTitleColor(Color3B::WHITE);
    gachaButton->setContentSize(Size(200, 70));
    gachaButton->setPosition(Vec2(100, 150));
    gachaButton->setColor(Color3B(200, 50, 50));

    // 十连抽按钮
    auto tenGachaButton = ui::Button::create();
    tenGachaButton->setTitleText("十连抽");
    tenGachaButton->setTitleFontSize(28);
    tenGachaButton->setTitleColor(Color3B::WHITE);
    tenGachaButton->setContentSize(Size(200, 70));
    tenGachaButton->setPosition(Vec2(250, 150));
    tenGachaButton->setColor(Color3B(180, 100, 50));

    tenGachaButton->addTouchEventListener(
        [this](Ref*, ui::Widget::TouchEventType type) {
            if (type == ui::Widget::TouchEventType::ENDED) {
                this->startTenGacha();
            }
        }
    );

    gachaBg->addChild(tenGachaButton);



    // 按钮发光效果
    auto buttonGlow = Sprite::create("ui/glow_circle.png");
    if (buttonGlow) {
        buttonGlow->setScale(1.2f);
        buttonGlow->setPosition(Vec2(100, 35));
        buttonGlow->setOpacity(150);
        buttonGlow->runAction(RepeatForever::create(
            Sequence::create(
                FadeTo::create(0.8f, 200),
                FadeTo::create(0.8f, 100),
                nullptr
            )
        ));
        gachaButton->addChild(buttonGlow, -1);
    }

    gachaButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->performSingleGacha(nullptr);;
        }
        });
    gachaBg->addChild(gachaButton);

    // 概率说明
    auto probability = Label::createWithSystemFont(
        "概率: SSR 5% | SR 15% | R 30% | N 50%",
        "Arial", 18
    );
    probability->setColor(Color3B(200, 200, 200));
    probability->setPosition(Vec2(175, 100));
    gachaBg->addChild(probability);
}

// 执行抽卡
void ShopPopup::performSingleGacha(
    const std::function<void(ShopItem)>& onFinished
) {
    int randomValue = rand() % 100;
    Rarity rarity = RARITY_N;

    if (randomValue < 5) rarity = RARITY_SSR;
    else if (randomValue < 20) rarity = RARITY_SR;
    else if (randomValue < 50) rarity = RARITY_R;
    else { // 50% N
        rarity = RARITY_N; }

    showGachaAnimation(rarity);

    this->scheduleOnce([this, rarity, onFinished](float) {
        std::vector<ShopItem*> items;

        for (auto& item : gachaPool_) {
            if (item.rarity == rarity) {
                items.push_back(&item);
            }
        }

        if (items.empty()) return;

        ShopItem result = *items[rand() % items.size()];

        showGachaResult(result);

        if (onFinished) {
            onFinished(result);
        }

        }, 2.5f, "single_gacha_result");
}

// 显示抽卡动画
void ShopPopup::showGachaAnimation(int rarity) {
    // 创建全屏黑色遮罩
    auto mask = LayerColor::create(Color4B(0, 0, 0, 180));
    mask->setContentSize(Director::getInstance()->getVisibleSize());
    mask->setPosition(Vec2::ZERO);
    mask->setTag(8888);
    mask->setOpacity(0);
    this->addChild(mask, 1000);
    mask->runAction(FadeIn::create(0.3f));

    // 创建闪光效果
    auto flash = Sprite::create("flash.png");
    if (rarity == RARITY_SSR) {

    }
    else if (rarity == RARITY_SR) {
        flash->setTexture("SRflash.png"); 
    }
    else  if (rarity == RARITY_R) {
        flash->setTexture("Rflash.png");
    }
    else {
        flash->setTexture("Nflash.png");
    }
    flash->setPosition(Vec2(
        Director::getInstance()->getVisibleSize().width / 2,
        Director::getInstance()->getVisibleSize().height / 2
    ));
    flash->setScale(0.1f);
    flash->setOpacity(0);
    mask->addChild(flash);

    // 闪光动画序列
    flash->runAction(Sequence::create(
        Spawn::create(
            ScaleTo::create(0.5f, 3.0f),
            FadeIn::create(0.3f),
            nullptr
        ),
        DelayTime::create(0.5f),
        Spawn::create(
            ScaleTo::create(0.5f, 0.5f),
            FadeOut::create(0.5f),
            nullptr
        ),
        nullptr
    ));

    // 创建旋转光效
    auto rotatingGlow = Node::create();
    for (int i = 0; i < 8; i++) {
        auto ray = Sprite::create("flash.png");
        if (rarity == RARITY_SSR) {
            ray->setTexture("flash.png");
        }
        else if (rarity == RARITY_SR) {
            ray->setTexture("SRflash.png");
        }
        else  if (rarity == RARITY_R) {
            ray->setTexture("Rflash.png");
        }
        else {
            ray->setTexture("Nflash.png");
        }
        ray->setPosition(Vec2(0, 150));
        ray->setRotation(i * 45);
        ray->setOpacity(0);
        rotatingGlow->addChild(ray);

        // 每个光线的淡入淡出动画
        ray->runAction(Sequence::create(
            DelayTime::create(i * 0.1f),
            FadeIn::create(0.3f),
            DelayTime::create(0.5f),
            FadeOut::create(0.3f),
            nullptr
        ));
    }
    rotatingGlow->setPosition(Vec2(
        Director::getInstance()->getVisibleSize().width / 2,
        Director::getInstance()->getVisibleSize().height / 2
    ));
    rotatingGlow->runAction(Repeat::create(
        RotateBy::create(2.0f, 360),
        1
    ));
    mask->addChild(rotatingGlow);

    // 抽卡中文字
    auto gachaText = Label::createWithSystemFont("抽卡中...", "fonts/Marker Felt.ttf", 60);
    gachaText->setColor(Color3B(255, 255, 100));
    gachaText->enableGlow(Color4B::YELLOW);
    gachaText->setPosition(Vec2(
        Director::getInstance()->getVisibleSize().width / 2,
        Director::getInstance()->getVisibleSize().height / 2 - 200
    ));
    gachaText->setOpacity(0);
    mask->addChild(gachaText);

    // 文字动画
    gachaText->runAction(Sequence::create(
        DelayTime::create(0.5f),
        FadeIn::create(0.3f),
        DelayTime::create(1.5f),
        FadeOut::create(0.3f),
        nullptr
    ));

    // 2.5秒后移除遮罩
    mask->runAction(Sequence::create(
        DelayTime::create(2.5f),
        FadeOut::create(0.3f),
        RemoveSelf::create(),
        nullptr
    ));
}

// 显示抽卡结果
void ShopPopup::showGachaResult(const ShopItem& item) {
    // 移除之前的抽卡结果
    if (gachaResultNode_) {
        gachaResultNode_->removeFromParent();
        gachaResultNode_ = nullptr;
    }

    // 创建结果展示层
    gachaResultNode_ = Node::create();
    gachaResultNode_->setPosition(Vec2::ZERO);
    gachaResultNode_->setTag(9999);
    this->addChild(gachaResultNode_, 1001);

    // 背景遮罩
    auto resultBg = LayerColor::create(Color4B(0, 0, 0, 200));
    resultBg->setContentSize(Director::getInstance()->getVisibleSize());
    resultBg->setPosition(Vec2::ZERO);
    gachaResultNode_->addChild(resultBg);

    // 结果卡片
    auto card = LayerColor::create(Color4B(50, 50, 80, 255), 400, 500);
    card->setPosition(Vec2(
        Director::getInstance()->getVisibleSize().width / 2 - 200,
        Director::getInstance()->getVisibleSize().height / 2 - 250
    ));
    gachaResultNode_->addChild(card);

    // 根据稀有度设置卡片边框颜色和光效
    Color3B borderColor;
    std::string rarityText;
    float glowIntensity = 1.0f;

    // 根据物品rarity判断稀有度
    if (item.rarity ==RARITY_SSR) { // SSR
        borderColor = Color3B(255, 215, 0); // 金色
        rarityText = "SSR";
        glowIntensity = 3.0f;
    }
    else if (item.rarity == RARITY_SR) { // SR
        borderColor = Color3B(255, 100, 255); // 紫色
        rarityText = "SR";
        glowIntensity = 2.0f;
    }
    else if (item.rarity == RARITY_R) { // R
        borderColor = Color3B(100, 200, 255); // 蓝色
        rarityText = "R";
        glowIntensity = 1.5f;
    }
    else { // N
        borderColor = Color3B(150, 150, 150); // 灰色
        rarityText = "N";
        glowIntensity = 1.0f;
    }

    // 卡片边框
    auto border = DrawNode::create();
    Color4F borderColor4F(borderColor);
    border->drawRect(Vec2(0, 0), Vec2(400, 500), borderColor4F);
    card->addChild(border);

    // 发光效果（根据稀有度调整强度）
    if (glowIntensity > 1.0f) {
        auto glow = Sprite::create("ui/glow_circle.png");
        if (glow) {
            glow->setScale(glowIntensity);
            glow->setPosition(Vec2(200, 250));
            glow->setColor(borderColor);
            glow->setOpacity(150);
            glow->runAction(RepeatForever::create(
                Sequence::create(
                    FadeTo::create(0.8f, 200),
                    FadeTo::create(0.8f, 100),
                    nullptr
                )
            ));
            card->addChild(glow, -1);
        }
    }

    // 稀有度文字
    auto rarityLabel = Label::createWithSystemFont(rarityText, "fonts/Marker Felt.ttf", 72);
    rarityLabel->setColor(borderColor);
    rarityLabel->enableGlow(Color4B(borderColor.r, borderColor.g, borderColor.b, 255));
    rarityLabel->setPosition(Vec2(200, 420));
    card->addChild(rarityLabel);

    // 物品图片
    auto itemImage = Sprite::create(item.imagePath);
    if (!itemImage) {
        itemImage = Sprite::create("ui/placeholder.png");
    }
    itemImage->setScale(1.0f);
    itemImage->setPosition(Vec2(200, 250));
    card->addChild(itemImage);

    // 物品名称
    auto nameLabel = Label::createWithSystemFont(item.name, "Arial", 36);
    nameLabel->setColor(Color3B::WHITE);
    nameLabel->setPosition(Vec2(200, 120));
    card->addChild(nameLabel);

    // 物品描述
    auto descLabel = Label::createWithSystemFont(
        "恭喜获得！" + item.unavailableReason,
        "Arial", 24
    );
    descLabel->setColor(Color3B(200, 200, 200));
    descLabel->setPosition(Vec2(200, 80));
    descLabel->setWidth(350);
    descLabel->setAlignment(TextHAlignment::CENTER);
    card->addChild(descLabel);

    // 确定按钮
    auto okButton = ui::Button::create();
    okButton->setTitleText("确定");
    okButton->setTitleFontSize(28);
    okButton->setTitleColor(Color3B::WHITE);
    okButton->setContentSize(Size(150, 60));
    okButton->setPosition(Vec2(200, 30));
    okButton->setColor(Color3B(100, 150, 200));
    okButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            if (gachaResultNode_) {
                gachaResultNode_->removeFromParent();
                gachaResultNode_ = nullptr;
            }
        }
        });
    card->addChild(okButton);

    // 5秒后自动关闭
    gachaResultNode_->runAction(Sequence::create(
        DelayTime::create(5.0f),
        CallFunc::create([this]() {
            if (gachaResultNode_) {
                gachaResultNode_->removeFromParent();
                gachaResultNode_ = nullptr;
            }
            }),
        nullptr
    ));
}

void ShopPopup::startTenGacha() {
    if (isTenGachaRunning_) return; // 防止重复点击

    isTenGachaRunning_ = true;
    currentTenIndex_ = 0;
    tenResults_.clear();

    runNextTenGacha();
}
void ShopPopup::runNextTenGacha() {
    // 十次完成
    if (currentTenIndex_ >= 10) {
        isTenGachaRunning_ = false;
        CCLOG("十连抽完成");
        return;
    }

    // 执行一次普通抽卡
    performSingleGacha([this](ShopItem result) {
        tenResults_.push_back(result);
        currentTenIndex_++;

        // 下一抽（给一点间隔）
        this->scheduleOnce([this](float) {
            runNextTenGacha();
            }, 0.5f, "next_ten_gacha");
        });
}


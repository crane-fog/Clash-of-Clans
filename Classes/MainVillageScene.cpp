#include "MainVillageScene.h"
#include "CoordAdaptor.h"
#include "cocos/ui/CocosGUI.h"
#include "UIparts.h"
#include"ShopPopup.h"
#include "DataHelper.h"
#include "Arch.h"
#include <chrono>
#include <vector>

USING_NS_CC;

bool MainVillage::init()
{
    if (!Village::init()) {
        return false;
    }

    // 从数据文件中读取建筑数据并创建建筑对象
    time_t current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    time_t data_time = 0;
    if (!DataHelper::readArchData(kMainVillageDataFile, data_time, this->arch_status_)) {
        return false;
    }

    std::vector<ArchData> arch_list;
    DataHelper::mapToList(arch_status_, arch_list);

    for (auto& arch : arch_list) {
        Arch::create(arch, base_map_);
    }

    // 创建一个角色 Sprite
    auto barbarian_sprite = Sprite::create("Barbarian.png");
    if (!barbarian_sprite) {
        return false;
    }
    // 将锚点设置为底部中心
    barbarian_sprite->setAnchorPoint(Vec2(0.5, 0));
    barbarian_sprite->setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(0, 0)));
    // 这个 base_map_ 从 Village 基类继承来
    base_map_->sprites_.push_back(barbarian_sprite);
    base_map_->addChild(barbarian_sprite, 2);
    // 获取屏幕尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto shopButton =cocos2d::ui::Button::create("shop.png", "shopSelected.png", "shopDisabled.png");
    //商店标签
    shopButton->setTitleText("SHOP");
    shopButton->setTitleAlignment(TextHAlignment::LEFT, TextVAlignment::TOP); // 居中
    shopButton->setTitleFontSize(50);

    //商店图标
    shopButton->setPosition(Vec2(visibleSize.width - 100, 100));
    shopButton->setScale(0.5f);
    shopButton->setContentSize(Size(300, 300));  // 设置足够的触摸区域
    shopButton->setTouchEnabled(true);
    shopButton->setEnabled(true);

    shopButton->addTouchEventListener([this](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED) { 
            this->onShopButtonClick(sender);
        }
        });
    this->addChild(shopButton,300);
    return true;
}

void MainVillage::onEnter()
{
    base_map_->changeLinedMap();
    Village::onEnter();

    // 让角色动
    auto move_by1 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(44, 0)));
    auto move_by2 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(0, 44)));
    auto move_by3 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(-44, 0)));
    auto move_by4 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(0, -44)));
    auto seq_action = Sequence::create(move_by1, move_by2, move_by3, move_by4, nullptr);
    auto repeatAction = RepeatForever::create(seq_action);
    base_map_->sprites_.back()->runAction(repeatAction);
}
void MainVillage::onShopButtonClick(Ref* sender)
{
    CCLOG("打开商店...");
    // 避免重复打开
    if (this->getChildByTag(100)) {
        CCLOG("商店已经打开");
        return;
    }
    auto popup = ShopPopup::create();
    if (popup) {
        popup->setTag(100);  // 设置 tag 便于查找
        popup->setLocalZOrder(9999);
        popup->show(this);
        CCLOG("商店弹窗打开成功");
    }
    else {
        CCLOG("错误：无法创建商店弹窗");
    }
}

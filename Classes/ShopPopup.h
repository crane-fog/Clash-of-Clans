#pragma once
#ifndef __SHOP_POPUP_H__
#define __SHOP_POPUP_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class ShopPopup : public cocos2d::Layer
{
public:
    CREATE_FUNC(ShopPopup);
    virtual bool init();

    void show(cocos2d::Node* parent);
    void close();
    void onShopButtonClick(cocos2d::Ref* sender);

private:
    void onClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

};
struct ShopItem {
    int id;
    std::string name;
    int price;
    bool isAvailable;
    std::string unavailableReason;
};
#endif // __SHOP_POPUP_H__#pragma once
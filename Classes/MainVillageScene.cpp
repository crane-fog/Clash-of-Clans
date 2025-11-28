#include "MainVillageScene.h"

USING_NS_CC;

bool MainVillage::init()
{
    if (!Village::init()) {
        return false;
    }

    // 创建一个角色 Sprite
    auto barbarian_sprite = Sprite::create("Barbarian.png");
    if (!barbarian_sprite) {
        return false;
    }
    // 将锚点设置为底部中心
    barbarian_sprite->setAnchorPoint(Vec2(0.5, 0));
    barbarian_sprite->setPosition(Vec2(620, 1280));
    // 这个 base_map_ 从 Village 基类继承来
    base_map_->sprites_.push_back(barbarian_sprite);
    base_map_->addChild(barbarian_sprite, 2);

    return true;
}

void MainVillage::onEnter()
{
    Village::onEnter();

    // 让角色动
    auto move_by1 = MoveBy::create(2, Vec2(1155, 865));
    auto move_by2 = MoveBy::create(2, Vec2(1155, -865));
    auto move_by3 = MoveBy::create(2, Vec2(-1155, -865));
    auto move_by4 = MoveBy::create(2, Vec2(-1155, 865));
    auto seq_action = Sequence::create(move_by1, move_by2, move_by3, move_by4, nullptr);
    auto repeatAction = RepeatForever::create(seq_action);
    base_map_->sprites_.back()->runAction(repeatAction);
}
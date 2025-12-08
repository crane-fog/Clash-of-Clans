#include "MainVillageScene.h"
#include "CoordAdaptor.h"
#include "cocos/ui/CocosGUI.h"
#include "UIparts.h"
#include "ShopPopup.h"
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

    // 从数据文件中读取资源数据
    if (!DataHelper::readSourceData(kSourceDataFile, gold_, elixir_)) {
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

void MainVillage::cleanup()
{
    // 保存当前资源数据和建筑数据
    std::vector<ArchData> arch_list;
    for (auto a : base_map_->archs_) {
        arch_list.push_back(ArchData(a));
    }
    DataHelper::listToMap(arch_list, arch_status_);
    DataHelper::writeSourceData(kSourceDataFile, gold_, elixir_);
    DataHelper::writeArchData(kMainVillageDataFile, std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(), arch_status_);
    Village::cleanup();
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

// 通过NO新建建筑并加入存档 + 加入场景
bool MainVillage::addBuildingByNO(unsigned char no,int price)
{
    // 默认新建筑等级为1
    unsigned char level = 1;

    // 校验no是否存在
    if (kArchInfo.find(no) == kArchInfo.end()) {
        CCLOG("建筑编号不存在:%d", no);
        return false;
    }

    // 读取ArchInfo默认资源
    auto& info = kArchInfo.at(no)[level - 1];

    // 构建建筑数据
    ArchData data;
    data.no_ = no;
    data.level_ = level;
    data.x_ = MAP_SIZE/2;                 //默认左下角
    data.y_ = MAP_SIZE/2;
    data.current_hp_ = info.hp_;         //如果没有max_hp_字段就给默认值
    data.current_capacity_ = info.max_capacity_;              //资源建筑容量

    // 创建建筑到地图
    auto arch = Arch::create(data, base_map_);
    if (!arch) return false;
    base_map_->archs_.push_back(arch);

    // 展示取消按钮和确认按钮
    createCancelButton(arch);
    createConfirmButton(arch);

    // 创建建筑预览（建筑会显示在默认位置）
    arch->setOpacity(150);  // 设置为半透明
    arch->setLocalZOrder(900);  // 设置在最上层
   
    // 立即显示放置提示和按钮
    return true;
}

void MainVillage::createCancelButton(Arch* pendingArch_)
{

    // 创建取消按钮

    auto cancelButton = ui::Button::create();
    cancelButton->setTitleText("取消放置");
    cancelButton->setTitleFontSize(24);
    cancelButton->setTitleColor(Color3B::WHITE);
    cancelButton->setContentSize(Size(300, 200));

    // 创建一个纯色背景
    auto buttonBg = LayerColor::create(Color4B(200, 50, 50,200));  // 纯红色背景，透明度255
    buttonBg->setContentSize(cancelButton->getContentSize());  // 设置背景大小与按钮大小一致
    buttonBg->setPosition(Vec2(0, 0));  // 背景位置设置为按钮的位置
    buttonBg->setName("CANCEL_BUTTONBG");

    // 将背景添加到按钮
    cancelButton->addChild(buttonBg, -1);  // -1 确保背景在按钮下面

    cancelButton->setPosition(Vec2(10,10));
    cancelButton->setColor(Color3B::BLACK);
    cancelButton->setName("CANCEL_BUTTON");


    cancelButton->addTouchEventListener([this,pendingArch_](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->cancelBuildingPlacement(pendingArch_);
        }
        });

    pendingArch_->addChild(cancelButton,900);
}

void MainVillage::createConfirmButton(Arch* pendingArch_)
{

    // 创建确认按钮
    auto confirmButton = ui::Button::create();
    confirmButton->setTitleText("确认放置");
    confirmButton->setTitleFontSize(24);
    confirmButton->setTitleColor(Color3B::WHITE);
    confirmButton->setContentSize(Size(300,200));
    // 创建一个纯色背景
    auto buttonBg = LayerColor::create(Color4B(50, 200, 50, 200));  // 纯红色背景，透明度255
    buttonBg->setContentSize(confirmButton->getContentSize());  // 设置背景大小与按钮大小一致
    buttonBg->setPosition(Vec2(0, 0));  // 背景位置设置为按钮的位置
    buttonBg->setName("CONFIRML_BUTTONBG");
    // 将背景添加到按钮
    confirmButton->addChild(buttonBg, -1);  // -1 确保背景在按钮下面
    confirmButton->setPosition(Vec2(200,10 ));
    confirmButton->setColor(Color3B::BLACK);
    confirmButton->setName("CONFIRM_BUTTON");


    confirmButton->addTouchEventListener([this, pendingArch_](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->confirmBuildingPlacement(pendingArch_);
        }
        });

    pendingArch_->addChild(confirmButton, 900);
}

void MainVillage::cancelBuildingPlacement(Arch* pendingArch_)
{
    if (!pendingArch_) return;
    // 从base_map_->archs_中移除
    auto& arr = base_map_->archs_;
    arr.erase(std::remove(arr.begin(), arr.end(), pendingArch_), arr.end());
    // 移除放置相关按钮
    removeCancelAndConfirmButtons(pendingArch_);
    // 删除已创建的建筑
    pendingArch_->removeFromParent();
    pendingArch_ = nullptr;

    // 显示提示并返回商店面板
    CCLOG("建筑放置已取消，返回商店面板");
    showShopPopupWithDelay(1.0f);
}

void MainVillage::confirmBuildingPlacement(Arch* pendingArch_)
{
    if (!pendingArch_) return;
    // 清理UI相关的按钮
    removeCancelAndConfirmButtons(pendingArch_);
    // 清理拖动状态
    pendingArch_->is_dragging_ = false;  
    // 使建筑恢复正常状态
    pendingArch_->setOpacity(255);  // 恢复透明度
    pendingArch_->setLocalZOrder(2);  // 恢复正常层级

    // 播放建筑落地效果
    playBuildingDropEffect(pendingArch_);
    // 将建筑加入存档数据结构
    arch_status_[pendingArch_->Arch::getx(pendingArch_)][pendingArch_->Arch::gety(pendingArch_)] = ArchData(pendingArch_);

    // 写入存档数据
    DataHelper::writeArchData(
        kMainVillageDataFile,
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()),
        arch_status_
    );

    

    // 重新回到商店界面
    CCLOG("建筑放置成功");

}

void MainVillage::removeCancelAndConfirmButtons(Arch* pendingArch_)
{
    pendingArch_->removeChildByName("CANCEL_BUTTON");
    pendingArch_->removeChildByName("CANCEL_BUTTONBG");
    pendingArch_->removeChildByName("CONFIRM_BUTTON");
    pendingArch_->removeChildByName("CONFIRM_BUTTONBG");

}

void MainVillage::playBuildingDropEffect(Arch* arch)
{
    if (!arch) return;

    // 创建发光效果，可以使用淡入淡出的效果
    auto fadeIn = FadeTo::create(0.5f, 255);  // 使建筑恢复透明度


    // 添加光晕效果（模拟发光）
    auto glowEffect = cocos2d::Sprite::create();  // 创建一个光晕精灵
    glowEffect->setTexture("flash.png");  
    glowEffect->setScale(0.1f);  
    glowEffect->setOpacity(50);  // 初始透明度较低
    glowEffect->setPosition(Vec2(arch->getPosition().x, arch->getPosition().y-40));  // 设置光晕的位置与建筑相同

    arch->getParent()->addChild(glowEffect, arch->getLocalZOrder() - 1);  // 将光晕放到建筑的下面

    // 创建光晕的扩散效果
    auto glowScaleUp = ScaleTo::create(0.4f,0.4f);  // 光晕变大
    auto glowFadeIn = FadeTo::create(0.3f, 255);  // 光晕逐渐变亮
    // 添加旋转效果
    auto glowRotate = RotateBy::create(0.6f, 360);  // 光晕旋转360度
    // 执行光晕扩散和发光的动画
    // 在动画结束时移除光晕
    auto removeGlow = CallFunc::create([glowEffect]() {
        glowEffect->removeFromParent();  // 移除光晕精灵
        });
    auto glowSequence = Sequence::create(glowScaleUp, glowFadeIn,glowRotate,removeGlow,fadeIn, nullptr);
    glowEffect->runAction(glowSequence);

}

void MainVillage::showShopPopupWithDelay(float sec)
{
    // 创建商店面板的延迟回调函数
    this->scheduleOnce([this](float dt) {
        // 创建商店面板
        auto shopPopup = ShopPopup::create();  // 创建商店面板
        if (shopPopup) {
            shopPopup->setGlobalZOrder(9999);  // 确保商店面板显示在最上层
            shopPopup->show(this);  // 将商店面板显示到当前场景（this 即为当前场景）
        }
        }, sec, "show_shop_popup_key");  // 延迟 2 秒调用
}


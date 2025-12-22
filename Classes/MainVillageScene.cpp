#include "MainVillageScene.h"
#include "CoordAdaptor.h"
#include "cocos/ui/CocosGUI.h"
#include "UIparts.h"
#include "ShopPopup.h"
#include "DataHelper.h"
#include "Arch.h"
#include "Barbarian.h"
#include "Archer.h"
#include "Giant.h"
#include "CocController.h"
#include <chrono>
#include <vector>
#include "AudioEngine.h"

USING_NS_CC;


bool MainVillage::init()
{
    if (!Village::init()) {
        return false;
    }

    unsigned long long gold = 0, elixir = 0;
    // 从数据文件中读取资源数据
    if (!DataHelper::readSourceData(kSourceDataFile, gold, elixir)) {
        return false;
    }
    GameManager::getInstance()->setGold(gold);
    GameManager::getInstance()->setElixir(elixir);

    // 从数据文件中读取建筑数据并创建建筑对象
    time_t current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    time_t data_time = 0;
    if (!DataHelper::readArchData(kMainVillageDataFile, data_time, this->arch_status_)) {
        return false;
    }

    std::vector<ArchData> arch_list;
    DataHelper::mapToList(arch_status_, arch_list);

    time_t time_diff = current_time - data_time;
    last_exit_time_ = 0;

    for (auto& arch : arch_list) {
        // 更新剩余升级时间
        if (arch.remaining_upgrade_time_ > 0) {
            if (arch.remaining_upgrade_time_ > time_diff) {
                arch.remaining_upgrade_time_ -= static_cast<unsigned int>(time_diff);
            }
            else {
                arch.remaining_upgrade_time_ = 0;
            }
        }
        Arch::create(arch, base_map_);
    }

    /*auto barbarian2 = Barbarian::create(base_map_, 1, Vec2(22, 22));
    if (!barbarian2)return false;
    barbarian2->takeDamage(25);
    base_map_->sprites_.push_back(barbarian2);*/

    /*auto archer = Archer::create(base_map_, 1, Vec2(22, 22));
    if (!archer)return false;
    archer->takeDamage(20);
    base_map_->sprites_.push_back(archer);*/

    auto giant = Giant::create(base_map_, 1, Vec2(22, 22));
    if (!giant)return false;
    giant->setDead();
    giant->takeDamage(200);
    base_map_->sprites_.push_back(giant);

    //// 创建一个角色 Sprite
    //auto barbarian_sprite = Sprite::create("Barbarian.png");
    //if (!barbarian_sprite) {
    //    return false;
    //}
    //// 将锚点设置为底部中心
    //barbarian_sprite->setAnchorPoint(Vec2(0.5, 0));
    //barbarian_sprite->setPosition(CoordAdaptor::cellToPixel(base_map_, Vec2(0, 0)));
    //// 这个 base_map_ 从 Village 基类继承来
    //base_map_->sprites_.push_back(barbarian_sprite);
    //base_map_->addChild(barbarian_sprite, 2);

    // 创建UI层（固定UI层）
    ui_layer_ = UIBars::create();
    if (!ui_layer_) {
        return false;
    }
    // UI层直接添加到场景，不受base_map变换影响
    this->addChild(ui_layer_, 200);  // 较高的z-order，确保UI显示在最上层且固定


    // 获取屏幕尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto shopButton =cocos2d::ui::Button::create("shop.png", "shopSelected.png", "shopDisabled.png");
    //商店标签
    shopButton->setTitleText("商店");
    shopButton->setTitleAlignment(TextHAlignment::LEFT, TextVAlignment::BOTTOM); // 居中
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

    auto attackButton = cocos2d::ui::Button::create("attack.png");
    //进攻图标
    attackButton->setPosition(Vec2(100, 100));
    attackButton->setScale(0.9f);
    attackButton->setContentSize(Size(300, 300));  // 设置足够的触摸区域
    attackButton->setTouchEnabled(true);
    attackButton->setEnabled(true);

    attackButton->addTouchEventListener([this](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
            this->onAttackButtonClick(sender);
        }
        });
    this->addChild(attackButton, 200);

    // 兵种配置图标
    auto troop_config_button = cocos2d::ui::Button::create("TroopConfig.png");
    troop_config_button->setPosition(Vec2(80, 230));
    troop_config_button->setScale(0.9f);
    troop_config_button->setContentSize(Size(300, 300));
    troop_config_button->setTouchEnabled(true);
    troop_config_button->setEnabled(true);

    troop_config_button->addTouchEventListener([this](Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED) {
            this->onTroopButtonClick(sender);
        }
    });
    this->addChild(troop_config_button, 200);

    return true;
}

void MainVillage::onEnter()
{
    AudioEngine::resume(mainhome_bgm);
    auto currentScene = Director::getInstance()->getRunningScene();
    addLoadingLayerToCurrentScene(currentScene, 2.5f);
    if (last_exit_time_ > 0) {
        time_t current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        time_t time_diff = current_time - last_exit_time_;

        if (time_diff > 0) {
            for (auto arch : base_map_->archs_) {
                arch->updateUpgradeTime(time_diff);
            }
        }
        last_exit_time_ = 0;
    }

    Village::onEnter();

    //// 让角色动
    //auto move_by1 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(44, 0)));
    //auto move_by2 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(0, 44)));
    //auto move_by3 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(-44, 0)));
    //auto move_by4 = MoveBy::create(2, CoordAdaptor::cellDeltaToPixelDelta(base_map_, Vec2(0, -44)));
    //auto seq_action = Sequence::create(move_by1, move_by2, move_by3, move_by4, nullptr);
    
    float moveDuration = 5.0f;  // 移动所需时间（秒）
    float delayTime = 2.0f;  // 停顿时间（秒）

    // 构建动作序列
    auto moveToMiddle1 = MoveTo::create(moveDuration, CoordAdaptor::cellToPixel(base_map_,Vec2(22.1,22-0.3)));
    auto delay1 = DelayTime::create(delayTime);

    auto moveToBottom = MoveTo::create(moveDuration, CoordAdaptor::cellToPixel(base_map_, Vec2(0.1, 44-0.3)));
    auto backToMiddle1 = MoveTo::create(moveDuration, CoordAdaptor::cellToPixel(base_map_, Vec2(22.1, 22-0.3)));
    auto delay2 = DelayTime::create(delayTime);

    auto backToTop = MoveTo::create(moveDuration, CoordAdaptor::cellToPixel(base_map_, Vec2(44.1, -0.3)));

    // 将所有动作串成一个 Sequence
    auto seq_action = Sequence::create(
        moveToMiddle1,
        delay1,
        moveToBottom,
        backToMiddle1,
        delay2,
        backToTop,
        nullptr  // 必须以 nullptr 结尾
    );
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
    DataHelper::writeSourceData(kSourceDataFile, GameManager::getInstance()->getGold(), GameManager::getInstance()->getElixir());
    DataHelper::writeArchData(kMainVillageDataFile, std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(), arch_status_);
    Village::cleanup();
}

void MainVillage::onAttackButtonClick(Ref* sender)
{
    // 播放音效
    int button_hit = cocos2d::AudioEngine::play2d("music/button.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule([button_hit, this](float dt) {
        if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
            // 停止音效播放并释放资源
            cocos2d::AudioEngine::uncache("music/button.mp3");
            this->unschedule("stop_audio_key"); // 停止检查
        }
        }, 0.1f, "stop_audio_key");
    // 创建并显示挑战场景选择面板
   UICommonHelper attack_panel;
   bool selected_bg[4] = {0};
   // stop music.
   cocos2d::AudioEngine::pause(mainhome_bgm);
    showChallengeSelectionPanel(this, GameManager::getInstance()->getGold(), GameManager::getInstance()->getElixir());
}

void MainVillage::onTroopButtonClick(Ref* sender)
{    // 播放音效
    int button_hit = cocos2d::AudioEngine::play2d("music/button.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule([button_hit, this](float dt) {
        if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
            // 停止音效播放并释放资源
            cocos2d::AudioEngine::uncache("music/button.mp3");
            this->unschedule("stop_audio_key"); // 停止检查
        }
        }, 0.1f, "stop_audio_key");
    
    auto panel = cocos2d::LayerColor::create(cocos2d::Color4B(130, 130, 190, 255));
    addChild(panel, 99999);

    // 面板标题
    auto titleLabel = cocos2d::Label::createWithSystemFont("还没写好\n这里配置的内容会被保存到单例类TroopConfig", "Arial", 56);
    titleLabel->setPosition(cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width / 2,
        cocos2d::Director::getInstance()->getVisibleSize().height / 2));
    panel->addChild(titleLabel, 1);

    // 退出按钮
    auto exitButton = cocos2d::ui::Button::create("attack_scene/exit.png");
    exitButton->setPosition(cocos2d::Vec2(200, 100));
    exitButton->setScale(0.8f);
    exitButton->addClickEventListener([panel, this](cocos2d::Ref* sender) {
        // 退出面板
        panel->removeFromParent();
        });
    panel->addChild(exitButton);
}

void MainVillage::onShopButtonClick(Ref* sender)
{
    // 播放音效
    int button_hit = cocos2d::AudioEngine::play2d("music/button.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule([button_hit, this](float dt) {
        if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
            // 停止音效播放并释放资源
            cocos2d::AudioEngine::uncache("music/button.mp3");
            this->unschedule("stop_audio_key"); // 停止检查
        }
        }, 0.1f, "stop_audio_key");
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

// 通过商店新建建筑并加入存档 + 加入场景
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
    data.remaining_upgrade_time_ = 0;
    data.current_hp_ = info.hp_;
    data.current_capacity_ = info.max_capacity_; //资源建筑容量

    // 创建建筑到地图
    auto arch = Arch::create(data, base_map_);
    if (!arch) return false;
    base_map_->archs_.push_back(arch);

    // 展示取消按钮和确认按钮
    createCancelButton(arch);
    createConfirmButton(arch,price, kArchInfo.at(no)[level - 1].upgrade_cost_type_);

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

void MainVillage::createConfirmButton(Arch* pendingArch_, int price,bool type_)
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
    unsigned long long currentGold = GameManager::getInstance()->getGold();
    unsigned long long currentExilir = GameManager::getInstance()->getElixir();
    confirmButton->addTouchEventListener([this, pendingArch_, currentGold, currentExilir, price,type_](Ref* sender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED) {
            this->confirmBuildingPlacement(pendingArch_);
            if (type_ == GOLD) {
                pendingArch_->Buiding_Upgrading(sender, pendingArch_, NEW_BUIDING, price, currentGold, type_);
            }
            else {
                pendingArch_->Buiding_Upgrading(sender, pendingArch_, NEW_BUIDING, price, currentExilir, type_);
            }
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
    arch_status_[pendingArch_->getx()][pendingArch_->gety()] = ArchData(pendingArch_);
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
    glowEffect->setOpacity(255);  // 初始透明度较低
    glowEffect->setPosition(Vec2(arch->getPosition().x, arch->getPosition().y-50));  // 设置光晕的位置与建筑相同

    arch->getParent()->addChild(glowEffect, arch->getLocalZOrder() - 1);  // 将光晕放到建筑的下面

    // 创建光晕的扩散效果
    auto glowScaleUp = ScaleTo::create(0.3f,0.4f);  // 光晕变大
    auto glowFadeIn = FadeTo::create(0.3f, 0);  // 光晕逐渐变暗

    // 在动画结束时移除光晕
    auto removeGlow = CallFunc::create([glowEffect]() {
        glowEffect->removeFromParent();  // 移除光晕精灵
        });
    auto glowSequence = Sequence::create(glowScaleUp, glowFadeIn,removeGlow,fadeIn, nullptr);
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

// 显示挑战场景选择面板
 void MainVillage::showChallengeSelectionPanel(cocos2d::Node* parent, int gold_, int elixir_) {
     // 播放音效
     auto select_bgm = AudioEngine::play2d("music/choosing_battle.mp3", true);
    // 创建一个覆盖全屏的面板
    auto panel = cocos2d::LayerColor::create(cocos2d::Color4B(130, 130, 190, 255));  // 黑色背景
    parent->addChild(panel, 99999);
    bool selectedOptions[4] = { false,false,false,false };
    // 面板标题
    auto titleLabel = cocos2d::Label::createWithSystemFont("选择挑战场景", "Arial", 56);
    titleLabel->setPosition(cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width / 2,
        cocos2d::Director::getInstance()->getVisibleSize().height - 50));
    panel->addChild(titleLabel, 1);

    // 创建四个选项
    std::vector<std::string> sceneNames = { "场景1", "场景2", "场景3", "场景4" };
    std::vector<std::string> sceneImages = { "attack_scene/Scenery1.webp", "attack_scene/Scenery2.webp", "attack_scene/Scenery3.webp", "attack_scene/Scenery4.webp" };
    std::vector<std::string> difficultyLevels = { "简单", "中等", "困难", "极难" };


    // 确认按钮
    auto confirmButton = cocos2d::ui::Button::create("attack_scene/yes.png");
    confirmButton->setPosition(cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width - 200, 100));
    confirmButton->setScale(0.8f);
    confirmButton->setEnabled(false);  // 默认不可点击
    confirmButton->setName("confirm_attack");
    panel->addChild(confirmButton);

    confirmButton->addClickEventListener([ &selectedOptions, gold_, elixir_, panel, this, select_bgm](cocos2d::Ref* sender) {
        // 确认后更换场景
        if (selectedOptions[0] != -1) { // 确保已经选择了一个选项
            // 播放音效
            int button_hit = cocos2d::AudioEngine::play2d("music/button.mp3", false, 0.7f);
            // 检查音频的状态，直到播放完成
            this->schedule([button_hit, this](float dt) {
                if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                    // 停止音效播放并释放资源
                    cocos2d::AudioEngine::uncache("music/button.mp3");
                    this->unschedule("stop_audio_key"); // 停止检查
                }
                }, 0.1f, "stop_audio_key");
            CocController::getInstance()->changeScene(1, gold_, elixir_);
            // 点击确认按钮后关闭面板
            panel->removeFromParent();
            this->selectedItemBg = nullptr;
            AudioEngine::stop(select_bgm);
        }
        });

    // 退出按钮
    auto exitButton = cocos2d::ui::Button::create("attack_scene/exit.png");
    exitButton->setPosition(cocos2d::Vec2(200, 100));
    exitButton->setScale(0.8f);
    exitButton->addClickEventListener([panel,this,select_bgm](cocos2d::Ref* sender) {
        // 播放音效
        int button_hit = cocos2d::AudioEngine::play2d("music/button.mp3", false, 0.7f);
        // 检查音频的状态，直到播放完成
        this->schedule([button_hit, this](float dt) {
            if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
                // 停止音效播放并释放资源
                cocos2d::AudioEngine::uncache("music/button.mp3");
                this->unschedule("stop_audio_key"); // 停止检查
            }
            }, 0.1f, "stop_audio_key");
        // 退出面板
        panel->removeFromParent();
        selectedItemBg = nullptr;
        // stop music.
        AudioEngine::stop(select_bgm);
        });
    panel->addChild(exitButton);

    float buttonWidth = 350;
    float buttonHeight = 400;
    float padding = 130;
    int canConfirm[1] = { -1 };
    for (size_t i = 0; i < sceneNames.size(); i++) {
        // 选项背景
        auto itemBg = cocos2d::LayerColor::create(cocos2d::Color4B(255, 255, 255, 255), buttonWidth, buttonHeight);
        itemBg->setPosition(cocos2d::Vec2((buttonWidth + padding) * i + 50, 350));
        itemBg->setTag(i);
        // 选项图片
        auto itemPic = cocos2d::Sprite::create(sceneImages[i]);
        float scale = std::min(buttonWidth / itemPic->getContentSize().width, buttonHeight / itemPic->getContentSize().height);
        itemPic->setPosition(cocos2d::Vec2(buttonWidth / 2, buttonHeight / 2 + 20));
        itemPic->setScale(scale);

        // 显示场景名称
        auto nameLabel = cocos2d::Label::createWithSystemFont(sceneNames[i], "Arial", 34);
        nameLabel->setPosition(cocos2d::Vec2(buttonWidth / 2, 25));  // 名字位置
        nameLabel->setColor(cocos2d::Color3B::BLACK);
        itemBg->addChild(nameLabel, 150);

        // 显示难度级别
        auto difficultyLabel = cocos2d::Label::createWithSystemFont(difficultyLevels[i], "Arial", 25);
        difficultyLabel->setColor(cocos2d::Color3B::BLACK);
        difficultyLabel->setPosition(cocos2d::Vec2(buttonWidth / 2, buttonHeight + 20));  // 难度位置
        itemBg->addChild(difficultyLabel, 150);

        // 将按钮添加到背景层
        itemBg->addChild(itemPic);
        panel->addChild(itemBg);

        // 添加触摸事件监听器
        auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = [parent, itemBg, i, &selectedOptions, panel, confirmButton, &canConfirm,this](cocos2d::Touch* touch, cocos2d::Event* event) {
            // 获取触摸点并判断是否点击了按钮
            cocos2d::Rect buttonRect = itemBg->getBoundingBox();
            if (buttonRect.containsPoint(touch->getLocation())) {
                onOptionClick(itemBg, confirmButton);
                return true;  // 阻止事件继续传播
            }
            return false;
            };
        parent->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, itemBg);  // 为按钮添加触摸事件
    }

}

// 选项点击事件处理
void MainVillage::onOptionClick(cocos2d::LayerColor* itemBg, cocos2d::ui::Button* confirmButton) {
    // 播放音效
    int button_hit = cocos2d::AudioEngine::play2d("music/button.mp3", false, 0.7f);
    // 检查音频的状态，直到播放完成
    this->schedule([button_hit, this](float dt) {
        if (cocos2d::AudioEngine::getState(button_hit) == cocos2d::AudioEngine::AudioState::PAUSED) {
            // 停止音效播放并释放资源
            cocos2d::AudioEngine::uncache("music/button.mp3");
            this->unschedule("stop_audio_key"); // 停止检查
        }
        }, 0.1f, "stop_audio_key");
    // 如果点击的是同一个按钮，保持选中状态
    if (selectedItemBg == itemBg) {
        return;  // 已经是选中的按钮，不做任何改变
    }
    // 如果已有按钮被选中，取消选中状态并恢复原始颜色
    if (selectedItemBg) {
        selectedItemBg->setColor(cocos2d::Color3B::WHITE);  // 恢复原始颜色
        remove_border(selectedItemBg);
    }
    // 更新当前选中的按钮
    selectedItemBg = itemBg;
    if (selectedItemBg) {
        // 更改选中按钮的颜色
        selectedItemBg->setColor(cocos2d::Color3B::BLUE);  // 变暗的颜色
        draw_border(selectedItemBg);


        confirmButton->setEnabled(true);
    }

}

void MainVillage::onExit()
{
    last_exit_time_ = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    Village::onExit();
}

unsigned char MainVillage::getTownHallLevel()
{
    for (auto arch : base_map_->archs_) {
        if (arch->getNo() == TOWN_HALL) {
            return arch->getLevel();
        }
    }
    assert(false && "主村庄中无大本营");
    return 1; // 这里不应该被触发
}

int MainVillage::getBuildingCount(unsigned char archNo)
{
    int count = 0;
    for (auto arch : base_map_->archs_) {
        if (arch->getNo() == archNo) {
            count++;
        }
    }
    return count;
}


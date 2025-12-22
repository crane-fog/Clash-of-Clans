
#pragma once
#ifndef __UI_PARTS_H__
#define __UI_PARTS_H__

#include "cocos2d.h"
#include "BaseMap.h"
#include "cocos/ui/CocosGUI.h"
#include"Arch.h"
#include<string.h>
#include "CocController.h"
#include"UIcommon.h"
#include"TroopTargetManager.h"
// 进度条结构体
struct ProgressBarData {
    cocos2d::ui::LoadingBar* loadingBar;
    cocos2d::LayerColor* background;
    cocos2d::Label* percentLabel;
    cocos2d::Sprite* icon;
    std::string title;
};

// UI部分基类
class UIBars : public cocos2d::Node {
private:


    std::vector<ProgressBarData> progressBars_;  // 存储多个进度条
    cocos2d::EventListenerCustom* goldUpdateListener;  // 存储监听器
    cocos2d::EventListenerCustom* elixirUpdateListener;  // 存储监听器
    cocos2d::EventListenerCustom* maxGoldUpdateListener;  // 存储监听器
    cocos2d::EventListenerCustom* maxElixirUpdateListener;  // 存储监听器

public:



    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;

    // 创建带背景的进度条
    //title:进度条左边文字标签， barcolor:进度条颜色， iconPath：图标文件路径，nowAmount：当前数量，x,y位置，UpperLimit:上限
    void createProgressBarWithBackground(const std::string& title, const cocos2d::Color3B& barColor, const std::string& iconPath, unsigned long long nowAmount, float x, float y, unsigned long long UpperLimit);
    // 更新指定进度条
    void updateProgressBar(const std::string& title, unsigned long long nowAmount, unsigned long long maxAmount);

    // 更新金币进度条的回调函数
    void onGoldUpdated(cocos2d::EventCustom* event);
    // 更新圣水进度条的回调函数
    void onElixirUpdated(cocos2d::EventCustom* event);
    // 更新最大金币的回调函数
    void onMaxGoldUpdated(cocos2d::EventCustom* event);
    // 更新最大圣水的回调函数
    void onMaxElixirUpdated(cocos2d::EventCustom* event);
    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(UIBars);
};



//倒计时
class CountdownTimer : public cocos2d::Node {
public:
    CREATE_FUNC(CountdownTimer);

    void start(unsigned int seconds,
        std::function<void(int remaining)> onTick = nullptr,
        std::function<void()> onComplete = nullptr);

private:
    unsigned int remainingTime_;
    unsigned int totalTime_;
    bool isRunning_;
    std::function<void(int)> onTick_;
    std::function<void()> onComplete_;

    void updateTimer(float dt);
};

enum Buidlingtype : bool{
    NEW_BUIDING = 0,
    UPGRADING = 1
};

class UICommonHelper {
private:
    int canConfirm_ = -1;
public:
    cocos2d::LayerColor* selectedItemBg = nullptr;  // 当前选中的按钮背景
    int getcanC() {
        return canConfirm_;
    }
    void setcanC(int i) {
        this->canConfirm_ = i;
    }

    // 显示挑战场景选择面板
    static void showChallengeSelectionPanel(cocos2d::Node* parent,  int gold_, int elixir_    ) {
        // 创建一个覆盖全屏的面板
        auto panel = cocos2d::LayerColor::create(cocos2d::Color4B(205, 160, 100, 255));  // 黑色背景
        parent->addChild(panel, 99999);
        bool selectedOptions[4] = {false,false,false,false};
        // 面板标题
        auto titleLabel = cocos2d::Label::createWithSystemFont("选择挑战场景", "Arial", 56);
        titleLabel->setPosition(cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width / 2,
            cocos2d::Director::getInstance()->getVisibleSize().height - 50));
        panel->addChild(titleLabel,1);

        // 创建四个选项
        std::vector<std::string> sceneNames = { "场景1", "场景2", "场景3", "场景4" };
        std::vector<std::string> sceneImages = { "attack_scene/Scenery1.webp", "attack_scene/Scenery2.webp", "attack_scene/Scenery3.webp", "attack_scene/Scenery4.webp" };
        std::vector<std::string> difficultyLevels = { "简单", "中等", "困难", "极难" };


        // 确认按钮
        auto confirmButton = cocos2d::ui::Button::create("attack_scene/yes.png");
        confirmButton->setPosition(cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width - 200, 100));
        confirmButton->setTitleText("确定");
        confirmButton->setTitleColor(cocos2d::Color3B::BLACK);
        confirmButton->setScale(0.8f);
        confirmButton->setEnabled(false);  // 默认不可点击
        confirmButton->setName("confirm_attack");
        panel->addChild(confirmButton);

        confirmButton->addClickEventListener([parent, &selectedOptions, gold_, elixir_,panel](cocos2d::Ref* sender) {
            // 确认后更换场景
            if (selectedOptions[0] != -1) { // 确保已经选择了一个选项
                CocController::getInstance()->changeScene(1, gold_, elixir_);
                // 点击确认按钮后关闭面板
                panel->removeFromParent();
            }
            });

        // 退出按钮
        auto exitButton = cocos2d::ui::Button::create("attack_scene/exit.png");
        exitButton->setPosition(cocos2d::Vec2(200, 100));
        exitButton->setTitleText("退出");
        exitButton->setScale(0.8f);
        exitButton->addClickEventListener([panel](cocos2d::Ref* sender) {
            // 退出面板
            panel->removeFromParent();
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
            touchListener->onTouchBegan = [parent, itemBg, i, &selectedOptions,panel, confirmButton,&canConfirm](cocos2d::Touch* touch, cocos2d::Event* event) {
                // 获取触摸点并判断是否点击了按钮
                cocos2d::Rect buttonRect = itemBg->getBoundingBox();
                if (buttonRect.containsPoint(touch->getLocation())) {
                    UICommonHelper::onOptionClick(itemBg, i, selectedOptions, confirmButton,panel); 
                    return true;  // 阻止事件继续传播
                }
                return false;
                };
            parent->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, itemBg);  // 为按钮添加触摸事件
        }

    }
    // 选项点击事件处理
    static void onOptionClick(cocos2d::LayerColor* itemBg, int index, bool selectedOptions[], cocos2d::ui::Button* confirmButton, cocos2d::LayerColor* panel) {
        itemBg->setColor(cocos2d::Color3B::BLUE);


        // 如果有选中项，启用确认按钮并变为橙色

            confirmButton->setEnabled(true);


    }
};



#endif // __UI_PARTS_H__
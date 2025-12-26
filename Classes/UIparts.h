#ifndef __UI_PARTS_H__
#define __UI_PARTS_H__

#include <string.h>

#include "Arch.h"
#include "BaseMap.h"
#include "CocManager.h"
#include "cocos/ui/CocosGUI.h"
#include "cocos2d.h"
#include "TroopTargetManager.h"
#include "UIcommon.h"

// 进度条结构体
struct ProgressBarData {
    // 进度条
    cocos2d::ui::LoadingBar* loading_bar_;
    // 进度条背景
    cocos2d::LayerColor* background_;
    // 百分比标签
    cocos2d::Label* percent_label_;
    // 资源图标
    cocos2d::Sprite* icon_;
    // 资源名称
    std::string title_;
};

// 进度条管理
class UIBars : public cocos2d::Node {
private:
    std::vector<ProgressBarData> progress_bars_;                // 存储多个进度条
    cocos2d::EventListenerCustom* gold_update_listener_;        // 存储金币更新监听器
    cocos2d::EventListenerCustom* elixir_update_listener_;      // 存储圣水更新监听器
    cocos2d::EventListenerCustom* max_gold_update_listener_;    // 存储最大金币更新监听器
    cocos2d::EventListenerCustom* max_elixir_update_listener_;  // 存储最大圣水更新监听器
    cocos2d::EventListenerCustom* jewel_update_listener_;       // 存储宝石更新监听器
    cocos2d::EventListenerCustom* max_jewel_update_listener_;   // 存储最大宝石数量监听器

public:
    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;

    // 创建带背景的进度条
    // title:进度条左边文字标签， barcolor:进度条颜色，
    // iconPath：图标文件路径，nowAmount：当前数量，x,y位置，UpperLimit:上限
    void createProgressBarWithBackground(const std::string& title, const cocos2d::Color3B& barColor,
                                         const std::string& iconPath, unsigned long long nowAmount, float x, float y,
                                         unsigned long long UpperLimit);

    // 创建仅显示数字的资源标签（用于宝石）
    void createResourceLabel(const std::string& title, const std::string& iconPath, unsigned long long nowAmount,
                             float x, float y);

    // 更新指定名称的进度条
    void updateProgressBar(const std::string& title, unsigned long long nowAmount, unsigned long long maxAmount);

    // 更新金币进度条的回调函数
    void onGoldUpdated(cocos2d::EventCustom* event);
    // 更新圣水进度条的回调函数
    void onElixirUpdated(cocos2d::EventCustom* event);
    // 更新宝石进度条的回调函数
    void onJewelUpdated(cocos2d::EventCustom* event);

    // 更新最大金币的回调函数
    void onMaxGoldUpdated(cocos2d::EventCustom* event);
    // 更新最大圣水的回调函数
    void onMaxElixirUpdated(cocos2d::EventCustom* event);

    CREATE_FUNC(UIBars);
};

// 倒计时
class CountdownTimer : public cocos2d::Node {
public:
    CREATE_FUNC(CountdownTimer);

    void start(unsigned int seconds, std::function<void(int remaining)> onTick = nullptr,
               std::function<void()> onComplete = nullptr);

private:
    // 剩余施工时间
    unsigned int remaining_time_;
    // 总施工时间
    unsigned int total_time_;
    // 是否正在施工
    bool is_running_;
    std::function<void(int)> on_tick_;
    // 是否完工
    std::function<void()> on_complete_;
    // 更新时间
    void updateTimer(float dt);
};
// 施工中的建筑类型
enum Buidlingtype : bool { NEW_BUIDING = 0, UPGRADING = 1 };

class UICommonHelper : public cocos2d::Node {
private:
    int can_confirm_ = -1;                             // 是否有选中
    cocos2d::LayerColor* selected_item_bg_ = nullptr;  // 当前选中的战斗场景

public:
    CREATE_FUNC(UICommonHelper);

    virtual bool init() { return Node::init(); }

    // 显示挑战场景选择面板
    void showChallengeSelectionPanel(cocos2d::Node* parent);

    // 显示回放面板
    void showReplayPanel(cocos2d::Node* parent);

    // 选项点击事件处理
    void onOptionClick(cocos2d::LayerColor* item_bg, int index, cocos2d::ui::Button* confirm_button);

private:
    // 创建单个选项
    void createOptionItem(cocos2d::Node* panel, int index, const std::string& name, const std::string& image_path,
                          cocos2d::ui::Button* confirm_button, int progress = -1);
};

#endif  // __UI_PARTS_H__
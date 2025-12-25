#include "AttackStars.h"

#include "cocos/ui/CocosGUI.h"
#include "DataHelper.h"
#include "MainVillageScene.h"
#include "EnemyVillageScene.h"
#include "UIparts.h"
USING_NS_CC;
using namespace ui;

bool AttackStars::init()
{
    if (!Node::init()) {
        return false;
    }
    // 注册大本营摧毁事件监听
    town_hall_death_listener_ =
        cocos2d::EventListenerCustom::create("town_hall_destroyed", CC_CALLBACK_1(AttackStars::onTownHallDeath, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
        town_hall_death_listener_, this);
    // 初始化成员变量
    progress_ = 0;
    last_dead_arch_ = 0;
    is_complete_[0] = false;
    is_complete_[1] = false;
    is_complete_[2] = false;
    is_showing_victory_ = false;

    // 获取建筑总数
    arch_sum_ = TroopTargetManager::getInstance()->getlivingsum();
    if (arch_sum_ <= 0) arch_sum_ = 1;  // 防止除零错误

    int x = 200;
    int y = 20;
    auto visible_size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建进度标签
    auto prog_label = Label::createWithSystemFont(StringUtils::format("摧毁进度：%d%%", progress_), "Arial", 30);
    prog_label->setPosition(Vec2(230, visible_size.height - 30));
    prog_label->setName("destoyed_label");
    this->addChild(prog_label);

    // 创建背景框
    auto background = LayerColor::create(Color4B(0, 0, 0, 150), 400, 40);
    background->setPosition(Vec2(30, visible_size.height - 100));
    background->setName("progress_bg");
    this->addChild(background, 0);

    // 创建进度条
    progress_bar_ = cocos2d::ui::LoadingBar::create("LoadingBarFile.png");
    progress_bar_->setScaleX(2.3f);
    progress_bar_->setScaleY(3.0f);
    progress_bar_->setPosition(cocos2d::Vec2(30.0f + x, visible_size.height - 100.0f + y));
    progress_bar_->setPercent(0);
    progress_bar_->setName("progress_bar");
    this->addChild(progress_bar_);

    // 创建星星精灵
    for (int i = 0; i < 3; ++i) {
        auto star = createStar();
        star->setScale(0.1f);
        star->setPosition(cocos2d::Vec2(130.0f + i * 100.0f, visible_size.height - 150.0f));
        star->setName(StringUtils::format("star_%d", i));
        stars_.push_back(star);
        this->addChild(star);
        setStarColor(star, false);
    }

    // 启动定时检查
    this->schedule(CC_CALLBACK_1(AttackStars::checkForUpdates, this), 2.0f, "update_checker");

    return true;
}

// 检查更新的函数
void AttackStars::checkForUpdates(float dt)
{
    if (is_showing_victory_) return;  // 正在显示胜利画面，不再更新

    CCLOG("=== 进度检查开始 ===");

    // 1. 检查建筑死亡数量
    int current_dead_arch = TroopTargetManager::getInstance()->getDeadsum();
    CCLOG("当前死亡建筑数量：%d", current_dead_arch);

    // 2. 如果有更新，触发UI更新
    if (current_dead_arch != last_dead_arch_) {
        CCLOG("检测到更新！上次：%d，本次：%d", last_dead_arch_, current_dead_arch);

        // 计算进度百分比 (0-100)
        int new_progress = static_cast<int>((static_cast<float>(current_dead_arch) * 100 / arch_sum_));
        auto prog_label = dynamic_cast<cocos2d::Label*>(this->getChildByName("destoyed_label"));

        // 更新进度标签
        if (prog_label) {
            prog_label->setString(StringUtils::format("摧毁进度：%d%%", new_progress));
        }

        progress_ = static_cast<float>(new_progress);
        last_dead_arch_ = current_dead_arch;

        // 更新进度条
        if (progress_bar_) {
            progress_bar_->setPercent(static_cast<float>(new_progress));
        }

        // 星星点亮逻辑
        if (new_progress >= 50 && !is_complete_[0]) {
            showPopup(stars_[0], 50);
            setStarColor(stars_[0], true);
            is_complete_[0] = true;
        }
        if (is_town_star_ && !is_complete_[1]) {
            showPopup(stars_[1], new_progress);
            setStarColor(stars_[1], true);
            is_complete_[1] = true;
        }
        // 100%胜利画面
        if (new_progress >= 100 && !is_complete_[2]) {
            showPopup(stars_[2], 100);
            setStarColor(stars_[2], true);
            is_complete_[2] = true;

            // 显示胜利画面
            showVictoryScreen();
        }
    }

    CCLOG("=== 进度检查结束 ===");
}

// 显示胜利画面
void AttackStars::showVictoryScreen()
{
    if (is_showing_victory_) return;  // 防止重复调用

    is_showing_victory_ = true;
    CCLOG("显示胜利画面");

    auto visible_size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center = Vec2(visible_size.width / 2, visible_size.height / 2);

    // 设置进度
    int level = CocController::getInstance()->getCurrentScene();
    CocController::getInstance()->level_info_list_.at(level - 1).progress_ = 100;

    // 1. 创建全屏黑色半透明遮盖
    auto full_screen_mask = LayerColor::create(Color4B(0, 0, 0, 200));
    full_screen_mask->setContentSize(visible_size);
    full_screen_mask->setPosition(origin);
    full_screen_mask->setName("victory_mask");
    this->addChild(full_screen_mask, -1);  // 较高层级

    // 2. 获取所有需要移动到中间的UI元素
    auto progress_label = dynamic_cast<cocos2d::Label*>(this->getChildByName("destoyed_label"));
    auto progress_bg = dynamic_cast<cocos2d::Node*>(this->getChildByName("progress_bg"));
    auto progress_bar = dynamic_cast<cocos2d::Node*>(this->getChildByName("progress_bar"));

    // 3. 停止定时器检查
    this->unschedule("update_checker");

    // 4. 创建胜利提示文字
    auto victory_label = Label::createWithSystemFont("所有建筑已被摧毁！", "Arial", 100);
    victory_label->setColor(Color3B::YELLOW);
    victory_label->setPosition(Vec2(center.x + 100, center.y + 200));
    victory_label->setName("victory_label");
    full_screen_mask->addChild(victory_label);

    // 5. 将所有UI元素移动到屏幕中央（动画）
    float move_duration = 0.8f;
    float final_y = center.y - 50;  // 最终Y坐标

    // 移动进度标签
    if (progress_label) {
        auto label_move = MoveTo::create(move_duration, Vec2(center.x, center.y + 120));
        auto label_scale = ScaleTo::create(move_duration, 1.5f);
        progress_label->runAction(Spawn::create(label_move, label_scale, nullptr));
    }

    // 移动进度条
    if (progress_bar) {
        auto bar_move = MoveTo::create(move_duration, Vec2(center.x, final_y));
        auto bar_scale = ScaleTo::create(move_duration, 2.5f, 3.0f);  // 稍微放大
        progress_bar->runAction(Spawn::create(bar_move, bar_scale, nullptr));
    }

    // 移动星星
    for (unsigned int i = 0; i < stars_.size(); ++i) {
        if (stars_[i]) {
            float star_x = center.x - 100 + i * 100;
            auto star_move = MoveTo::create(move_duration, Vec2(star_x, final_y - 80));
            auto star_scale = ScaleTo::create(move_duration, 0.15f);  // 稍微放大
            auto rotate = RotateBy::create(move_duration, 360);

            // 如果星星还没点亮，先点亮它
            if (!is_complete_[i]) {
                setStarColor(stars_[i], true);
                is_complete_[i] = true;
            }

            stars_[i]->runAction(Spawn::create(star_move, star_scale, rotate, nullptr));
        }
    }

    // 6. 添加退出按钮
    auto exit_label = Label::createWithSystemFont("退出", "Arial", 60);
    auto exit_item = MenuItemLabel::create(exit_label, [this](Ref* sender) {
        auto scene = Director::getInstance()->getRunningScene();
        auto enemyVillage = dynamic_cast<EnemyVillage*>(scene);
        if (enemyVillage) {
            enemyVillage->onExitButtonClick(sender);
        }
    });
    exit_item->setPosition(Vec2(center.x, center.y - 300));

    auto menu = Menu::create(exit_item, nullptr);
    menu->setPosition(Vec2::ZERO);
    full_screen_mask->addChild(menu);

    // 7. 吞噬触摸事件，防止点击到后面的游戏内容
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event) {
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, full_screen_mask);
}

// 重置UI到原始位置
void AttackStars::resetUIPosition()
{
    auto visible_size = Director::getInstance()->getVisibleSize();

    // 重置进度标签
    auto progress_label = dynamic_cast<cocos2d::Label*>(this->getChildByName("destoyed_label"));
    if (progress_label) {
        progress_label->setPosition(Vec2(230, visible_size.height - 30));
        progress_label->setScale(1.0f);
    }

    // 重置背景框
    auto progress_bg = dynamic_cast<cocos2d::Node*>(this->getChildByName("progress_bg"));
    if (progress_bg) {
        progress_bg->setPosition(Vec2(30, visible_size.height - 100));
    }

    // 重置进度条
    auto progress_bar = dynamic_cast<cocos2d::Node*>(this->getChildByName("progress_bar"));
    if (progress_bar) {
        progress_bar->setPosition(Vec2(230, visible_size.height - 80));
        progress_bar->setScale(2.3f, 3.0f);
    }

    // 重置星星位置
    for (unsigned int i = 0; i < stars_.size(); ++i) {
        if (stars_[i]) {
            stars_[i]->setPosition(Vec2(130.0f + i * 100.0f, visible_size.height - 150.0f));
            stars_[i]->setScale(0.1f);
        }
    }
}

void AttackStars::showPopup(cocos2d::Sprite* targetStar, int progress)
{
    auto visible_size = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visible_size.width / 2, visible_size.height / 2);

    // 创建飞行星星精灵
    auto flying_star = createStar();
    flying_star->setPosition(center);
    flying_star->setScale(1.0f);
    flying_star->setName("flying_star");
    this->addChild(flying_star, 10);  // 较高层级

    // 创建提示文本
    std::string text;
    if (progress == 50) {
        text = "摧毁进度超过50%！";
    }
    if (is_town_star_)
        text = "敌方大本营已被摧毁！";
    else if (progress == 100) {
        text = "所有建筑已被摧毁！";
    }

    auto progress_text = cocos2d::Label::createWithSystemFont(text, "Arial", 40);
    progress_text->setColor(Color3B::YELLOW);
    progress_text->setPosition(Vec2(center.x, center.y - 200));
    progress_text->setName("popup_text");
    this->addChild(progress_text, 10);

    // 飞行动画：飞到目标星星位置
    auto move_to = MoveTo::create(2.0f, targetStar->getPosition());
    auto rotate_by = RotateBy::create(2.0f, 360);
    auto scale_to = ScaleTo::create(2.0f, 0.1f);
    auto fade_out = FadeOut::create(2.0f);

    auto spawn = Spawn::create(move_to, rotate_by, scale_to, fade_out, nullptr);

    // 文字淡出动画
    auto text_fade_out = FadeOut::create(1.5f);

    // 清理函数
    auto cleanup = CallFunc::create([flying_star, progress_text]() {
        flying_star->removeFromParent();
        progress_text->removeFromParent();
    });

    // 执行动画
    flying_star->runAction(Sequence::create(spawn, cleanup, nullptr));
    progress_text->runAction(Sequence::create(DelayTime::create(0.5f), text_fade_out, nullptr));
}
void AttackStars::onTownHallDeath(cocos2d::EventCustom* event)
{
    is_town_star_ = 1;
    checkForUpdates();
}

// 析构函数
AttackStars::~AttackStars()
{
    if (dead_arch_update_listener_) {
        cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(dead_arch_update_listener_);
    }
}

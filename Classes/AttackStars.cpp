#include "UIparts.h"
#include "cocos/ui/CocosGUI.h"
#include"MainVillageScene.h"
#include"DataHelper.h"
#include"AttackStars.h"
USING_NS_CC;
using namespace ui;

bool AttackStars::init() {
    if (!Node::init()) {
        return false;
    }
    // 注册大本营摧毁事件监听
    TownHallDeathListener = cocos2d::EventListenerCustom::create("town_hall_destroyed", CC_CALLBACK_1(AttackStars::onTownHallDeath, this));
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(TownHallDeathListener, this);
    // 初始化成员变量
    progress_ = 0;
    lastDeadArch = 0;
    isComplete[0] = false;
    isComplete[1] = false;
    isComplete[2] = false;
    isShowingVictory = false;

    // 获取建筑总数
    ArchSum = TroopTargetManager::getInstance()->getlivingsum();
    if (ArchSum <= 0) ArchSum = 1; // 防止除零错误

    int x_ = 200;
    int y_ = 20;
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建进度标签
    auto prog_label = Label::createWithSystemFont(
        StringUtils::format("摧毁进度：%d%%",
           progress_),
        "Arial", 30);
    prog_label->setPosition(Vec2(230, visibleSize.height - 30));
    prog_label->setName("destoyed_label");
    this->addChild(prog_label);

    // 创建背景框
    auto background = LayerColor::create(Color4B(0, 0, 0, 150), 400, 40);
    background->setPosition(Vec2(30, visibleSize.height - 100));
    background->setName("progress_bg");
    this->addChild(background, 0);

    // 创建进度条
    progressBar_ = cocos2d::ui::LoadingBar::create("LoadingBarFile.png");
    progressBar_->setScaleX(2.3f);
    progressBar_->setScaleY(3.0f);
    progressBar_->setPosition(cocos2d::Vec2(30 + x_, visibleSize.height - 100 + y_));
    progressBar_->setPercent(0);
    progressBar_->setName("progress_bar");
    this->addChild(progressBar_);

    // 创建星星精灵
    for (int i = 0; i < 3; ++i) {
        auto star = createStar();
        star->setScale(0.1f);
        star->setPosition(cocos2d::Vec2(130 + i * 100, visibleSize.height - 150));
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
void AttackStars::checkForUpdates(float dt) {
    if (isShowingVictory) return; // 正在显示胜利画面，不再更新

    CCLOG("=== 进度检查开始 ===");

    // 1. 检查建筑死亡数量
    int currentDeadArch = TroopTargetManager::getInstance()->getDeadsum();
    CCLOG("当前死亡建筑数量：%d", currentDeadArch);

    // 2. 如果有更新，触发UI更新
    if (currentDeadArch != lastDeadArch) {
        CCLOG("检测到更新！上次：%d，本次：%d", lastDeadArch, currentDeadArch);

        // 计算进度百分比 (0-100)
        int newProgress = static_cast<int>((static_cast<float>(currentDeadArch) * 100 / ArchSum));
        auto prog_label = dynamic_cast<cocos2d::Label*>(this->getChildByName("destoyed_label"));

        // 更新进度标签
        if (prog_label) {
            prog_label->setString(StringUtils::format("摧毁进度：%d%%",
                newProgress));
        }

        progress_ = newProgress;
        lastDeadArch = currentDeadArch;

        // 更新进度条
        if (progressBar_) {
            progressBar_->setPercent(newProgress);
        }

        // 星星点亮逻辑
        if (newProgress >= 50 && !isComplete[0]) {
            showPopup(stars_[0], 50);
            setStarColor(stars_[0], true);
            isComplete[0] = true;
        }
        if(isTownStar&&!isComplete[1])
        {
            showPopup(stars_[1], newProgress);
            setStarColor(stars_[1], true);
            isComplete[1] = true;
        }
        // 100%胜利画面
        if (newProgress >= 100 && !isComplete[2]) {
            showPopup(stars_[2], 100);
            setStarColor(stars_[2], true);
            isComplete[2] = true;

            // 显示胜利画面
            showVictoryScreen();
        }
    }

    CCLOG("=== 进度检查结束 ===");
}

// 显示胜利画面
void AttackStars::showVictoryScreen() {
    if (isShowingVictory) return; // 防止重复调用

    isShowingVictory = true;
    CCLOG("显示胜利画面");

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);

    // 1. 创建全屏黑色半透明遮盖
    auto fullScreenMask = LayerColor::create(Color4B(0, 0, 0, 200));
    fullScreenMask->setContentSize(visibleSize);
    fullScreenMask->setPosition(origin);
    fullScreenMask->setName("victory_mask");
    this->addChild(fullScreenMask,-1); // 较高层级

    // 2. 获取所有需要移动到中间的UI元素
    auto progressLabel = dynamic_cast<cocos2d::Label*>(this->getChildByName("destoyed_label"));
    auto progressBg = dynamic_cast<cocos2d::Node*>(this->getChildByName("progress_bg"));
    auto progressBar = dynamic_cast<cocos2d::Node*>(this->getChildByName("progress_bar"));

    // 3. 停止定时器检查
    this->unschedule("update_checker");

    // 4. 创建胜利提示文字
    auto victoryLabel = Label::createWithSystemFont("胜利！所有建筑已被摧毁！", "Arial", 100);
    victoryLabel->setColor(Color3B::YELLOW);
    victoryLabel->setPosition(Vec2(center.x+100, center.y + 200));
    victoryLabel->setName("victory_label");
    fullScreenMask->addChild(victoryLabel);

    // 5. 将所有UI元素移动到屏幕中央（动画）
    float moveDuration = 0.8f;
    float finalY = center.y - 50; // 最终Y坐标

    // 移动进度标签
    if (progressLabel) {
        auto labelMove = MoveTo::create(moveDuration, Vec2(center.x, center.y + 120));
        auto labelScale = ScaleTo::create(moveDuration, 1.5f);
        progressLabel->runAction(Spawn::create(labelMove, labelScale, nullptr));
    }


    // 移动进度条
    if (progressBar) {
        auto barMove = MoveTo::create(moveDuration, Vec2(center.x, finalY));
        auto barScale = ScaleTo::create(moveDuration, 2.5f, 3.0f); // 稍微放大
        progressBar->runAction(Spawn::create(barMove, barScale, nullptr));
    }

    // 移动星星
    for (int i = 0; i < stars_.size(); ++i) {
        if (stars_[i]) {
            float starX = center.x - 100 + i * 100;
            auto starMove = MoveTo::create(moveDuration, Vec2(starX, finalY - 80));
            auto starScale = ScaleTo::create(moveDuration, 0.15f); // 稍微放大
            auto rotate = RotateBy::create(moveDuration, 360);

            // 如果星星还没点亮，先点亮它
            if (!isComplete[i]) {
                setStarColor(stars_[i], true);
                isComplete[i] = true;
            }

            stars_[i]->runAction(Spawn::create(starMove, starScale, rotate, nullptr));
        }
    }

    // 6. 5秒后自动消失
    auto delay = DelayTime::create(5.0f);
    auto fadeOut = FadeOut::create(0.8f);
    auto cleanup = CallFunc::create([this]() {
        // 移除遮盖层
        auto mask = this->getChildByName("victory_mask");
        if (mask) {
            mask->removeFromParent();
        }

        // 重置UI位置到原始位置
        resetUIPosition();

        isShowingVictory = false;
        CCLOG("胜利画面消失");
        });

    fullScreenMask->runAction(Sequence::create(delay, fadeOut, cleanup, nullptr));
}

// 重置UI到原始位置
void AttackStars::resetUIPosition() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 重置进度标签
    auto progressLabel = dynamic_cast<cocos2d::Label*>(this->getChildByName("destoyed_label"));
    if (progressLabel) {
        progressLabel->setPosition(Vec2(230, visibleSize.height - 30));
        progressLabel->setScale(1.0f);
    }

    // 重置背景框
    auto progressBg = dynamic_cast<cocos2d::Node*>(this->getChildByName("progress_bg"));
    if (progressBg) {
        progressBg->setPosition(Vec2(30, visibleSize.height - 100));
    }

    // 重置进度条
    auto progressBar = dynamic_cast<cocos2d::Node*>(this->getChildByName("progress_bar"));
    if (progressBar) {
        progressBar->setPosition(Vec2(230, visibleSize.height - 80));
        progressBar->setScale(2.3f, 3.0f);
    }

    // 重置星星位置
    for (int i = 0; i < stars_.size(); ++i) {
        if (stars_[i]) {
            stars_[i]->setPosition(Vec2(130 + i * 100, visibleSize.height - 150));
            stars_[i]->setScale(0.1f);
        }
    }
}

void AttackStars::showPopup(cocos2d::Sprite* targetStar, int progress) {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);

    // 创建飞行星星精灵
    auto flyingStar = createStar();
    flyingStar->setPosition(center);
    flyingStar->setScale(1.0f);
    flyingStar->setName("flying_star");
    this->addChild(flyingStar, 10); // 较高层级

    // 创建提示文本
    std::string text;
    if (progress == 50) {
        text = "摧毁进度超过50%！";
    }
    if(isTownStar) text = "敌方大本营已被摧毁！";
    else if (progress == 100) {
        text = "所有建筑已被摧毁！";
    }

    auto progressText = cocos2d::Label::createWithSystemFont(text, "Arial", 40);
    progressText->setColor(Color3B::YELLOW);
    progressText->setPosition(Vec2(center.x, center.y - 200));
    progressText->setName("popup_text");
    this->addChild(progressText, 10);

    // 飞行动画：飞到目标星星位置
    auto moveTo = MoveTo::create(2.0f, targetStar->getPosition());
    auto rotateBy = RotateBy::create(2.0f, 360);
    auto scaleTo = ScaleTo::create(2.0f, 0.1f);
    auto fadeOut = FadeOut::create(2.0f);

    auto spawn = Spawn::create(moveTo, rotateBy, scaleTo, fadeOut, nullptr);

    // 文字淡出动画
    auto textFadeOut = FadeOut::create(1.5f);

    // 清理函数
    auto cleanup = CallFunc::create([flyingStar, progressText]() {
        flyingStar->removeFromParent();
        progressText->removeFromParent();
        });

    // 执行动画
    flyingStar->runAction(Sequence::create(spawn, cleanup, nullptr));
    progressText->runAction(Sequence::create(DelayTime::create(0.5f), textFadeOut, nullptr));
}
void AttackStars::onTownHallDeath(cocos2d::EventCustom* event) {
    isTownStar = 1;
    checkForUpdates();
}

// 析构函数
AttackStars::~AttackStars() {
    if (deadArchUpdateListener) {
        cocos2d::Director::getInstance()->getEventDispatcher()
            ->removeEventListener(deadArchUpdateListener);
    }
}

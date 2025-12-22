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
    int x_ = 200;
    int y_ = 20;
    // 获取屏幕尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto prog_label = Label::createWithSystemFont(StringUtils::format("摧毁进度：%d  %%", progress_), "Arial", 30);
    prog_label->setPosition(Vec2(230, visibleSize.height - 30));
    prog_label->setName("destoyed_label");
    this->addChild(prog_label);

    // 创建背景框
    auto background = LayerColor::create(Color4B(0, 0, 0, 150), 400, 40); // 黑色半透明
    background->setPosition(Vec2(30, visibleSize.height - 100)); // 设置位置
    this->addChild(background, 0);

    // 创建进度条
    progressBar_ = cocos2d::ui::LoadingBar::create("LoadingBarFile.png");
    progressBar_->setScaleX(2.3f);
    progressBar_->setScaleY(3.0f);
    progressBar_->setPosition(cocos2d::Vec2(30 + x_, visibleSize.height - 100 + y_));
    progressBar_->setPercent(0); // 初始为0%

    this->addChild(progressBar_);

    // 创建星星精灵并将其放入 vector
    for (int i = 0; i < 3; ++i) {
        auto star = createStar();
        star->setScale(0.1f);
        star->setPosition(cocos2d::Vec2(130 + i * 100, visibleSize.height - 150)); // 星星的位置
        stars_.push_back(star); // 添加星星到 vector
        this->addChild(star);
        setStarColor(star, false); // 初始化星星颜色为灰色
    }
    int newdeath = TroopTargetManager::getInstance()->getlivingsum();
    // 注册更新事件监听
    //deadArchUpdateListener = cocos2d::EventListenerCustom::create("update_deadArchs_event", CC_CALLBACK_1(AttackStars::onDestroyedUpdated, this));
    //cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(deadArchUpdateListener, this);
    // 每隔5秒执行一次checkForUpdates
    this->schedule(CC_CALLBACK_1(AttackStars::checkForUpdates, this), 2.0f, "update_checker");
    return true;

}
// 检查更新的函数
void AttackStars::checkForUpdates(float dt) {
    CCLOG("=== 2秒检查开始 ===");

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
            prog_label->setString(StringUtils::format("摧毁进度：%d  %%", newProgress));
        }
        progress_ = newProgress;
        // 保存当前值
        lastDeadArch = currentDeadArch;
        // 更新进度条
        if (progressBar_) {
            progressBar_->setPercent(newProgress);
        }

        if (newProgress >= 50 && !isComplete[0]) {
            showPopup(stars_[0], 50);
            setStarColor(stars_[0], true);
            isComplete[0] = 1;
        }
        if (newProgress == 100 && !isComplete[2]) {
            showPopup(stars_[2], newProgress);
            setStarColor(stars_[2], true);
            isComplete[2] = 1;
        }
    }


    CCLOG("=== 2秒检查结束 ===");
}


void AttackStars::showPopup(cocos2d::Sprite* targetStar, int progress) {
    // 获取屏幕尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建飞行星星精灵
    auto flyingStar = createStar();
    flyingStar->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2));  // 从屏幕中央起飞
    flyingStar->setScale(1.0f); // 初始大小设置为1
    this->addChild(flyingStar);

    // 创建提示文本
    auto progressText = cocos2d::Label::createWithSystemFont(
        StringUtils::format("摧毁进度达到%d %% %！", progress), "Arial", 40);
    if (progress == 50)progressText->setString("摧毁进度超过50%！");
    progressText->setPosition(cocos2d::Vec2(visibleSize.width / 2, visibleSize.height / 2 - 200)); // 设置文字显示位置
    this->addChild(progressText);

    // 飞行动画：飞到目标位置并添加旋转和缩放
    auto moveTo = cocos2d::MoveTo::create(2.0f, targetStar->getPosition());  // 飞到目标星星位置
    auto rotateBy = cocos2d::RotateBy::create(2.0f, 360);  // 缓慢旋转360度
    auto scaleTo = cocos2d::ScaleTo::create(2.0f, 0.1f);  // 到达目标位置时缩小到0.1倍大小
    auto fadeOut = cocos2d::FadeOut::create(2.0f);  // 飞到后淡出

    // 创建旋转和缩放的组合动作
    auto rotateAndScale = cocos2d::Spawn::create(rotateBy, scaleTo, nullptr);

    // 创建飞行动作和淡出
    auto spawn = cocos2d::Spawn::create(moveTo, rotateAndScale, fadeOut, nullptr);

    // 创建文字淡出动画
    auto fadeOutText = cocos2d::FadeOut::create(1.5f);

    // 在动画结束后移除飞行星星和提示文本
    auto cleanup = cocos2d::CallFunc::create([flyingStar, progressText] {
        flyingStar->removeFromParent();
        progressText->removeFromParent();
        });

    // 组合动画：星星的飞行、旋转、缩放和淡出
    flyingStar->runAction(cocos2d::Sequence::create(spawn, cleanup, nullptr));
    progressText->runAction(fadeOutText);

}

//析构函数
AttackStars::~AttackStars() {
    if (deadArchUpdateListener) {
        cocos2d::Director::getInstance()->getEventDispatcher()
            ->removeEventListener(deadArchUpdateListener);
    }
}
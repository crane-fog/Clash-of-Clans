#ifndef __MAIN_VILLAGE_SCENE_H__
#define __MAIN_VILLAGE_SCENE_H__

#include "Arch.h"
#include "AudioEngine.h"
#include "cocos2d.h"
#include "CocUtility.h"
#include "VillageScene.h"
#include "Barbarian.h"
// 主村庄场景类
class MainVillage : public Village {
private:
    // 村庄中的建筑状态数据
    ArchData arch_status_[kMapSize][kMapSize];

    // 上次退出场景的时间
    time_t last_exit_time_ = 0;

public:
    // 初始化，当对象被创建时被自动调用
    virtual bool init() override;

    // 当对象被渲染时被自动调用
    virtual void onEnter() override;

    // 当对象退出渲染时被自动调用
    virtual void onExit() override;

    // 当对象被销毁时被自动调用
    virtual void cleanup() override;

    // 静态创建函数，替代构造函数，会将创建的对象自动放入自动释放池
    CREATE_FUNC(MainVillage);


    /* 按钮回调函数 */
    void onShopButtonClick(Ref* sender);
    void onAttackButtonClick(Ref* sender);
    void onTroopButtonClick(Ref* sender);
    void onLabButtonClick(Ref* sender);
    void onMessageButtonClick(Ref* sender);
    void onTroopUpradeClick(Ref* sender, Widget::TouchEventType type, unsigned char& it, cocos2d::LayerColor* panel);


    /* 建筑建造相关 */
    // 获取大本营等级
    unsigned char getTownHallLevel();

    // 获取指定建筑数量，用于在建造建筑时限制建筑数量
    int getBuildingCount(unsigned char archNo);

    bool addBuildingByNO(unsigned char no, int price);

    void MainVillage::createCancelButton(Arch* pendingArch_);

    void MainVillage::createConfirmButton(Arch* pendingArch_, int price, bool type_);

    void MainVillage::cancelBuildingPlacement(Arch* pendingArch_);

    void MainVillage::confirmBuildingPlacement(Arch* pendingArch_);

    void MainVillage::removeCancelAndConfirmButtons(Arch* pendingArch_);

    void MainVillage::playBuildingDropEffect(Arch* arch);


    // 背景音乐
    int mainhome_bgm_ = cocos2d::AudioEngine::play2d("music/mainhome.mp3", true);
};


    // 按钮回调函数
    void onAttackButtonClick(Ref* sender);
    void onTroopButtonClick(Ref* sender);
    void onLabButtonClick(Ref* sender);
    void onMessageButtonClick(Ref* sender);
    void onTroopUpradeClick(Ref* sender, Widget::TouchEventType type, unsigned char& it, cocos2d::LayerColor* panel);

    // 获取大本营等级
    unsigned char getTownHallLevel();

    // 获取指定建筑数量
    int getBuildingCount(unsigned char archNo);

    // void onReplayButtonClick(cocos2d::Ref* sender, int gold_, int elixir_, bool isReplay = 1);
     cocos2d::Vec2 barracksPosition = cocos2d::Vec2::ZERO;
    // 定义巡逻类型枚举
    enum PatrolType {
        PATROL_SEQUENCE = 0,  // 顺序巡逻
        PATROL_CIRCULAR = 1,  // 圆形巡逻
        PATROL_BEZIER = 2,    // 贝塞尔曲线巡逻
        PATROL_RANDOM = 3     // 随机巡逻
    };

    // 巡逻半径参数
    float patrolRadius = 80.0f;
    // 创建顺序巡逻动作
    cocos2d::Action* createSequencePatrol(const cocos2d::Vec2& center, float radius);

    // 创建圆形巡逻动作
    cocos2d::Action* createCircularPatrol(const cocos2d::Vec2& center, float radius);

    // 创建贝塞尔曲线巡逻动作
    cocos2d::Action* createBezierPatrol(const cocos2d::Vec2& center, float radius);

    // 创建随机巡逻动作
    cocos2d::Action* createRandomPatrol(const cocos2d::Vec2& center, float radius);

    // 通用巡逻选择函数
    cocos2d::Action* createPatrolAction(PatrolType patrolType, const cocos2d::Vec2& center, float radius = 80.0f);

    // 添加巡逻动画效果
    void addPatrolEffects(cocos2d::Node* target, bool addScaleEffect = true, bool addParticles = false);

    // 生成巡逻点列表（辅助函数）
    std::vector<cocos2d::Vec2> generatePatrolPoints(const cocos2d::Vec2& center, float radius, int pointCount,
                                                    bool isCircle = true);

    // 根据点列表创建巡逻动作
    cocos2d::Action* createPatrolFromPoints(const std::vector<cocos2d::Vec2>& points, float moveTime, float waitTime);


    // 设置野蛮人巡逻
    void setupBarbarianPatrol(Barbarian* barbarian, const cocos2d::Vec2& barracksPosition,
                              PatrolType patrolType = PATROL_BEZIER, float patrolRadius = 80.0f)
    {
        if (!barbarian) return;

        // 创建巡逻动作
        cocos2d::Action* patrolAction = createPatrolAction(patrolType, barracksPosition, patrolRadius);

        if (patrolAction) {
            barbarian->runAction(patrolAction);
        }

        // 添加巡逻效果
        addPatrolEffects(barbarian, true, false);
    }
    };
#endif  // __MAIN_VILLAGE_SCENE_H__

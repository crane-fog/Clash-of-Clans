#ifndef __ARCH_H__
#define __ARCH_H__

#include <string.h>

#include <algorithm>

#include "ArchInfo.h"
#include "HealthBar.h"
#include "ITroopTarget.h"
#include "ResourceManager.h"
#include "TroopConfig.h"
#include "TroopTargetManager.h"
#include "UIparts.h"

class BaseMap;

class Arch : public cocos2d::Sprite, public ITroopTarget {
    typedef unsigned int UI;
    typedef unsigned char UC;

protected:
    /* 建筑属性 */
    // 建筑种类的编号
    UC no_;
    // 建筑等级
    UC level_;
    // 建筑位置
    UC x_;
    UC y_;
    // 当前生命值
    int current_hp_;

    // 当前剩余升级时间
    UI remaining_upgrade_time_;

    // 当前容量（仅适用于资源生产建筑，储存建筑的容量在游戏中由ResourceManager类管理）
    UI current_capacity_;

    /* 攻击相关 */
    // 计时器
    float attack_timer_ = 0.0f;

    IArchTarget* current_target_ = nullptr;

    virtual void update(float dt) override;

    void tryAttack(float dt);

    /* 杂项 */
    // 所在的地图指针
    BaseMap* base_map_;

    cocos2d::EventListenerTouchOneByOne* touch_listener_ = nullptr;

    // 指示是否为自己的建筑
    bool is_mine_;

    // 生命条指针
    HealthBar* health_bar_;

private:
    /* 建筑面板及升级相关 */
    void closeArchPanel();
    void showRefusePopup(std::string text_);
    static std::string getArchNameFromEnum(unsigned char archNo);
    bool is_upgrading_ = false;
    // 检查是否可以升级
    void archUpgrade();
    // 取消升级
    void onUpgradeCancel(Ref* sender);
    // 开始升级动画
    void startUpgradeAnimation(unsigned int time, const std::string& notice);
    // 升级完成回调
    virtual void onUpgradeFinished() {}



public:
    Arch(const ArchData& data, BaseMap* base_map, bool is_mine)
        : no_(data.no_),
          level_(data.level_),
          x_(data.x_),
          y_(data.y_),
          current_hp_(kArchInfo.at(no_)[level_ - 1].hp_),
          remaining_upgrade_time_(data.remaining_upgrade_time_),
          current_capacity_(data.current_capacity_),
          base_map_(base_map),
          is_mine_(is_mine)
    {}
    static Arch* create(const ArchData& data, BaseMap* base_map, bool is_mine = true);
    virtual bool initWithFile(const std::string& filename) override;
    virtual void onEnter() override;
    virtual void onExit() override;

    /* 建筑拖动相关 */
    bool is_dragging_ = false;
    cocos2d::Vec2 touch_start_pos_;
    cocos2d::Node* highlight_node_ = nullptr;

    // 拖动前的原始位置
    UC original_x_;
    UC original_y_;

    void createHighlight();
    void updateHighlightPos();
    void removeHighlight();
    void updateHighlightColor(bool collision);
    bool checkCollision(int x, int y);

    // 为城墙状态更新预留的接口
    virtual void updateWall(Arch* moving_wall = nullptr, bool is_moving = false) {}
    virtual void updateSurroundingWalls(int x, int y, bool is_moving = false) {}

    /* 触摸事件回调 */
    bool onTouchDown(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchUp(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMove(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchCancel(cocos2d::Touch* touch, cocos2d::Event* event);

    /* ITroopTarget 接口实现 */
    virtual void onDeath();
    virtual void takeDamage(float damage) override;
    virtual cocos2d::Vec2 getCellPosition(float& size) const override;
    virtual bool isAlive() const override { return current_hp_ > 0; }
    virtual UC getTargetType() const override { return kArchInfo.at(no_)[level_ - 1].type_; }

    /* 建筑面板及升级相关 */
    //展示建筑信息面板
    virtual void showArchPanel();
    // 显示升级信息对比面板
    virtual void createUpgradeComparisonPanel();
    // 更新剩余升级时间
    void updateUpgradeTime(long long elapsed);
    // 检查升级资源并显示升级效果
    void buidingUpgrading(Ref* sender, Arch* arch, bool a, unsigned int cost, unsigned long long currentGold,
                          bool type);
    // 更新建筑资源的显示
    void updateBuildingDisplay();

    /* 杂项 */
    // 资源生产
    void startResourceProduction();

    
    // 建筑是否被摧毁
    bool is_destroyed_ = false;

    UI getx() const { return this->x_; }
    // 获取y坐标
    UI gety() const { return this->y_; }
    // 获取建筑编号
    UC getNo() const { return no_; }
    // 获取建筑等级
    UC getLevel() const { return level_; }

    friend class ShopPopup;

    friend struct ArchData;
};

class TownHall : public Arch {
public:
    TownHall(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine) {}
    virtual void onDeath() override;
};

class Wall : public Arch {
private:
    std::vector<cocos2d::Node*> connection_nodes_;

public:
    Wall(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine) {}
    virtual void updateWall(Arch* moving_wall = nullptr, bool is_moving = false) override;
    virtual void updateSurroundingWalls(int x, int y, bool is_moving = false) override;
    virtual void onDeath() override;
};

class GoldStorage : public Arch {
public:
    GoldStorage(const ArchData& data, BaseMap* base_map, bool is_mine);
    virtual void showArchPanel() override;
    virtual void createUpgradeComparisonPanel() override;
    virtual void onUpgradeFinished() override;
};

class ElixirStorage : public Arch {
public:
    ElixirStorage(const ArchData& data, BaseMap* base_map, bool is_mine);
    virtual void showArchPanel() override;
    virtual void createUpgradeComparisonPanel() override;
    virtual void onUpgradeFinished() override;
};

class GoldMine : public Arch {
public:
    GoldMine(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine) {}
    virtual void showArchPanel() override;
    virtual void createUpgradeComparisonPanel() override;
    virtual void onUpgradeFinished() override { startResourceProduction(); }
};

class ElixirCollector : public Arch {
public:
    ElixirCollector(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine) {}
    virtual void showArchPanel() override;
    virtual void createUpgradeComparisonPanel() override;
    virtual void onUpgradeFinished() override { startResourceProduction(); }
};

class Barracks : public Arch {
public:
    Barracks(const ArchData& data, BaseMap* base_map, bool is_mine);
    virtual void showArchPanel() override;
    virtual void createUpgradeComparisonPanel() override;
    virtual void onUpgradeFinished() override;
};

class ArmyCamp : public Arch {
public:
    ArmyCamp(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine)
    {
        TroopConfig::getInstance()->setArmyCampCapacity(kArmyCampCapacity[level_ - 1]);
    }
    virtual void showArchPanel() override;
    virtual void createUpgradeComparisonPanel() override;
    virtual void onUpgradeFinished() override;
};

class Cannon : public Arch {
public:
    Cannon(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine) {}
    virtual void showArchPanel() override;
};

class ArcherTower : public Arch {
public:
    ArcherTower(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine) {}
    virtual void showArchPanel() override;
};

class Bomb : public Arch {
public:
    Bomb(const ArchData& data, BaseMap* base_map, bool is_mine);
    virtual void showArchPanel() override;
    virtual void update(float dt) override;
    virtual void onDeath() override;
};

// 建筑工厂类
class ArchFactory {
    using Creater = std::function<Arch*(const ArchData&, BaseMap*, bool)>;

private:
    // 存储各类建筑创建函数的映射表
    static std::map<unsigned char, Creater> creaters;

public:
    // 注册建筑创建函数，在游戏初始化时每种建筑调用一次
    static void registerCreater(unsigned char no, const Creater& creater) { creaters[no] = creater; }

    // 创建建筑实例
    static Arch* createArch(const ArchData& data, BaseMap* base_map, bool is_mine);
};
#endif  // __ARCH_H__
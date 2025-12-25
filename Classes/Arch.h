#ifndef __ARCH_H__
#define __ARCH_H__

#include <string.h>
#include <algorithm>

#include "ArchInfo.h"
#include "ResourceManager.h"
#include "HealthBar.h"
#include "ITroopTarget.h"
#include "TroopConfig.h"
#include "TroopTargetManager.h"
#include "UIparts.h"

class BaseMap;

class Arch : public cocos2d::Sprite, public ITroopTarget {
    typedef unsigned int UI;
    typedef unsigned char UC;

protected:
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

    // 资源建筑
    // 当前容量（仅适用于生产建筑，储存建筑的容量在游戏中由ResourceManager类管理）
    UI current_capacity_;

    // 所在的地图指针
    BaseMap* base_map_;
    cocos2d::EventListenerTouchOneByOne* touch_listener_ = nullptr;

    // 指示是否为自己的建筑（拖动）
    bool is_mine_;

    // 生命条指针
    HealthBar* health_bar_;

    // 攻击相关
    float attack_timer_ = 0.0f;
    IArchTarget* current_target_ = nullptr;

    virtual void update(float dt) override;

    void tryAttack(float dt);

public:
    // 建筑是否被摧毁
    bool is_destroyed_ = false;

    // 拖动相关
    bool is_dragging_ = false;
    cocos2d::Vec2 touch_start_pos_;
    cocos2d::Node* highlight_node_ = nullptr;

    // 记录拖动前的原始位置
    UC original_x_;
    UC original_y_;

    void createHighlight();
    void updateHighlightPos();
    void removeHighlight();
    void updateHighlightColor(bool collision);
    bool checkCollision(int x, int y);

    // 触摸事件回调
    bool onTouchDown(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchUp(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMove(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchCancel(cocos2d::Touch* touch, cocos2d::Event* event);

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

    // 为城墙状态更新预留的接口
    virtual void updateWall(Arch* moving_wall = nullptr, bool is_moving = false) {}
    virtual void updateSurroundingWalls(int x, int y, bool is_moving = false) {}

    // 升级完成回调
    virtual void onUpgradeFinished() {}

    // ITroopTarget 接口实现
    virtual void onDeath();
    virtual void takeDamage(float damage) override;
    virtual cocos2d::Vec2 getCellPosition(float& size) const override;
    virtual bool isAlive() const override { return current_hp_ > 0; }
    virtual UC getTargetType() const override { return kArchInfo.at(no_)[level_ - 1].type_; }

    // 建筑面板UI相关
    virtual void showArchPanel();
    bool is_upgrading_ = false;

    // 关闭建筑信息面板
    void closeArchPanel();

    // 创建显示的弹窗
    void showRefusePopup(std::string text_);

    static std::string getArchNameFromEnum(unsigned char archNo);

    // 升级相关
    void archUpgrade();
    virtual void createUpgradeComparisonPanel();
    void onUpgradeCancel(Ref* sender);
    void buidingUpgrading(Ref* sender, Arch* arch, bool a, unsigned int cost, unsigned long long currentGold,
                                 bool type);
    // 资源生产
    void startResourceProduction();

    // 更新建筑的显示
    void updateBuildingDisplay();

    // 开始升级动画
    void startUpgradeAnimation(unsigned int time, const std::string& notice);

    // 更新剩余升级时间
    void updateUpgradeTime(long long elapsed);

    UI getx() const { return this->x_; }
    UI gety() const { return this->y_; }
    UC getNo() const { return no_; }
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
    Bomb(const ArchData& data, BaseMap* base_map, bool is_mine) : Arch(data, base_map, is_mine) {}
    virtual void showArchPanel() override;
    virtual void update(float dt) override;
};

class ArchFactory {
    using Creater = std::function<Arch*(const ArchData&, BaseMap*, bool)>;

private:
    static std::map<unsigned char, Creater> creaters;

public:
    static void registerCreater(unsigned char no, const Creater& creater) { creaters[no] = creater; }
    static Arch* createArch(const ArchData& data, BaseMap* base_map, bool is_mine);
};
#endif  // __ARCH_H__
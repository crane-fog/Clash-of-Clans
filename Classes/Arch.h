#ifndef __ARCH_H__
#define __ARCH_H__

#include "cocos2d.h"
#include "ArchInfo.h"
#include "ITroopTarget.h"
#include"UIparts.h"
#include <string.h>

class BaseMap;
class Arch;



// 建筑数据
struct ArchData {
    typedef unsigned int UI;
    typedef unsigned char UC;
    // 建筑种类的编号
    UC no_;
    // 建筑等级
    UC level_;
    // 建筑位置
    UC x_;
    UC y_;

    // 当前生命值
    UI current_hp_;

    // 资源建筑
    // 当前容量
    UI current_capacity_;

    ArchData() = default;
    explicit ArchData(Arch*a);
    explicit ArchData(UC no, UC lv = 1) : no_(no), level_(lv), x_(1), y_(1),
        current_hp_(kArchInfo.at(no)[lv].hp_), current_capacity_(kArchInfo.at(no)[lv].max_capacity_) {}
};

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

    // 资源建筑
    // 当前容量
    UI current_capacity_;

    // 所在的地图指针
    BaseMap* base_map_;
public:
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

    Arch(const ArchData& data, BaseMap* base_map) : no_(data.no_), level_(data.level_), x_(data.x_), y_(data.y_),
        current_hp_(kArchInfo.at(no_)[level_ - 1].hp_), current_capacity_(data.current_capacity_), base_map_(base_map) {}
    static Arch* create(const ArchData& data, BaseMap* base_map);
    virtual bool initWithFile(const std::string& filename) override;
    virtual void onEnter() override;

    // 为城墙状态更新预留的接口
    virtual void updateWall(Arch* moving_wall = nullptr, bool is_moving = false) {}
    virtual void updateSurroundingWalls(int x, int y, bool is_moving = false) {}

    // ITroopTarget 接口实现
    virtual void takeDamage(float damage) override { current_hp_ -= static_cast<UI>(damage); }
    virtual cocos2d::Vec2 getCellPosition(float& size) const override
    { 
        size = static_cast<float>(kArchInfo.at(no_)[level_ - 1].size_);
        return cocos2d::Vec2(x_ + size / 2.0f, y_ + size / 2.0f);
    }
    virtual bool isAlive() const override { return current_hp_ > 0; }
    virtual UC getTargetType() const override { return kArchInfo.at(no_)[level_ - 1].type_; }

    // 建筑面板UI相关
    void showArchPanel(Arch* arch);
    void closeArchPanel();
    //升级按钮
    void Arch::archUpgrade(Arch* arch);
    // 创建显示"当前已是最高等级"的弹窗
    void Arch::showMaxLevelPopup();
    std::string getArchNameFromEnum(unsigned char archNo);
    void Arch::createUpgradeComparisonPanel(Arch* arch);

    void Arch::onUpgradeCancel(Ref* sender);
    void Arch::Buiding_Upgrading(Ref* sender, Arch* arch,bool a);
    UI getx(Arch * data) {
        return data->x_;
      }
    UI getx() {
        return this->x_;
    }
    UI gety() {
        return this->y_;
    }


    friend class ShopPopup;

    friend struct ArchData;
};

class Wall : public Arch {
    std::vector<cocos2d::Node*> connection_nodes_;
public:
    Wall(const ArchData& data, BaseMap* base_map) : Arch(data, base_map) {}
    virtual void updateWall(Arch* moving_wall = nullptr, bool is_moving = false) override;
    virtual void updateSurroundingWalls(int x, int y, bool is_moving = false) override;
};

#endif // __ARCH_H__
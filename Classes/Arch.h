#ifndef __ARCH_H__
#define __ARCH_H__

#include "cocos2d.h"
#include "BaseMap.h"
#include "ArchInfo.h"
#include<string.h>

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
};

class Arch : public cocos2d::Sprite {
    typedef unsigned int UI;
    typedef unsigned char UC;
private:
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


    UC getNo() const { return no_; }
    UC getLevel() const { return level_; }
    UI getCurrentHP() const { return current_hp_; }
    UI getCurrentCapacity() const { return current_capacity_; }
    BaseMap* base_map_;
public:
    Arch(const ArchData& data, BaseMap* base_map) : no_(data.no_), level_(data.level_), x_(data.x_), y_(data.y_),
        current_hp_(kArchInfo.at(no_)[level_ - 1].hp_), current_capacity_(data.current_capacity_), base_map_(base_map) {}
    static Arch* create(const ArchData& data, BaseMap* base_map);
    virtual bool initWithFile(const std::string& filename) override;
    void showArchPanel(Arch* arch);
    void closeArchPanel();

    std::string getArchNameFromEnum(unsigned char archNo);
};

#endif // __ARCH_H__
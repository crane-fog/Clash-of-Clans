#ifndef __ARCH_H__
#define __ARCH_H__

// 一张地图上的一格的建筑状况
struct ArchStatus {
    typedef unsigned int UI;
    typedef unsigned char UC;
    // 建筑种类的编号
    UC no_;
    // 建筑等级
    UC level_;

    // 当前生命值
    UI current_hp_;

    // 资源建筑
    // 当前容量
    UI current_capacity_;
};

#endif // __ARCH_H__
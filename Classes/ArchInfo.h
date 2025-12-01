#ifndef __ARCH_INFO_H__
#define __ARCH_INFO_H__
#include <map>
#include <string>
#include <vector>

// 示例用法：取3级箭塔的最大生命值
// unsigned int a = kArchInfoData[ARCHER_TOWER][2].hp_;

enum ArchName : unsigned char {
    TOWN_HALL = 0, // 大本营
    WALL = 1, // 城墙
    GOLD_STORAGE = 10, // 金库
    ELIXIR_STORAGE = 11, // 圣水罐
    GOLD_MINE = 12, // 金矿
    ELIXIR_COLLECTOR = 13, // 圣水收集器
    BARRACKS = 20, // 训练营
    ARMY_CAMP = 21, // 兵营
    CANNON = 30, // 加农炮
    ARCHER_TOWER = 31 // 箭塔
};

enum ArchType : unsigned char {
    OTHER = 0, // 其它
    RESOURCE = 1, // 资源
    DEFENSE = 2, // 防御
    WALL = 3 // 城墙
};

// 用于存放建筑物信息
struct ArchInfo {
    typedef unsigned int UI;
    typedef unsigned char UC;
    // 建筑种类的编号
    UC no_;
    // 建筑等级
    UC level_;
    // 建筑占地大小（x=x*x）
    UC size_;
    // 建筑类型（1=资源建筑（攻击可得到资源）、2=防御建筑、3=城墙、0=其它）
    UC type_;

    // 建筑生命值
    UI hp_;

    // 资源建筑
    // 最大容量
    UI max_capacity_;
    // 生产速率/每分钟（对于存储建筑而言就是0）
    UI produce_speed_;

    // 防御建筑
    // 每次攻击伤害
    UI damage_;
    // 攻击间隔毫秒数
    UI attack_interval_;
    // 攻击范围（乘10，如攻击半径为5.5格则存55，攻击范围的圆的中心为建筑网格坐标的中心点）
    UI attack_range_;
    // 目标类型（0=地面，1=空中，2=地面+空中）
    UC target_type_;
    // 伤害类型（0=单体，1=区域溅射）
    UC damage_type_;
    // 伤害溅射范围（伤害类型为0时则为0）
    float splash_range_;

    ArchInfo(UC no, UC lvl, UC size, UC type, UI hp, UI cap = 0, UI prod = 0, UI dmg = 0,
        UI atkInt = 0, UI range = 0, UC tgt = 0, UC dmgType = 0, float splash = 0.0f) :
        no_(no), level_(lvl), size_(size), type_(type), hp_(hp), max_capacity_(cap), produce_speed_(prod), damage_(dmg),
        attack_interval_(atkInt), attack_range_(range), target_type_(tgt), damage_type_(dmgType), splash_range_(splash)
    {
    }
};

// 一张地图上的一格的建筑状况
struct ArchData {
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

    ArchData(UC no, UC lvl, UI hp, UI cap = 0) :
        no_(no), level_(lvl), current_hp_(hp), current_capacity_(cap)
    {
    }
};

// 建筑物信息数据
const std::map<unsigned char, std::vector<ArchInfo>> kArchInfoData = {
    {TOWN_HALL, {
        {TOWN_HALL, 1, 4, OTHER, 10000},
        {TOWN_HALL, 2, 4, OTHER, 20000},
        {TOWN_HALL, 3, 4, OTHER, 30000},
        {TOWN_HALL, 4, 4, OTHER, 40000}}},
    {WALL, {
        {WALL, 1, 1, WALL, 5000},
        {WALL, 2, 1, WALL, 10000},
        {WALL, 3, 1, WALL, 15000},
        {WALL, 4, 1, WALL, 20000}}},
    {GOLD_STORAGE, {
        {GOLD_STORAGE, 1, 3, RESOURCE, 10000, 50000},
        {GOLD_STORAGE, 2, 3, RESOURCE, 20000, 100000},
        {GOLD_STORAGE, 3, 3, RESOURCE, 30000, 150000},
        {GOLD_STORAGE, 4, 3, RESOURCE, 40000, 200000}}},
    {ELIXIR_STORAGE, {
        {ELIXIR_STORAGE, 1, 3, RESOURCE, 10000, 50000},
        {ELIXIR_STORAGE, 2, 3, RESOURCE, 20000, 100000},
        {ELIXIR_STORAGE, 3, 3, RESOURCE, 30000, 150000},
        {ELIXIR_STORAGE, 4, 3, RESOURCE, 40000, 200000}}},
    {GOLD_MINE, {
        {GOLD_MINE, 1, 2, RESOURCE, 1000, 5000, 500},
        {GOLD_MINE, 2, 2, RESOURCE, 2000, 10000, 1000},
        {GOLD_MINE, 3, 2, RESOURCE, 3000, 15000, 1500},
        {GOLD_MINE, 4, 2, RESOURCE, 4000, 20000, 2000}}},
    {ELIXIR_COLLECTOR, {
        {ELIXIR_COLLECTOR, 1, 2, RESOURCE, 1000, 5000, 500},
        {ELIXIR_COLLECTOR, 2, 2, RESOURCE, 2000, 10000, 1000},
        {ELIXIR_COLLECTOR, 3, 2, RESOURCE, 3000, 15000, 1500},
        {ELIXIR_COLLECTOR, 4, 2, RESOURCE, 4000, 20000, 2000}}},
    {BARRACKS, {
        {BARRACKS, 1, 2, OTHER, 1000},
        {BARRACKS, 2, 2, OTHER, 2000},
        {BARRACKS, 3, 2, OTHER, 3000},
        {BARRACKS, 4, 2, OTHER, 4000}}},
    {ARMY_CAMP, {
        {ARMY_CAMP, 1, 3, OTHER, 1000},
        {ARMY_CAMP, 2, 3, OTHER, 2000},
        {ARMY_CAMP, 3, 3, OTHER, 3000},
        {ARMY_CAMP, 4, 3, OTHER, 4000}}},
    {CANNON, {
        {CANNON, 1, 2, DEFENSE, 2000, 0, 0, 50, 1000, 60},
        {CANNON, 2, 2, DEFENSE, 4000, 0, 0, 60, 1000, 60},
        {CANNON, 3, 2, DEFENSE, 6000, 0, 0, 70, 1000, 60},
        {CANNON, 4, 2, DEFENSE, 8000, 0, 0, 80, 1000, 60}}},
    {ARCHER_TOWER, {
        {ARCHER_TOWER, 1, 2, DEFENSE, 2000, 0, 0, 50, 800, 70, 2},
        {ARCHER_TOWER, 2, 2, DEFENSE, 4000, 0, 0, 60, 800, 70, 2},
        {ARCHER_TOWER, 3, 2, DEFENSE, 6000, 0, 0, 70, 800, 70, 2},
        {ARCHER_TOWER, 4, 2, DEFENSE, 8000, 0, 0, 80, 800, 70, 2}}}
};

#endif // __ARCH_INFO_H__
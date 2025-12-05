#ifndef __ARCH_INFO_H__
#define __ARCH_INFO_H__
#include <map>
#include <string>
#include <vector>

// 示例用法：取3级箭塔的最大生命值
// unsigned int a = kArchInfo.at(ARCHER_TOWER)[2].hp_;

const unsigned char INVALID_ARCH_NO = 255;

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
    WALLT = 3 // 城墙
};

enum ResourceType : unsigned char {
    GOLD = 0, // 金币
    ELIXIR = 1 // 圣水
};

// 用于存放建筑物信息
struct ArchInfo {
    typedef unsigned int UI;
    typedef unsigned char UC;
    // 建筑图片文件
    std::string image_;
    // 建筑等级
    UC level_;
    // 建筑占地大小（x=x*x）
    UC size_;
    // 建筑类型（1=资源建筑（攻击可得到资源）、2=防御建筑、3=城墙、0=其它）
    UC type_;

    // 升到这一级所需要的资源类型（0=金币，1=圣水）（在1级时即为建筑初始建造时长）
    UC upgrade_cost_type_;
    // 升到这一级所需要的资源数量
    UI upgrade_cost_amount_;
    // 升级时间秒数
    UI upgrade_time_;
    // 建筑生命值
    int hp_;

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

    ArchInfo(std::string i, UC lvl, UC size, UC type, UC uct, UI uca, UI ut, int hp, UI cap = 0,
        UI prod = 0, UI dmg = 0, UI atkInt = 0, UI range = 0, UC tgt = 0, UC dmgType = 0, float splash = 0.0f) :
        image_(i), level_(lvl), size_(size), type_(type), upgrade_cost_type_(uct), upgrade_cost_amount_(uca), upgrade_time_(ut), hp_(hp), max_capacity_(cap),
        produce_speed_(prod), damage_(dmg), attack_interval_(atkInt), attack_range_(range), target_type_(tgt), damage_type_(dmgType), splash_range_(splash)
    {
    }
};

// 建筑物信息数据
const std::map<unsigned char, std::vector<ArchInfo>> kArchInfo = {
    {TOWN_HALL, {
        {"arch/Town_Hall1.webp", 1, 4, OTHER, GOLD, 0, 0, 400},
        {"arch/Town_Hall2.webp", 2, 4, OTHER, GOLD, 1000, 10, 800},
        {"arch/Town_Hall3.webp", 3, 4, OTHER, GOLD, 4000, 1800, 1600},
        {"arch/Town_Hall4.webp", 4, 4, OTHER, GOLD, 25000, 10800, 2000}}},
    {WALL, {
        {"arch/Wall1.webp", 1, 1, WALLT, GOLD, 0, 0, 100},
        {"arch/Wall2.webp", 2, 1, WALLT, GOLD, 1000, 0, 200},
        {"arch/Wall3.webp", 3, 1, WALLT, GOLD, 5000, 0, 400},
        {"arch/Wall4.webp", 4, 1, WALLT, GOLD, 10000, 0, 800}}},
    {GOLD_STORAGE, {
        {"arch/Gold_Storage1.webp", 1, 3, RESOURCE, ELIXIR, 300, 10, 150, 1500},
        {"arch/Gold_Storage2.webp", 2, 3, RESOURCE, ELIXIR, 750, 120, 300, 3000},
        {"arch/Gold_Storage3.webp", 3, 3, RESOURCE, ELIXIR, 1500, 300, 450, 6000},
        {"arch/Gold_Storage4.webp", 4, 3, RESOURCE, ELIXIR, 3000, 900, 600, 12000}}},
    {ELIXIR_STORAGE, {
        {"arch/Elixir_Storage1.webp", 1, 3, RESOURCE, GOLD, 300, 10, 150, 1500},
        {"arch/Elixir_Storage2.webp", 2, 3, RESOURCE, GOLD, 750, 120, 300, 3000},
        {"arch/Elixir_Storage3.webp", 3, 3, RESOURCE, GOLD, 1500, 300, 450, 6000},
        {"arch/Elixir_Storage4.webp", 4, 3, RESOURCE, GOLD, 3000, 900, 600, 12000}}},
    {GOLD_MINE, {
        {"arch/Gold_Mine1.webp", 1, 3, RESOURCE, ELIXIR, 150, 5, 75, 1000, 200},
        {"arch/Gold_Mine2.webp", 2, 3, RESOURCE, ELIXIR, 300, 15, 150, 2000, 400},
        {"arch/Gold_Mine3.webp", 3, 3, RESOURCE, ELIXIR, 700, 60, 300, 3000, 600},
        {"arch/Gold_Mine4.webp", 4, 3, RESOURCE, ELIXIR, 1400, 120, 400, 5000, 800}}},
    {ELIXIR_COLLECTOR, {
        {"arch/Elixir_Collector1.webp", 1, 3, RESOURCE, GOLD, 150, 5, 75, 1000, 200},
        {"arch/Elixir_Collector2.webp", 2, 3, RESOURCE, GOLD, 300, 15, 150, 2000, 400},
        {"arch/Elixir_Collector3.webp", 3, 3, RESOURCE, GOLD, 700, 60, 300, 3000, 600},
        {"arch/Elixir_Collector4.webp", 4, 3, RESOURCE, GOLD, 1400, 120, 400, 5000, 800}}},
    {BARRACKS, {
        {"arch/Barracks1.webp", 1, 3, OTHER, ELIXIR, 100, 10, 100},
        {"arch/Barracks2.webp", 2, 3, OTHER, ELIXIR, 500, 15, 200},
        {"arch/Barracks3.webp", 3, 3, OTHER, ELIXIR, 2500, 120, 250},
        {"arch/Barracks4.webp", 4, 3, OTHER, ELIXIR, 5000, 1800, 300}}},
    {ARMY_CAMP, {
        {"arch/Army_Camp1.webp", 1, 4, OTHER, ELIXIR, 200, 60, 100},
        {"arch/Army_Camp2.webp", 2, 4, OTHER, ELIXIR, 2000, 300, 150},
        {"arch/Army_Camp3.webp", 3, 4, OTHER, ELIXIR, 10000, 1800, 200},
        {"arch/Army_Camp4.webp", 4, 4, OTHER, ELIXIR, 100000, 7200, 250}}},
    {CANNON, {
        {"arch/Cannon1.webp", 1, 3, DEFENSE, GOLD, 250, 5, 300, 0, 0, 5, 800, 90},
        {"arch/Cannon2.webp", 2, 3, DEFENSE, GOLD, 1000, 30, 360, 0, 0, 8, 800, 90},
        {"arch/Cannon3.webp", 3, 3, DEFENSE, GOLD, 4000, 120, 420, 0, 0, 10, 800, 90},
        {"arch/Cannon4.webp", 4, 3, DEFENSE, GOLD, 16000, 1200, 500, 0, 0, 14, 800, 90}}},
    {ARCHER_TOWER, {
        {"arch/Archer_Tower1.webp", 1, 3, DEFENSE, GOLD, 1000, 15, 380, 0, 0, 5, 500, 100, 2},
        {"arch/Archer_Tower2.webp", 2, 3, DEFENSE, GOLD, 2000, 120, 420, 0, 0, 7, 500, 100, 2},
        {"arch/Archer_Tower3.webp", 3, 3, DEFENSE, GOLD, 5000, 1200, 460, 0, 0, 9, 500, 100, 2},
        {"arch/Archer_Tower4.webp", 4, 3, DEFENSE, GOLD, 20000, 3600, 500, 0, 0, 12, 500, 100, 2}}}
};

#endif // __ARCH_INFO_H__
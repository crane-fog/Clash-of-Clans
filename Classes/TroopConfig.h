#ifndef __TROOP_CONFIG_H__
#define __TROOP_CONFIG_H__

#include <vector>

#include "ArchInfo.h"

// todo: 发现没有人口占用的常量，临时写了一个，待整合进具体兵种的构造函数
const std::map<unsigned char, unsigned char> kNoHousingSpace = {{Troop::BARBARIAN, 1}, {Troop::ARCHER, 1},
                                                                {Troop::GIANT, 5},     {Troop::WALL_BREAKER, 2},
                                                                {Troop::DRAGON, 20},   {Troop::BALLOON, 5}};

// 同上，这个主要为了解决顺序和it问题
const std::vector<unsigned char> kTroopTypes = {Troop::BARBARIAN,    Troop::ARCHER,  Troop::GIANT,
                                                Troop::WALL_BREAKER, Troop::BALLOON, Troop::DRAGON};

// 兵种图标路径映射
static const std::map<unsigned char, std::string> kIconPaths = {
    {Troop::BARBARIAN, "troop/babarian_icon.png"}, {Troop::ARCHER, "troop/archer_icon.png"},
    {Troop::GIANT, "troop/Giant_icon.png"},        {Troop::WALL_BREAKER, "troop/bomb_icon.png"},
    {Troop::DRAGON, "troop/dragon_icon.png"},      {Troop::BALLOON, "troop/balloon_icon.png"}};

// 玩家兵种配置
class TroopConfig {
private:
    unsigned int capacity_;
    unsigned char unlocked_troop_type_;
    std::map<unsigned char, unsigned int> config_;

    TroopConfig() : capacity_(0), unlocked_troop_type_(0)
    {
        for (auto it : kTroopTypes) config_.emplace(it, 0);
    }
    TroopConfig(const TroopConfig&) = delete;
    TroopConfig& operator=(const TroopConfig&) = delete;
    // 兵种等级
    std::map<unsigned char, int> kTroopLevels = {{Troop::BARBARIAN, 1},    {Troop::ARCHER, 1}, {Troop::GIANT, 1},
                                                 {Troop::WALL_BREAKER, 1}, {Troop::DRAGON, 1}, {Troop::BALLOON, 1}};

public:
    static TroopConfig* getInstance()
    {
        static TroopConfig instance;
        return &instance;
    }

    // 设置训练营等级
    void setBarrackLevel(unsigned char level);

    // 设置人口容量
    void setArmyCampCapacity(unsigned int capacity) { capacity_ = capacity; }

    // 配置兵种数量
    void setTroopCount(unsigned char troop_type, unsigned int count)
    {
        if (troop_type <= unlocked_troop_type_) {
            config_.at(troop_type) = count;
        }
    }

    // 获取人口容量
    unsigned int getArmyCampCapacity() const { return capacity_; }

    // 获取已解锁的兵种进度的 kTroopTypes 索引
    unsigned char getUnlockedTroopIndex() const { return unlocked_troop_type_; }

    // 获取当前已配置的兵种数量
    unsigned int getTroopCount(unsigned char troop_type) const { return config_.at(troop_type); }

    // 获取兵种的等级
    int getTroopLevel(unsigned char troopname) { return kTroopLevels.at(troopname); }
    void upgradeTroopLevel(unsigned char troopname) { kTroopLevels.at(troopname)++; }
};

#endif  // __TROOP_CONFIG_H__
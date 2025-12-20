#ifndef __TROOP_CONFIG_H__
#define __TROOP_CONFIG_H__

#include "ArchInfo.h"
#include <vector>

// 玩家兵种配置
class TroopConfig {
private:
    unsigned int capacity_;
    unsigned char unlocked_troop_type_;
    std::vector<unsigned int> config_;

    TroopConfig() : capacity_(0), unlocked_troop_type_(0), config_(TROOP_TYPE_NUM, 0) {}
    TroopConfig(const TroopConfig&) = delete;
    TroopConfig& operator= (const TroopConfig&) = delete;

public:
    static TroopConfig* getInstance()
    {
        static TroopConfig instance;
        return &instance;
    }
    
    // 设置训练营等级
    void setBarrackLevel(unsigned char level);

    // 设置人口容量
    void setArmyCampCapacity(unsigned int capacity)
    {
        capacity_ = capacity;
    }

    // 配置兵种数量
    void setTroopCount(unsigned char troop_type, unsigned int count)
    {
        if (troop_type <= unlocked_troop_type_) {
            config_[troop_type] = count;
        }
    }

    // 获取人口容量
    unsigned int getArmyCampCapacity() const
    {
        return capacity_;
    }

    // 获取已解锁的兵种进度索引
    unsigned char getUnlockedTroops() const
    {
        return unlocked_troop_type_;
    }

    // 获取当前已配置的兵种数量
    unsigned int getTroopCount(unsigned char troop_type) const
    {
        return config_[troop_type];
    }
};

#endif // __TROOP_CONFIG_H__
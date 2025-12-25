#ifndef __REPLAY_ATTACK_H__
#define __REPLAY_ATTACK_H__
#include <cocos2d.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "EnemyVillageScene.h"
struct Unit {
    int type_;                // 兵种类型
    cocos2d::Vec2 position_;  // 位置

    Unit(const int& type, const cocos2d::Vec2& position) : type_(type), position_(position) {}
};

class UnitManager {
private:
    static UnitManager* instance;  // 单例实例

    std::unordered_map<int, std::vector<cocos2d::Vec2>> units_by_type_;  // 按兵种类型分类

    // 私有化构造函数，确保外部无法直接创建实例
    UnitManager() {}

public:
    int soldiers_sum_ = 0;
    std::vector<Unit> units_;  // 存储所有兵种的顺序和位置
    // 获取单例实例
    static UnitManager* getInstance()
    {
        if (instance == nullptr) {
            instance = new UnitManager();  // 创建唯一实例
        }
        return instance;
    }

    // 删除单例实例（通常用于清理工作）
    static void destroyInstance()
    {
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }

    // 添加兵种
    void addUnit(const int& type, const cocos2d::Vec2& position)
    {
        // 将单位添加到顺序列表
        units_.push_back(Unit(type, position));
        CCLOG("成功记录士兵种类和位置:", std::to_string(type), std::to_string(position.x));

        soldiers_sum_++;
    }

    // 清除所有记录
    void clearUnits()
    {
        units_.clear();
        units_by_type_.clear();
    }

    // 自动放置士兵
    void autoPutUnits() const
    {
        for (const auto& unit : units_) {
            if (unit.type_ == 0) {
                // EnemyVillage::spawnBarbarian(unit.position);
            }
        }
    }
};

// 初始化静态成员变量
UnitManager* UnitManager::instance = nullptr;
#endif  // __REPLAY_ATTACK_H__
#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__

// 资源管理器单例类，用于管理游戏中的各种资源（金币、圣水、宝石）
class ResourceManager {
    // unsigned long long 类型的别名
    typedef unsigned long long ULL;

private:
    // 私有构造函数，实现单例模式
    ResourceManager() = default;

    // 禁用拷贝构造函数
    ResourceManager(const ResourceManager&) = delete;

    // 禁用赋值运算符
    ResourceManager& operator=(const ResourceManager&) = delete;

    // 当前拥有的金币数量
    ULL my_gold_;

    // 当前拥有的圣水数量
    ULL my_elixir_;

    // 当前拥有的宝石数量，初始值为10
    ULL my_jewel_ = 10;

    // 金币存储上限
    ULL max_gold_;

    // 圣水存储上限
    ULL max_elixir_;

    // 宝石存储上限，宝石通常没有上限（0表示无上限）
    ULL max_jewel_ = 0;

public:
    // 获取资源管理器单例实例
    static ResourceManager* getInstance();

    // 设置当前金币数量
    void setGold(ULL gold);

    // 设置金币存储上限
    void setMaxGold(ULL max_gold);

    // 获取当前金币数量
    ULL getGold() const { return my_gold_; }

    // 获取金币存储上限
    ULL getMaxGold() const { return max_gold_; }

    // 设置当前圣水数量
    void setElixir(ULL Elixir);

    // 设置圣水存储上限
    void setMaxElixir(ULL max_elixir);

    // 获取当前圣水数量
    ULL getElixir() const { return my_elixir_; }

    // 获取圣水存储上限
    ULL getMaxElixir() const { return max_elixir_; }

    // 设置当前宝石数量
    void setJewel(ULL Jewel);

    // 获取当前宝石数量
    ULL getJewel() const { return my_jewel_; }

    // 获取宝石存储上限
    ULL getMaxJewel() const { return max_jewel_; }
};
#endif  // __GAME_MANAGER_H__
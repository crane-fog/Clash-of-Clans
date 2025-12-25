#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__

class ResourceManager {
    typedef unsigned long long ULL;

private:
    ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    ULL my_gold_;
    ULL my_elixir_;
    ULL my_jewel_ = 10;
    ULL max_gold_;
    ULL max_elixir_;
    ULL max_jewel_ = 0;

public:
    static ResourceManager* getInstance();

    void setGold(ULL gold);
    void setMaxGold(ULL max_gold);

    ULL getGold() const { return my_gold_; }
    ULL getMaxGold() const { return max_gold_; }

    void setElixir(ULL Elixir);
    void setMaxElixir(ULL max_elixir);

    ULL getElixir() const { return my_elixir_; }
    ULL getMaxElixir() const { return max_elixir_; }

    void setJewel(ULL Jewel);

    ULL getJewel() const { return my_jewel_; }
    ULL getMaxJewel() const { return max_jewel_; }
};

#endif  // __GAME_MANAGER_H__
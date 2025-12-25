#include "TroopConfig.h"

TroopConfig::TroopConfig() : capacity_(0), unlocked_troop_type_(0)
{
    for (auto it : kTroopTypes) config_.emplace(it, 0);
}

TroopConfig* TroopConfig::getInstance()
{
    static TroopConfig instance;
    return &instance;
}

void TroopConfig::setTroopCount(unsigned char troop_type, unsigned int count)
{
    if (troop_type <= unlocked_troop_type_) {
        config_.at(troop_type) = count;
    }
}

void TroopConfig::setBarrackLevel(unsigned char level)
{
    unlocked_troop_type_ = 0;
    for (auto troop : kTroopTypes) {
        if (kBarracksTroopUnlock.at(troop) <= level) {
            unlocked_troop_type_++;
        }
    }
    unlocked_troop_type_--;  // 转为索引
}
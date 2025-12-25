#include "TroopConfig.h"

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
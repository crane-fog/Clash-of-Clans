#include "TroopConfig.h"

void TroopConfig::setBarrackLevel(unsigned char level)
{
    unlocked_troop_type_ = 0;
    for (const auto& troop : kBarracksTroopUnlock) {
        if (troop.first <= level) {
            unlocked_troop_type_++;
        }
    }
}
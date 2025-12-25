#include "GameManager.h"

#include "cocos2d.h"

void GameManager::setGold(ULL gold)
{
    this->my_gold_ = gold;
    // 发布金币更新事件
    cocos2d::EventCustom event("update_gold_event");
    event.setUserData(&my_gold_);
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GameManager::setMaxGold(ULL max_gold)
{
    max_gold_ = max_gold;
    cocos2d::EventCustom event("update_max_gold_event");
    event.setUserData(&max_gold_);
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GameManager::setElixir(ULL Elixir)
{
    this->my_elixir_ = Elixir;
    // 发布圣水更新事件
    cocos2d::EventCustom event("update_elixir_event");
    event.setUserData(&my_elixir_);
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GameManager::setMaxElixir(ULL max_elixir)
{
    max_elixir_ = max_elixir;
    cocos2d::EventCustom event("update_max_elixir_event");
    event.setUserData(&max_elixir_);
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GameManager::setJewel(ULL Jewel)
{
    this->my_jewel_ = Jewel;
    // 发布宝石更新事件
    cocos2d::EventCustom event("update_jewel_event");
    event.setUserData(&my_jewel_);
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GameManager::setMaxJewel(ULL max_jewel)
{
    max_jewel_ = max_jewel;
    cocos2d::EventCustom event("update_max_jewel_event");
    event.setUserData(&max_jewel_);
    cocos2d::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}
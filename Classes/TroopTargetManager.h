class TroopTargetManager {
    protected:
        std::vector<ITroopTarget*> targets_;
    public:
        static TroopTargetManager* getInstance();
        
        // 建筑把自己注册为可攻击目标
        void registerTroopTarget(ITroopTarget* target);
        
        // 建筑把自己从目标列表移除
        void unregisterTroopTarget(ITroopTarget* target);
        
        // 士兵查找攻击目标
        ITroopTarget* getNearestTroopTarget(const cocos2d::Vec2& position, 
                                       unsigned char preferred_target_ = 255);
    };
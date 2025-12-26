#ifndef __TROOPTARGETMANAGER_H__
#define __TROOPTARGETMANAGER_H__
#include <unordered_map>
#include <vector>

#include "cocos2d.h"
#include "ITroopTarget.h"
#include "Troop.h"

class TroopTargetManager {
private:
    TroopTargetManager() = default;
    TroopTargetManager(const TroopTargetManager&) = delete;
    TroopTargetManager& operator=(const TroopTargetManager&) = delete;

    struct Compare {
        bool operator()(const std::tuple<cocos2d::Vec2, float>& a, const std::tuple<cocos2d::Vec2, float>& b) const
        {
            return std::get<1>(a) > std::get<1>(b);  // 小顶堆示例
        }
    };

    using DistancePQ =
        std::priority_queue<std::tuple<cocos2d::Vec2, float>, std::vector<std::tuple<cocos2d::Vec2, float>>, Compare>;

    // 初始化优先队列距离场
    void pqInit(DistancePQ& pq, std::vector<std::vector<float>>& distance_field, ITroopTarget* target);

    // 为单个目标计算距离场
    void computeDistanceField(ITroopTarget* target);

    // 将网格坐标转换为距离场数组索引
    int posToIndex(const cocos2d::Vec2& pos) const;

    // 检查坐标是否有效
    bool isValidPosition(const cocos2d::Vec2& pos) const;

    int living_arch_ = 0;
    int dead_arch_ = 0;

protected:
    /*  OTHER = 0, // 其它
        RESOURCE = 1, // 资源
        DEFENSE = 2, // 防御
        WALLT = 3, // 城墙 */
    std::vector<std::vector<ITroopTarget*>> targets_;

    std::vector<std::vector<ITroopTarget*>> target_map_;  // 按坐标位置存储建筑目标

    // 距离场数据：为每个目标建筑存储距离场图
    // 建筑到各个格子的距离（44x44的float数组） 墙没有距离场数据!
    std::unordered_map<ITroopTarget*, std::vector<std::vector<float>>> distance_fields_;

    // 墙代价地图：预计算的墙障碍地图
    std::vector<std::vector<float>> wall_cost_map_;

    // 地图尺寸常量
    static const int kMapWidth = 44;  // 44格，对应游戏坐标
    static const int kMapHeight = 44;

    // 移动代价常量
    static constexpr float kNormalCost = 1.0f;           // 普通地面移动代价
    static constexpr float kDiagonalCost = 1.414f;       // 斜向移动代价
    static constexpr float kWallCost = 30.0f;            // 墙的移动代价（可通行但代价很高）
    static constexpr float kBuildingBlockCost = 999.0f;  // 建筑阻挡代价（基本不可通行）
    static constexpr float kUnreachableCost = 1000.0f;   // 完全不可达的代价

public:
    static TroopTargetManager* getInstance();

    // 清除所有数据，在离开场景时调用
    void clear();

    // 建筑把自己注册为可攻击目标
    void registerTroopTarget(ITroopTarget* target);

    // 建筑把自己从目标列表移除
    void unregisterTroopTarget(ITroopTarget* target);

    // 当建筑被摧毁时清理相关的距离场数据
    void onTargetDestroyed(ITroopTarget* target);

    // 士兵查找position附近的攻击目标 返回distance最小距离
    ITroopTarget* getNearestTroopTarget(const cocos2d::Vec2& position, float& min_distance,
                                        bool is_wall_included = false,
                                        Troop::PreferredTarget preferred_target = Troop::NONE);

    // 按坐标位置查找对应的建筑目标
    ITroopTarget* getTroopTargetByCellPos(const cocos2d::Vec2& position);

    // 判断指定坐标格子是否为墙
    bool isCellWall(const cocos2d::Vec2& position);

    // 预计算墙代价地图（只计算一次）
    void precomputeWallCostMap();

    // 预计算所有目标的距离场
    void precomputeDistanceFields();

    // 获取指定目标的距离场
    const std::vector<std::vector<float>>& getDistanceField(ITroopTarget* target);

    // 根据距离场获取下一步移动方向 (返回相对于当前位置的偏移)
    cocos2d::Vec2 getNextMoveDirection(const cocos2d::Vec2& current_pos, ITroopTarget* target, float attack_range,
                                       bool is_air = false);

    // 检查当前位置是否在攻击范围内
    bool isInAttackRange(const cocos2d::Vec2& position, ITroopTarget* target, float attack_range) const;

    // 获取与指定圆形区域接触的所有建筑
    std::vector<ITroopTarget*> getTargetsInRange(const cocos2d::Vec2& position, float radius);

    // 为炸弹人查找最优的墙壁目标（周围至少有2个墙壁）
    ITroopTarget* getOptimalWallTarget(const cocos2d::Vec2& position);

    int getlivingsum() const { return living_arch_; }
    int getDeadsum() const { return dead_arch_; }
    void setlivingsum(int newArch) { this->living_arch_ = newArch; }
    void setdeadsum(int newArch) { this->dead_arch_ = newArch; }
};

#endif  // __TROOPTARGETMANAGER_H__
#ifndef __TROOPTARGETMANAGER_H__
#define __TROOPTARGETMANAGER_H__
#include "cocos2d.h"
#include "ITroopTarget.h"
#include "Troop.h"
#include <vector>
#include <unordered_map>

class TroopTargetManager {
    protected:
        /*  OTHER = 0, // 其它
            RESOURCE = 1, // 资源
            DEFENSE = 2, // 防御
            WALLT = 3, // 城墙 */
        std::vector<std::vector<ITroopTarget*>> targets_;

        // 距离场数据：为每个目标建筑和每种兵种类型存储距离场图
        // 第一维：兵种类型(0-5)，第二维：建筑，第三维：距离场数据(44x44的float数组) 墙应该没有距离场数据!
        std::vector<std::unordered_map<ITroopTarget*, std::vector<std::vector<float>>>> distance_fields_;

        // 墙代价地图：预计算的墙障碍地图
        std::vector<std::vector<float>> wall_cost_map_;

        // 地图尺寸常量
        static const int MAP_WIDTH = 44;   // 44格，对应游戏坐标
        static const int MAP_HEIGHT = 44;

        // 移动代价常量
        static constexpr float NORMAL_COST = 1.0f;           // 普通地面移动代价
        static constexpr float DIAGONAL_COST = 1.414f;       // 斜向移动代价
        static constexpr float WALL_COST = 10.0f;            // 墙的移动代价（可通行但代价很高）
        static constexpr float BUILDING_BLOCK_COST = 999.0f; // 建筑阻挡代价（基本不可通行）
        static constexpr float UNREACHABLE_COST = 1000.0f;   // 完全不可达的代价

    public:
        static TroopTargetManager* getInstance();

		//清除所有数据，在离开场景时调用
        void clear();

        // 建筑把自己注册为可攻击目标
        void registerTroopTarget(ITroopTarget* target);

        // 建筑把自己从目标列表移除
        void unregisterTroopTarget(ITroopTarget* target);

        // 当建筑被摧毁时清理相关的距离场数据
        void onTargetDestroyed(ITroopTarget* target);

        // 士兵查找position附近的攻击目标 返回distance最小距离
        ITroopTarget* getNearestTroopTarget(const cocos2d::Vec2& position, float& min_distance, bool is_wall_included=false,
            Troop::PreferredTarget preferred_target = Troop::NONE);

        // 预计算墙代价地图（只计算一次）
        void precomputeWallCostMap();

        // 预计算所有目标的距离场
        void precomputeDistanceFields(std::vector<Troop*>& troop_vec);

        // 获取指定目标和攻击类型的距离场
        const std::vector<std::vector<float>>& getDistanceField(ITroopTarget* target, Troop* troop) const;

        // 根据距离场获取下一步移动方向 (返回相对于当前位置的偏移)
        cocos2d::Vec2 getNextMoveDirection(const cocos2d::Vec2& current_pos, ITroopTarget* target, Troop* troop);

        // 检查当前位置是否在攻击范围内
        bool isInAttackRange(const cocos2d::Vec2& position, ITroopTarget* target, Troop* troop) const;

    private:
        struct Compare {
            bool operator()(const std::tuple<cocos2d::Vec2, float>& a,
                const std::tuple<cocos2d::Vec2, float>& b) const {
                return std::get<1>(a) > std::get<1>(b); // 小顶堆示例
            }
        };
        using DistancePQ = std::priority_queue<std::tuple<cocos2d::Vec2, float>, std::vector<std::tuple<cocos2d::Vec2, float>>,Compare>;

		// 初始化优先队列距离场
		void pqInit(DistancePQ& pq, std::vector<std::vector<float>>& distance_field, ITroopTarget* target, Troop* troop);
        // 检查兵种是否需要考虑墙障碍
        bool doesTroopConsiderWalls(Troop::TroopType troop_type) const {
            return troop_type == Troop::BARBARIAN ||
                   troop_type == Troop::ARCHER ||
                   troop_type == Troop::GIANT;
        }

        // 为单个目标计算距离场
        void computeDistanceField(ITroopTarget* target, Troop* troop);

        // 将网格坐标转换为距离场数组索引
        int posToIndex(const cocos2d::Vec2& pos) const { return static_cast<int>(pos.y) * MAP_WIDTH + static_cast<int>(pos.x); }

        // 检查坐标是否有效
        bool isValidPosition(const cocos2d::Vec2& pos) const {
            return pos.x >= 0 && pos.x < MAP_WIDTH && pos.y >= 0 && pos.y < MAP_HEIGHT;
        }
    };

#endif // __TROOPTARGETMANAGER_H__
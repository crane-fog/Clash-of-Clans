#include "TroopTargetManager.h"
#include <algorithm>
#include <limits>
#include <queue>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "CalculateHelper.h"


TroopTargetManager* TroopTargetManager::getInstance() {
    static TroopTargetManager instance;
    // 初始化4个类型容器 (OTHER, RESOURCE, DEFENSE, WALLT)
    if (instance.targets_.empty()) {
        instance.targets_.resize(4);
    }
    if (instance.target_map_.empty()) {
        instance.target_map_= std::vector<std::vector<ITroopTarget*>>(MAP_WIDTH, std::vector<ITroopTarget*>(MAP_HEIGHT));
    }
    if(instance.livingArch<instance.deadArch)   instance.deadArch = 0;

    return &instance;
}

void TroopTargetManager::clear() {
	targets_.clear();
	distance_fields_.clear();
    wall_cost_map_.clear();
	target_map_.clear();
}

void TroopTargetManager::registerTroopTarget(ITroopTarget* target) {
    if (!target) return;

    // 获取目标类型并转换为容器索引
    Troop::PreferredTarget target_type = static_cast<Troop::PreferredTarget>(target->getTargetType());
    size_t container_index = static_cast<size_t>(target_type);

    // 确保容器索引有效
    if (container_index >= targets_.size()) {
        container_index = 0; // 默认放入OTHER容器
    }

    // 检查是否已注册
    auto& container = targets_[container_index];
    if (std::find(container.begin(), container.end(), target) == container.end()) {
        container.push_back(target);

    }

    // 获取建筑位置和大小
    float building_size;
    cocos2d::Vec2 building_pos = target->getCellPosition(building_size);

    // 计算建筑占据的格子范围
    int left = static_cast<int>(building_pos.x - building_size / 2.0f);
    int right = static_cast<int>(building_pos.x + building_size / 2.0f);
    int bottom = static_cast<int>(building_pos.y - building_size / 2.0f);
    int top = static_cast<int>(building_pos.y + building_size / 2.0f);

    // 将建筑指针填入target_map_中覆盖的所有格子
    for (int y = std::max(0, bottom); y < std::min(MAP_HEIGHT, top); ++y) {
        for (int x = std::max(0, left); x < std::min(MAP_WIDTH, right); ++x) {
            target_map_[x][y] = target;
        }
    }
}

void TroopTargetManager::unregisterTroopTarget(ITroopTarget* target) {
    if (!target) return;

    // 获取目标类型并转换为容器索引
    Troop::PreferredTarget target_type = static_cast<Troop::PreferredTarget>(target->getTargetType());
    size_t container_index = static_cast<size_t>(target_type);

    // 确保容器索引有效
    if (container_index >= targets_.size()) {
        container_index = 0; // 默认在OTHER容器中查找
    }

    auto& container = targets_[container_index];
    auto it = std::find(container.begin(), container.end(), target);
    if (it != container.end()) {
        //更新被摧毁的建筑数量
        deadArch++;
        container.erase(it);

        // 清理target_map_中对应的格子
        float building_size;
        cocos2d::Vec2 building_pos = target->getCellPosition(building_size);

        // 计算建筑占据的格子范围
        int left = static_cast<int>(building_pos.x - building_size / 2.0f);
        int right = static_cast<int>(building_pos.x + building_size / 2.0f);
        int bottom = static_cast<int>(building_pos.y - building_size / 2.0f);
        int top = static_cast<int>(building_pos.y + building_size / 2.0f);

        // 将target_map_中对应的格子设为nullptr
        for (int y = std::max(0, bottom); y < std::min(MAP_HEIGHT, top); ++y) {
            for (int x = std::max(0, left); x < std::min(MAP_WIDTH, right); ++x) {
                target_map_[x][y] = nullptr;
            }
        }

        // 当建筑从目标列表移除时，清理其距离场数据
        onTargetDestroyed(target);
    }
}

ITroopTarget* TroopTargetManager::getNearestTroopTarget(const cocos2d::Vec2& position, float& min_distance, bool is_wall_included,
    Troop::PreferredTarget preferred_target) {

    ITroopTarget* nearest_target = nullptr;
    min_distance = std::numeric_limits<float>::max();

    // 第一遍：查找符合偏好类型的目标
    if (preferred_target != Troop::NONE) {
        size_t container_index = static_cast<size_t>(preferred_target);
        if (container_index < targets_.size()) {
            const auto& container = targets_[container_index];
            for (ITroopTarget* target : container) {
                if (target->getTargetType() == Troop::WALLT && !is_wall_included) continue;
                if (!target->isAlive()) continue;

                float size;
                cocos2d::Vec2 target_pos = target->getCellPosition(size);
                float distance = CalculateHelper::calculateDistanceToSquare(position, target_pos, size);

                if (distance < min_distance) {
                    min_distance = distance;
                    nearest_target = target;
                }
            }
        }
    }

    // 如果找到了符合偏好类型的目标，返回它
    if (nearest_target != nullptr) {
        return nearest_target;
    }

    // 第二遍：查找所有目标中的最近者（包括不符合偏好类型的）
    min_distance = std::numeric_limits<float>::max();
    for (size_t i = 0; i < targets_.size(); ++i) {
        // 跳过城墙容器（如果不包含城墙的话）
        if (i == static_cast<size_t>(Troop::WALLT) && !is_wall_included) continue;

        const auto& container = targets_[i];
        for (ITroopTarget* target : container) {
            if (!target->isAlive()) continue;

            float size;
            cocos2d::Vec2 target_pos = target->getCellPosition(size);
            float distance = CalculateHelper::calculateDistanceToSquare(position, target_pos, size);

            if (distance < min_distance) {
                min_distance = distance;
                nearest_target = target;
            }
        }
    }

    return nearest_target;
}


/**
 * @brief 获取与指定圆形区域接触的所有建筑
 * @param position 圆心的坐标位置
 * @param radius 圆的半径
 * @return 所有与圆接触的建筑列表
 */
std::vector<ITroopTarget*>& TroopTargetManager::getTargetsInRange(const cocos2d::Vec2& position, float radius) {
    std::vector<ITroopTarget*> targets_in_range;

    // 遍历所有类型的建筑容器
    for (size_t i = 0; i < targets_.size(); ++i) {
        const auto& container = targets_[i];
        for (ITroopTarget* target : container) {
            if (!target->isAlive()) continue;

            // 获取建筑位置和大小
            float size;
            cocos2d::Vec2 target_pos = target->getCellPosition(size);

            // 计算建筑与圆心的距离
            float distance = CalculateHelper::calculateDistanceToSquare(position, target_pos, size);

            // 如果距离小于等于半径，则该建筑与圆接触
            if (distance <= radius) {
                targets_in_range.push_back(target);
            }
        }
    }

    return targets_in_range;
}

/**
 * @brief 按坐标位置查找对应的建筑目标
 * @param position 要查找的坐标位置（浮点数）
 * @return 包含该坐标的建筑指针，如果没有找到则返回nullptr
 */
ITroopTarget* TroopTargetManager::getTroopTargetByCellPos(const cocos2d::Vec2& position) {
    // 将坐标转换为网格索引
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);

    // 检查坐标是否在有效范围内
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        ITroopTarget* target = target_map_[x][y];
        // 检查建筑是否还活着
        if (target && target->isAlive()) {
            return target;
        }
    }

    return nullptr; // 没有找到包含该坐标的建筑
}

/**
 * @brief 判断指定坐标格子是否为墙
 * @param position 要检查的坐标位置
 * @return 如果该坐标处是活着的墙则返回true，否则返回false
 */
bool TroopTargetManager::isCellWall(const cocos2d::Vec2& position) {
    // 将坐标转换为网格索引
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);

    // 检查坐标是否在有效范围内
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        ITroopTarget* target = target_map_[x][y];
        // 检查是否存在墙且墙还活着
        if (target && target->isAlive() && target->getTargetType() == Troop::WALLT) {
            return true;
        }
    }

    return false; // 不是墙
}

void TroopTargetManager::onTargetDestroyed(ITroopTarget* target) {
    if (!target) return;

    // 获取建筑位置和大小
    float target_size;
    cocos2d::Vec2 target_pos = target->getCellPosition(target_size);

    // 计算建筑占据的格子范围（墙也当作1x1的建筑处理）
    int left = static_cast<int>(target_pos.x - target_size / 2.0f);
    int right = static_cast<int>(target_pos.x + target_size / 2.0f);
    int bottom = static_cast<int>(target_pos.y - target_size / 2.0f);
    int top = static_cast<int>(target_pos.y + target_size / 2.0f);

    // 将wall_cost_map_中对应位置设为普通地面
    for (int y = std::max(0, bottom); y < std::min(MAP_HEIGHT, top); ++y) {
        for (int x = std::max(0, left); x < std::min(MAP_WIDTH, right); ++x) {
            wall_cost_map_[x][y] = 0.0f; // 设为普通地面
        }
    }

    // 如果不是墙，清除对应的距离场数据
    if (target->getTargetType() != Troop::WALLT) {
        distance_fields_.erase(target);
        // 其他建筑的摧毁会影响现有路径
    } 
    precomputeDistanceFields();
}

void TroopTargetManager::precomputeWallCostMap() {
    // 初始化为0.0（普通地面无额外代价）
    wall_cost_map_ = std::vector<std::vector<float>>(MAP_WIDTH, std::vector<float>(MAP_HEIGHT, 0.0f));

    // 直接遍历target_map_的所有格子，根据建筑类型设置不同的代价
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            ITroopTarget* target = target_map_[x][y];
            if (target && target->isAlive()) {
                if (target->getTargetType() == Troop::WALLT) {
                    // 墙：可通行但代价很高
                    wall_cost_map_[x][y] = WALL_COST; // 10.0f
                } else {
                    // 其他建筑：基本不可通行
                    wall_cost_map_[x][y] = BUILDING_BLOCK_COST; // 999.0f
                }
            }
            // 如果没有建筑，保持为0.0f（普通地面）
        }
    }

    //// 输出调试信息到文件
    //std::ofstream debug_file("troop_target_debug.txt");
    //if (debug_file.is_open()) {
    //    debug_file << "=== WALL_COST_MAP_ (44x44) ===\n";
    //    for (int y = 0; y < MAP_HEIGHT; ++y) {
    //        for (int x = 0; x < MAP_WIDTH; ++x) {
    //            debug_file << std::fixed << std::setprecision(1) << std::setw(6) << wall_cost_map_[x][y];
    //        }
    //        debug_file << "\n";
    //    }
    //    debug_file << "\n=== TARGET_MAP_ (44x44) ===\n";
    //    for (int y = 0; y < MAP_HEIGHT; ++y) {
    //        for (int x = 0; x < MAP_WIDTH; ++x) {
    //            if (target_map_[x][y]) {
    //                debug_file << std::setw(16) << std::hex << std::showbase << reinterpret_cast<uintptr_t>(target_map_[x][y]) << std::dec;
    //            } else {
    //                debug_file << std::setw(16) << "nullptr";
    //            }
    //        }
    //        debug_file << "\n";
    //    }
    //    debug_file.close();
    //}
}

void TroopTargetManager::precomputeDistanceFields() {
    if (wall_cost_map_.empty()) {
        // 首先预计算墙代价地图
        precomputeWallCostMap();
    }

    // 清除所有距离场数据
    distance_fields_.clear();

    // 为所有非墙建筑计算距离场
    for (size_t target_type_idx = 0; target_type_idx < targets_.size(); ++target_type_idx) {
        if (target_type_idx == static_cast<size_t>(Troop::WALLT)) continue; // 跳过墙

        for (ITroopTarget* target : targets_[target_type_idx]) {
            if (target->isAlive()) {
                computeDistanceField(target);
            }
        }
    }
}

void TroopTargetManager::pqInit(DistancePQ& pq, std::vector<std::vector<float>>&distance_field,ITroopTarget* target) {
    // 获取建筑大小作为默认攻击范围
    float target_size;
    cocos2d::Vec2 target_pos = target->getCellPosition(target_size);
    // 将目标建筑周围在攻击范围内的格子设为0
    int target_left = static_cast<int>(target_pos.x - target_size / 2.0f);
    int target_right = static_cast<int>(target_pos.x + target_size / 2.0f);
    int target_bottom = static_cast<int>(target_pos.y - target_size / 2.0f);
    int target_top = static_cast<int>(target_pos.y + target_size / 2.0f);

    // 1. 先把建筑范围内的格子强制置为0
    for (int y = std::max(0, target_bottom); y < std::min(MAP_HEIGHT, target_top); ++y) {
        for (int x = std::max(0, target_left); x < std::min(MAP_WIDTH, target_right); ++x) {
            cocos2d::Vec2 grid_pos(static_cast<float>(x), static_cast<float>(y));
            distance_field[x][y] = 0.0f;
            pq.push(std::make_tuple(grid_pos, 0.0f));
        }
    }
}

void TroopTargetManager::computeDistanceField(ITroopTarget* target) {
    if (!target||target->getTargetType()==Troop::WALLT) return; // 墙没有距离场数据

    // 初始化距离场为无限大（所有位置都不可达）
    std::vector<std::vector<float>> distance_field(MAP_WIDTH,
        std::vector<float>(MAP_HEIGHT, std::numeric_limits<float>::max()));

    // 使用Dijkstra算法从目标建筑向外扩散
    DistancePQ pq;

	/////////////// 初始化 ///////////////
	pqInit(pq, distance_field, target);
    
    // 8个方向的偏移
    const std::vector<cocos2d::Vec2> directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0},  // 上下左右
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}  // 斜向
    };

    // Dijkstra算法主循环
    while (!pq.empty()) {
        auto current = pq.top();
        pq.pop();

        cocos2d::Vec2 current_pos = std::get<0>(current);
        float current_distance = std::get<1>(current);

        int x = static_cast<int>(current_pos.x);
        int y = static_cast<int>(current_pos.y);

        // 如果已经找到更小的距离，跳过
        if (current_distance > distance_field[x][y]) continue;

        // 检查8个方向
        for (const auto& dir : directions) {
            cocos2d::Vec2 neighbor = current_pos + dir;
            if (!isValidPosition(neighbor)) continue;

            int nx = static_cast<int>(neighbor.x);
            int ny = static_cast<int>(neighbor.y);

            // 检查邻居格子是否可通行
            // 距离场计算中，假设所有单位都可以穿过墙，但不能穿过其他建筑
            bool is_neighbor_passable = wall_cost_map_[nx][ny] < BUILDING_BLOCK_COST;

            if (!is_neighbor_passable) continue;

            // 计算移动到邻居格子的总代价
            float base_move_cost = (dir.x != 0 && dir.y != 0) ? DIAGONAL_COST : NORMAL_COST; // 基础移动代价
            float terrain_extra_cost = wall_cost_map_[nx][ny]; // 从wall_cost_map读取额外地形代价
            float total_move_cost = base_move_cost + terrain_extra_cost; // 总移动代价
            float new_path_cost = current_distance + total_move_cost; // 新的路径代价

            // 如果找到更短路径，更新
            if (new_path_cost < distance_field[nx][ny]) {
                distance_field[nx][ny] = new_path_cost;
                pq.push(std::make_tuple(neighbor, new_path_cost));
            }
        }
    }

    // 保存距离场
    distance_fields_[target] = std::move(distance_field);
}

const std::vector<std::vector<float>>& TroopTargetManager::getDistanceField(ITroopTarget* target) const {
    static const std::vector<std::vector<float>> empty_field;

    auto it = distance_fields_.find(target);
    return (it != distance_fields_.end()) ? it->second : empty_field;
}

cocos2d::Vec2 TroopTargetManager::getNextMoveDirection(const cocos2d::Vec2& current_pos, ITroopTarget* target,float attack_range, bool is_air) {
    // 获取当前位置的距离值
    if (!isValidPosition(current_pos)) return cocos2d::Vec2::ZERO;
    
    // 检查当前位置是否在攻击范围内
    if (isInAttackRange(current_pos, target, attack_range)) {
        return cocos2d::Vec2::ZERO; // 不需要移动，已经在攻击范围内
    }
    float size;
    if (target->getTargetType() == Troop::WALLT|| is_air) {
        return (target->getCellPosition(size) - current_pos);// 空中单位和目标为墙时都视为无需绕路
    }

    
    const auto& distance_field = getDistanceField(target);
    if (distance_field.empty()) return cocos2d::Vec2::ZERO;

    
    //TODO: 炸弹人单独的逻辑
    int x = static_cast<int>(current_pos.x);
    int y = static_cast<int>(current_pos.y);
    float current_distance = distance_field[x][y];

    // 如果当前位置是墙或其他不可达区域，返回零向量
    if (wall_cost_map_[x][y] >= WALL_COST) return cocos2d::Vec2::ZERO;

    // 查找周围8个方向中距离值最小的方向
    cocos2d::Vec2 best_direction = cocos2d::Vec2::ZERO;
    float min_distance = current_distance;

    const std::vector<cocos2d::Vec2> directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0},  // 上下左右
        //{1, 1}, {1, -1}, {-1, 1}, {-1, -1}  // 斜向
    };

    for (const auto& dir : directions) {
        cocos2d::Vec2 neighbor = current_pos + dir;
        if (!isValidPosition(neighbor)) continue;

        int nx = static_cast<int>(neighbor.x);
        int ny = static_cast<int>(neighbor.y);
        float neighbor_distance = distance_field[nx][ny];

        // 跳过不可达区域
        if (neighbor_distance >= BUILDING_BLOCK_COST) continue;

        if (neighbor_distance < min_distance) {
            min_distance = neighbor_distance;
            best_direction = dir;
        }
    }

    return best_direction;
}

bool TroopTargetManager::isInAttackRange(const cocos2d::Vec2& position, ITroopTarget* target, float attack_range) const {
    /*const auto& distance_field = getDistanceField(target);
    if (distance_field.empty()) return false;*/

    if (!isValidPosition(position)) return false;
    float size;
    cocos2d::Vec2 center = target->getCellPosition(size);
    /*int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);
    float distance = distance_field[x][y];*/
    // TODO:暂时就这样
	if (attack_range == 0)return position.distance(center) <= 0.1f;
    return CalculateHelper::calculateDistanceToSquare(position,center,size) <= attack_range;
}

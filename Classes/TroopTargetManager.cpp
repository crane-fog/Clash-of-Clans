#include "TroopTargetManager.h"
#include <algorithm>
#include <limits>
#include <queue>
#include <typeinfo>
#include "CalculateHelper.h"


// 用于优先队列的节点结构 (使用tuple避免模板问题)
//using DistanceNode = std::tuple<cocos2d::Vec2, float>; // position, distance

TroopTargetManager* TroopTargetManager::getInstance() {
    static TroopTargetManager instance;
    // 初始化4个类型容器 (OTHER, RESOURCE, DEFENSE, WALLT)
    if (instance.targets_.empty()) {
        instance.targets_.resize(4);
    }
    // 初始化6个兵种类型的距离场容器
    if (instance.distance_fields_.empty()) {
        instance.distance_fields_.resize(6); // 6种兵种类型
    }
    // 初始化墙代价地图（x是行，y是列）
    if (instance.wall_cost_map_.empty()) {
        instance.wall_cost_map_ = std::vector<std::vector<float>>(MAP_WIDTH, std::vector<float>(MAP_HEIGHT, 1.0f));
    }
    return &instance;
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
        container.erase(it);
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

void TroopTargetManager::onTargetDestroyed(ITroopTarget* target) {
    if (!target) return;

    // 获取建筑位置
    float target_size;
    cocos2d::Vec2 target_pos = target->getCellPosition(target_size);

    if (target->getTargetType() == Troop::WALLT) {
        // 墙被摧毁：将wall_cost_map_中对应位置设为普通地面
        // 墙是1x1的，只需要把其格子设置为地面
        int x = static_cast<int>(target_pos.x);
        int y = static_cast<int>(target_pos.y);
        if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
            wall_cost_map_[x][y] = 0.0f; // 设为普通地面
        }

        // 墙的摧毁会影响所有地面兵种的路径，但这里暂时不重新计算
        // 因为重新计算所有距离场开销太大，应该在需要时延迟计算
        // TODO: 延迟计算
    }
    else {
        // 其他建筑被摧毁：清除对应的距离场数据
        for (auto& troop_distance_fields : distance_fields_) {
            troop_distance_fields.erase(target);
        }
        // 计算建筑占据的格子范围
        int left = static_cast<int>(target_pos.x - target_size / 2.0f);
        int right = static_cast<int>(target_pos.x + target_size / 2.0f);
        int bottom = static_cast<int>(target_pos.y - target_size / 2.0f);
        int top = static_cast<int>(target_pos.y + target_size / 2.0f);

        // 将wall_cost_map_中对应位置设为普通地面
        for (int y = std::max(0, bottom); y <= std::min(MAP_HEIGHT - 1, top); ++y) {
            for (int x = std::max(0, left); x <= std::min(MAP_WIDTH - 1, right); ++x) {
                wall_cost_map_[x][y] = 0.0f; // 设为普通地面
            }
        }

        // 其他建筑的摧毁会影响现有路径
        // TODO: 延迟计算
    }
}

void TroopTargetManager::precomputeWallCostMap() {
    // 初始化为0.0（普通地面无额外代价）
    wall_cost_map_ = std::vector<std::vector<float>>(MAP_WIDTH, std::vector<float>(MAP_HEIGHT, 0.0f));

    // 第一遍：将所有建筑设为阻挡代价（基本不可通行）
    for (size_t target_type = 0; target_type < targets_.size(); ++target_type) {
        if (target_type == static_cast<size_t>(Troop::WALLT)) continue; // 跳过墙，墙在第二遍处理
        for (ITroopTarget* building : targets_[target_type]) {
            if (!building->isAlive()) continue;

            float building_size;
            cocos2d::Vec2 building_pos = building->getCellPosition(building_size);

            // 计算建筑占据的格子范围
            int left = static_cast<int>(building_pos.x - building_size / 2.0f);
            int right = static_cast<int>(building_pos.x + building_size / 2.0f);
            int bottom = static_cast<int>(building_pos.y - building_size / 2.0f);
            int top = static_cast<int>(building_pos.y + building_size / 2.0f);

            // 标记建筑占据的区域为阻挡代价（额外代价）
            for (int y = std::max(0, bottom); y <= std::min(MAP_HEIGHT - 1, top); ++y) {
                for (int x = std::max(0, left); x <= std::min(MAP_WIDTH - 1, right); ++x) {
                    wall_cost_map_[x][y] = BUILDING_BLOCK_COST; // 999.0f - 基本不可通行
                }
            }
        }
    }

    // 第二遍：将墙的代价设置为可通行但很高代价（额外代价）
    if (!targets_[static_cast<size_t>(Troop::WALLT)].empty()) {
        for (ITroopTarget* wall : targets_[static_cast<size_t>(Troop::WALLT)]) {
            if (!wall->isAlive()) continue;

            float wall_size;
            cocos2d::Vec2 wall_pos = wall->getCellPosition(wall_size);

            // 墙大小固定为1x1，设置墙额外代价
            int wx = static_cast<int>(wall_pos.x);
            int wy = static_cast<int>(wall_pos.y);

            if (wx >= 0 && wx < MAP_WIDTH && wy >= 0 && wy < MAP_HEIGHT) {
                wall_cost_map_[wx][wy] = WALL_COST; // 10.0f - 可通行但代价很高
            }
        }
    }
}

void TroopTargetManager::precomputeDistanceFields(std::vector<Troop*>& troop_vec) {
    // 首先预计算墙代价地图
    precomputeWallCostMap();

    // 清除所有距离场数据
    for (auto& troop_distance_fields : distance_fields_) {
        troop_distance_fields.clear();
    }

    // 只为需要考虑墙的兵种计算距离场 (Archer, Barbarian, Giant)
    // Balloon和Dragon是空中兵种，无视墙
    // WallBreaker有特殊机制，暂时跳过
	bool needs_wall_consideration[6] = { true, true, true, false, false, false };

    for (Troop* troop : troop_vec) {
        size_t troop_type_idx = static_cast<size_t>(troop->getTroopTypeIndex());
		if (!needs_wall_consideration[troop_type_idx])continue; // 跳过不考虑墙的兵种
		needs_wall_consideration[troop_type_idx] = false; // 每种兵种只计算一次
        // 遍历除墙以外的所有建筑类型
        for (size_t target_type_idx = 0; target_type_idx < targets_.size(); ++target_type_idx) {
            if (target_type_idx == static_cast<size_t>(Troop::WALLT)) continue; // 跳过墙

            for (ITroopTarget* target : targets_[target_type_idx]) {
                if (target->isAlive()) {
                    computeDistanceField(target, troop);
                }
            }
        }
    }
}

void TroopTargetManager::computeDistanceField(ITroopTarget* target, Troop* troop) {
    if (!target||target->getTargetType()==Troop::WALLT) return; // 墙没有距离场数据

    Troop::TroopType troop_type = troop->getTroopTypeIndex();

    // 初始化距离场为无限大（所有位置都不可达）
    std::vector<std::vector<float>> distance_field(MAP_WIDTH,
        std::vector<float>(MAP_HEIGHT, std::numeric_limits<float>::max()));

    // 获取目标位置和大小
    float target_size;
    cocos2d::Vec2 target_pos = target->getCellPosition(target_size);

    // 使用Dijkstra算法从目标建筑向外扩散
    auto cmp = [](const std::tuple<cocos2d::Vec2, float>& a, const std::tuple<cocos2d::Vec2, float>& b) {
        return std::get<1>(a) > std::get<1>(b);
    };
    std::priority_queue<std::tuple<cocos2d::Vec2, float>, std::vector<std::tuple<cocos2d::Vec2, float>>, decltype(cmp)> pq(cmp);

    // 获取该兵种类型的攻击范围
    float attack_range = troop->range_;

    // 将目标建筑周围在攻击范围内的格子设为0
    int target_left = static_cast<int>(target_pos.x - target_size / 2.0f);
    int target_right = static_cast<int>(target_pos.x + target_size / 2.0f);
    int target_bottom = static_cast<int>(target_pos.y - target_size / 2.0f);
    int target_top = static_cast<int>(target_pos.y + target_size / 2.0f);

    for (int y = std::max(0, target_bottom); y <= std::min(MAP_HEIGHT - 1, target_top); ++y) {
        for (int x = std::max(0, target_left); x <= std::min(MAP_WIDTH - 1, target_right); ++x) {
            cocos2d::Vec2 grid_pos(static_cast<float>(x), static_cast<float>(y));
            float distance_to_target = CalculateHelper::calculateDistanceToSquare(grid_pos, target_pos, target_size);

            // 检查该位置是否可通行
            bool is_passable = true;
            if (troop_type == Troop::WALL_BREAKER) {
                // 炸弹人可以穿过一切（暂时）
                is_passable = true;
            }
            else if (doesTroopConsiderWalls(troop_type)) {
                // 地面兵种：不能穿过建筑阻挡，但可以穿过墙，需要实时检测是否遇到墙！！！
                is_passable = wall_cost_map_[x][y] < BUILDING_BLOCK_COST;
            }
            else {
                // 空中兵种：可以穿过一切
                is_passable = true;
            }

            // 如果在攻击范围内且位置可通行，设为0并加入优先队列
            if (distance_to_target <= attack_range && is_passable) {
                distance_field[x][y] = 0.0f;
                pq.push(std::make_tuple(grid_pos, 0.0f));
            }
        }
    }

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
            bool is_neighbor_passable = true;
            if (troop_type == Troop::WALL_BREAKER) {
                // 炸弹人可以穿过一切（暂时）
                is_neighbor_passable = true;
            }
            else if (doesTroopConsiderWalls(troop_type)) {
                // 地面兵种：不能穿过建筑阻挡，但可以穿过墙
                is_neighbor_passable = wall_cost_map_[nx][ny] < BUILDING_BLOCK_COST;
            }
            else {
                // 空中兵种：可以穿过一切
                is_neighbor_passable = true;
            }

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
    size_t troop_type_idx = static_cast<size_t>(troop->getTroopTypeIndex());
    distance_fields_[troop_type_idx][target] = std::move(distance_field);
}

const std::vector<std::vector<float>>& TroopTargetManager::getDistanceField(ITroopTarget* target, Troop* troop) const {
    static const std::vector<std::vector<float>> empty_field;
    size_t troop_type_idx = static_cast<size_t>(troop->getTroopTypeIndex());
    if (troop_type_idx >= distance_fields_.size()) return empty_field;

    auto it = distance_fields_[troop_type_idx].find(target);
    return (it != distance_fields_[troop_type_idx].end()) ? it->second : empty_field;
}

cocos2d::Vec2 TroopTargetManager::getNextMoveDirection(const cocos2d::Vec2& current_pos, ITroopTarget* target, Troop* troop) {
	Troop::TroopType troop_type = troop->getTroopTypeIndex();
    const auto& distance_field = getDistanceField(target, troop);
    if (distance_field.empty()) return cocos2d::Vec2::ZERO;

    // 检查当前位置是否在攻击范围内
    if (isInAttackRange(current_pos, target,troop )) {
        return cocos2d::Vec2::ZERO; // 不需要移动，已经在攻击范围内
    }

    // 获取当前位置的距离值
    if (!isValidPosition(current_pos)) return cocos2d::Vec2::ZERO;

    int x = static_cast<int>(current_pos.x);
    int y = static_cast<int>(current_pos.y);
    float current_distance = distance_field[x][y];

    // 如果当前位置是墙或其他不可达区域，返回零向量
    if (current_distance >= std::numeric_limits<float>::max() / 2) return cocos2d::Vec2::ZERO;

    // 查找周围8个方向中距离值最小的方向
    cocos2d::Vec2 best_direction = cocos2d::Vec2::ZERO;
    float min_distance = current_distance;

    const std::vector<cocos2d::Vec2> directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0},  // 上下左右
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}  // 斜向
    };

    for (const auto& dir : directions) {
        cocos2d::Vec2 neighbor = current_pos + dir;
        if (!isValidPosition(neighbor)) continue;

        int nx = static_cast<int>(neighbor.x);
        int ny = static_cast<int>(neighbor.y);
        float neighbor_distance = distance_field[nx][ny];

        // 跳过不可达区域
        if (neighbor_distance >= std::numeric_limits<float>::max() / 2) continue;

        if (neighbor_distance < min_distance) {
            min_distance = neighbor_distance;
            best_direction = dir;
        }
    }

    return best_direction;
}

bool TroopTargetManager::isInAttackRange(const cocos2d::Vec2& position, ITroopTarget* target, Troop* troop) const {
    const auto& distance_field = getDistanceField(target, troop);
    if (distance_field.empty()) return false;

    if (!isValidPosition(position)) return false;

    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);
    float distance = distance_field[x][y];

    // 如果距离为0，说明在攻击范围内
    return distance == 0.0f;
}

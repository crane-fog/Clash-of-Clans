#ifndef __COC_UTILITY_H__
#define __COC_UTILITY_H__

#include <cmath>
#include <string>
#include <vector>

#include "cocos2d.h"
#include "ArchInfo.h"
#include "BaseMap.h"

const std::string kSourceDataFile = "data/SourceData.dat";
const std::string kMainVillageDataFile = "data/MainVillageData.dat";
const std::string kOfflineDataFile[] = {
    "data/LevelInfo.dat", "data/Level1.dat", "data/Level2.dat", "data/Level3.dat", "data/Level4.dat",
};
const std::string kReplayDataFile = "data/ReplayData.dat";

struct LevelInfo {
    // 关卡ID
    int level_;

    // 关卡最大进度(0-100)
    unsigned char progress_;

    // 剩余可获取的资源量（弃用）
    unsigned int gold_;
    unsigned int elixir_;
};

struct DeploymentInfo {
    int type_;
    float time_;
    float x_;
    float y_;
};

struct ReplayData {
    std::vector<DeploymentInfo> deployments_;
    int level_;
    time_t timestamp_;
};

namespace CalculateHelper {
// 计算点到方形的最小距离
float calculateDistanceToSquare(const cocos2d::Vec2& point, const cocos2d::Vec2& square_center, float square_size);
};

namespace CoordAdaptor {
// 格子坐标转像素坐标
cocos2d::Vec2 cellToPixel(const cocos2d::Node* const kBaseMap, const cocos2d::Vec2& original);

// 格子坐标移动量转像素坐标移动量
cocos2d::Vec2 cellDeltaToPixelDelta(const cocos2d::Node* const kBaseMap, const cocos2d::Vec2& delta);

// 像素坐标转格子坐标
cocos2d::Vec2 pixelToCell(const cocos2d::Node* const kBaseMap, const cocos2d::Vec2& pixelPos);

// 层级计算
int calcOrder(const cocos2d::Vec2& middle_pos);
}

// 数据文件操作
namespace DataHelper {
// [44][44]的地图表形式与一个list形式的转换
// 建筑地图表转换为建筑列表（需要保证target为空）
void mapToList(const ArchData source[kMapSize][kMapSize], std::vector<ArchData>& target);

// 建筑列表转换为建筑地图表（需要保证source有内容）
void listToMap(const std::vector<ArchData>& source, ArchData target[kMapSize][kMapSize]);

// 读数据文件
bool readArchData(const std::string& file_name, time_t& time, ArchData target[kMapSize][kMapSize]);
bool readSourceData(const std::string& file_name, unsigned long long& gold, unsigned long long& elixir,
                           unsigned long long& jewel);
bool readLevelData(const std::string& file_name, std::vector<LevelInfo>& level_info_list);
bool readReplayData(const std::string& filename, std::vector<ReplayData>& data);

// 写数据文件
bool writeArchData(const std::string& file_name, time_t time, const ArchData source[kMapSize][kMapSize]);
bool writeSourceData(const std::string& file_name, const unsigned long long gold,
                            const unsigned long long elixir, const unsigned long long jewel);
bool writeLevelData(const std::string& file_name, const std::vector<LevelInfo>& level_info_list);
bool addReplayData(const std::string& filename, const ReplayData& data);
}

#endif  // __COC_UTILITY_H__
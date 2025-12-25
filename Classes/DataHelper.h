#ifndef __DATA_HELPER_H__
#define __DATA_HELPER_H__

#include <string>
#include <vector>

#include "Arch.h"
#include "BaseMap.h"

const std::string kSourceDataFile = "data/SourceData.dat";
const std::string kMainVillageDataFile = "data/MainVillageData.dat";
const std::string kOfflineDataFile[] = {
    "data/LevelInfo.dat", "data/Level1.dat", "data/Level2.dat", "data/Level3.dat", "data/Level4.dat",
};

struct LevelInfo {
    // 关卡ID
    int level_;

    // 关卡最大进度(0-100)
    unsigned char progress_;

    // 剩余可获取的资源量
    unsigned int gold_;
    unsigned int elixir_;
};

// todo: 想把纯静态类改为使用命名空间
// 数据文件操作
class DataHelper {
public:
    // [44][44]的地图表形式与一个list形式的转换
    // 建筑地图表转换为建筑列表（需要保证target为空）
    static void mapToList(const ArchData source[kMapSize][kMapSize], std::vector<ArchData>& target);

    // 建筑列表转换为建筑地图表（需要保证source有内容）
    static void listToMap(const std::vector<ArchData>& source, ArchData target[kMapSize][kMapSize]);

    // 读数据文件
    static bool readArchData(const std::string& file_name, time_t& time, ArchData target[kMapSize][kMapSize]);
    static bool readSourceData(const std::string& file_name, unsigned long long& gold, unsigned long long& elixir);
    static bool readLevelData(const std::string& file_name, std::vector<LevelInfo>& level_info_list);

    // 写数据文件
    static bool writeArchData(const std::string& file_name, time_t time, const ArchData source[kMapSize][kMapSize]);
    static bool writeSourceData(const std::string& file_name, const unsigned long long kGold,
                                const unsigned long long kElixir);
    static bool writeLevelData(const std::string& file_name, const std::vector<LevelInfo>& level_info_list);
};
#endif  // __DATA_HELPER_H__
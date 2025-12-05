#ifndef __DATA_HELPER_H__
#define __DATA_HELPER_H__

#include <string>
#include <vector>
#include "Arch.h"
#include "BaseMap.h"

const std::string kSourceDataFile = "data/SourceData.dat";
const std::string kMainVillageDataFile = "data/MainVillageData.dat";

// 数据文件操作
class DataHelper {
public:
    // [44][44]的地图表形式与一个list形式的转换
    // 建筑地图表转换为建筑列表（需要保证target为空）
    static void mapToList(const ArchData source[MAP_SIZE][MAP_SIZE], std::vector<ArchData>& target);

    // 建筑列表转换为建筑地图表（需要保证source有内容）
    static void listToMap(const std::vector<ArchData>& source, ArchData target[MAP_SIZE][MAP_SIZE]);

    // 读数据文件
    static bool readArchData(const std::string& file_name, time_t& time, ArchData target[MAP_SIZE][MAP_SIZE]);
    static bool readSourceData(const std::string& file_name, unsigned long long& gold, unsigned long long& elixir);

    // 写数据文件
    static bool writeArchData(const std::string& file_name, time_t time, const ArchData source[MAP_SIZE][MAP_SIZE]);
    static bool writeSourceData(const std::string& file_name, const unsigned long long gold, const unsigned long long elixir);
};
#endif // __DATA_HELPER_H__
#ifndef __DATA_HELPER_H__
#define __DATA_HELPER_H__

#include <string>
#include "Arch.h"
#include "BaseMap.h"

const std::string kMainVillageDataFile = "data/MainVillageData.dat";

// 用于读取建筑数据
struct ArchData {
    typedef unsigned int UI;
    typedef unsigned char UC;
    // 建筑种类的编号
    UC no_;
    // 建筑等级
    UC level_;
    // 建筑位置
    UC x_;
    UC y_;

    // 资源建筑
    // 当前容量
    UI current_capacity_;
};

// 数据文件操作
class DataHelper {
public:
    // 读数据文件
    static bool readArchData(const std::string& file_name, unsigned long long& time, ArchStatus target[MAP_SIZE][MAP_SIZE]);
    // 写数据文件
    static bool writeArchData(const std::string& file_name, unsigned long long time, const ArchStatus source[MAP_SIZE][MAP_SIZE]);
};
#endif // __DATA_HELPER_H__
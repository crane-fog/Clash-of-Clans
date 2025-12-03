#include <fstream>
#include "DataHelper.h"
#include "ArchInfo.h"
// todo：改用cocos2d::FileUtils

void DataHelper::mapToList(const ArchStatus source[MAP_SIZE][MAP_SIZE], std::vector<ArchData>& target)
{
    unsigned char size = 0;
    ArchData temp;
    bool visited[MAP_SIZE][MAP_SIZE] = { false };
    for (int x = 0; x < MAP_SIZE; x++) {
        for (int y = 0; y < MAP_SIZE; y++) {
            if (visited[x][y]) {
                continue;
            }

            const ArchStatus& current = source[x][y];

            if (current.no_ == INVALID_ARCH_NO) {
                visited[x][y] = true;
                continue;
            }

            temp.no_ = current.no_;
            temp.level_ = current.level_;
            temp.x_ = static_cast<unsigned char>(x);
            temp.y_ = static_cast<unsigned char>(y);
            temp.current_capacity_ = current.current_capacity_;
            target.push_back(temp);

            size = kArchInfo.at(current.no_)[current.level_ - 1].size_;

            for (unsigned char dx = 0; dx < size; ++dx) {
                for (unsigned char dy = 0; dy < size; ++dy) {
                    if (x + dx < MAP_SIZE && y + dy < MAP_SIZE) {
                        visited[x + dx][y + dy] = true;
                    }
                }
            }
        }
    }
}

void DataHelper::listToMap(const std::vector<ArchData>& source, ArchStatus target[MAP_SIZE][MAP_SIZE])
{
    unsigned char size = 0, x = 0, y = 0;
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            target[i][j].no_ = INVALID_ARCH_NO;
        }
    }

    for (unsigned short i = 0; i < source.size(); i++) {
        x = source[i].x_;
        y = source[i].y_;
        target[x][y].no_ = source[i].no_;
        target[x][y].level_ = source[i].level_;
        target[x][y].current_hp_ = kArchInfo.at(source[i].no_)[source[i].level_ - 1].hp_;
        target[x][y].current_capacity_ = source[i].current_capacity_;
        // 填充建筑占地
        for (unsigned char dx = 0; dx < kArchInfo.at(source[i].no_)[source[i].level_ - 1].size_; dx++) {
            for (unsigned char dy = 0; dy < kArchInfo.at(source[i].no_)[source[i].level_ - 1].size_; dy++) {
                if (!(dx == 0 && dy == 0)) {
                    target[x + dx][y + dy] = target[x][y];
                }
            }
        }
    }
}

bool DataHelper::readArchData(const std::string& file_name, time_t& time, ArchStatus target[MAP_SIZE][MAP_SIZE])
{
    unsigned char x = 0, y = 0;
    unsigned short num = 0;
    std::vector<ArchData> buffer;
    std::ifstream infile(file_name, std::ios::binary);
    if (!infile) {
        return false;
    }

    infile.read(reinterpret_cast<char*>(&time), sizeof(unsigned long long));
    infile.read(reinterpret_cast<char*>(&num), sizeof(unsigned short));

    buffer.resize(num);
    infile.read(reinterpret_cast<char*>(buffer.data()), sizeof(ArchData) * num);

    listToMap(buffer, target);

    infile.close();
    return true;
}

bool DataHelper::writeArchData(const std::string& file_name, time_t time, const ArchStatus source[MAP_SIZE][MAP_SIZE])
{
    unsigned short num = 0;
    std::vector<ArchData> data;
    std::ofstream outfile(file_name, std::ios::binary);
    if (!outfile) {
        return false;
    }

    outfile.write(reinterpret_cast<const char*>(&time), sizeof(unsigned long long));

    mapToList(source, data);

    num = static_cast<unsigned short>(data.size());
    outfile.write(reinterpret_cast<const char*>(&num), sizeof(unsigned short));

    if (num > 0) {
        outfile.write(reinterpret_cast<const char*>(data.data()), sizeof(ArchData) * num);
    }

    return true;
}
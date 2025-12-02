#include <fstream>
#include <vector>
#include "DataHelper.h"
#include "ArchInfo.h"
// todo：改用cocos2d::FileUtils

bool DataHelper::readArchData(const std::string& file_name, unsigned long long& time, ArchStatus target[MAP_SIZE][MAP_SIZE])
{
    unsigned char x = 0, y = 0;
    unsigned short num = 0;
    ArchData* buffer = nullptr;
    std::ifstream infile(file_name, std::ios::binary);
    if (!infile) {
        return false;
    }

    infile.read(reinterpret_cast<char*>(&time), sizeof(unsigned long long));
    infile.read(reinterpret_cast<char*>(&num), sizeof(unsigned short));

    buffer = new ArchData[num];
    infile.read(reinterpret_cast<char*>(buffer), sizeof(ArchData) * num);
    
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            target[i][j].no_ = INVALID_ARCH_NO;
        }
    }

    for (unsigned short i = 0; i < num; i++) {
        x = buffer[i].x_;
        y = buffer[i].y_;
        target[x][y].no_ = buffer[i].no_;
        target[x][y].level_ = buffer[i].level_;
        target[x][y].current_hp_ = kArchInfo.at(buffer[i].no_)[buffer[i].level_ - 1].hp_;
        target[x][y].current_capacity_ = buffer[i].current_capacity_;
        // 填充建筑占地
        for (unsigned char dx = 0; dx < kArchInfo.at(buffer[i].no_)[buffer[i].level_ - 1].size_; dx++) {
            for (unsigned char dy = 0; dy < kArchInfo.at(buffer[i].no_)[buffer[i].level_ - 1].size_; dy++) {
                if (!(dx == 0 && dy == 0)) {
                    if (x + dx >= MAP_SIZE || y + dy >= MAP_SIZE) {
                        delete[] buffer;
                        return false;
                    }
                    target[x + dx][y + dy] = target[x][y];
                }
            }
        }
    }

    delete[] buffer;
    infile.close();
    return true;
}

bool DataHelper::writeArchData(const std::string& file_name, unsigned long long time, const ArchStatus source[MAP_SIZE][MAP_SIZE])
{
    unsigned short num = 0;
    unsigned char size = 0;
    std::vector<ArchData> data;
    ArchData temp;
    std::ofstream outfile(file_name, std::ios::binary);
    if (!outfile) {
        return false;
    }

    outfile.write(reinterpret_cast<const char*>(&time), sizeof(unsigned long long));

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
            data.push_back(temp);

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

    num = static_cast<unsigned short>(data.size());
    outfile.write(reinterpret_cast<const char*>(&num), sizeof(unsigned short));

    if (num > 0) {
        outfile.write(reinterpret_cast<const char*>(data.data()), sizeof(ArchData) * num);
    }

    return true;
}
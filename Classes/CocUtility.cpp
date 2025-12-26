#include "CocUtility.h"

#include <fstream>

#include "ArchInfo.h"

float CalculateHelper::calculateDistanceToSquare(const cocos2d::Vec2& point, const cocos2d::Vec2& square_center,
                                                 float square_size)
{
    float half_size = square_size / 2.0f;

    // 计算方形的边界
    float left = square_center.x - half_size;
    float right = square_center.x + half_size;
    float top = square_center.y + half_size;
    float bottom = square_center.y - half_size;

    // 计算点到方形的距离
    float dx = 0.0f;
    float dy = 0.0f;

    if (point.x < left) {
        dx = left - point.x;
    }
    else if (point.x > right) {
        dx = point.x - right;
    }

    if (point.y < bottom) {
        dy = bottom - point.y;
    }
    else if (point.y > top) {
        dy = point.y - top;
    }

    // 如果点在方形内部，距离为0
    if (dx == 0.0f && dy == 0.0f) {
        return 0.0f;
    }

    // 计算欧几里得距离
    return sqrtf(dx * dx + dy * dy);
}

cocos2d::Vec2 CoordAdaptor::cellToPixel(const cocos2d::Node* const kBaseMap, const cocos2d::Vec2& original)
{
    float cell_width_zero = kBaseMap->getContentSize().width * 0.17813765f;
    float cell_height_zero = kBaseMap->getContentSize().height * 0.53831041f;
    float cell_width = kBaseMap->getContentSize().width * 0.00757575f;
    float cell_height = kBaseMap->getContentSize().height * 0.00826040f;
    return cocos2d::Vec2((original.x + original.y) * cell_width + cell_width_zero,
                         (original.y - original.x) * cell_height + cell_height_zero);
}

cocos2d::Vec2 CoordAdaptor::cellDeltaToPixelDelta(const cocos2d::Node* const kBaseMap, const cocos2d::Vec2& delta)
{
    float cell_width = kBaseMap->getContentSize().width * 0.00757575f;
    float cell_height = kBaseMap->getContentSize().height * 0.00826040f;
    return cocos2d::Vec2((delta.x + delta.y) * cell_width, (delta.y - delta.x) * cell_height);
}

cocos2d::Vec2 CoordAdaptor::pixelToCell(const cocos2d::Node* const kBaseMap, const cocos2d::Vec2& pixelPos)
{
    float cell_width_zero = kBaseMap->getContentSize().width * 0.17813765f;
    float cell_height_zero = kBaseMap->getContentSize().height * 0.53831041f;
    float cell_width = kBaseMap->getContentSize().width * 0.00757575f;
    float cell_height = kBaseMap->getContentSize().height * 0.00826040f;

    float x_prime = (pixelPos.x - cell_width_zero) / cell_width;
    float y_prime = (pixelPos.y - cell_height_zero) / cell_height;

    float cx = (x_prime - y_prime) / 2.0f;
    float cy = (x_prime + y_prime) / 2.0f;

    return cocos2d::Vec2(cx, cy);
}

int CoordAdaptor::calcOrder(const cocos2d::Vec2& middle_pos)
{
    return static_cast<int>(middle_pos.x - middle_pos.y) + 50;
}

void DataHelper::mapToList(const ArchData source[kMapSize][kMapSize], std::vector<ArchData>& target)
{
    unsigned char size = 0;
    ArchData temp;
    bool visited[kMapSize][kMapSize] = {false};
    for (int x = 0; x < kMapSize; x++) {
        for (int y = 0; y < kMapSize; y++) {
            if (visited[x][y]) {
                continue;
            }

            const ArchData& current = source[x][y];

            if (current.no_ == kInvalidArchNo) {
                visited[x][y] = true;
                continue;
            }

            temp.no_ = current.no_;
            temp.level_ = current.level_;
            temp.x_ = static_cast<unsigned char>(x);
            temp.y_ = static_cast<unsigned char>(y);
            temp.current_hp_ = 0;  // 存储时不保存当前生命值，读取时根据等级自动填充满血
            temp.remaining_upgrade_time_ = current.remaining_upgrade_time_;
            temp.current_capacity_ = current.current_capacity_;
            target.push_back(temp);

            size = kArchInfo.at(current.no_)[current.level_ - 1].size_;

            for (unsigned char dx = 0; dx < size; ++dx) {
                for (unsigned char dy = 0; dy < size; ++dy) {
                    if (x + dx < kMapSize && y + dy < kMapSize) {
                        visited[x + dx][y + dy] = true;
                    }
                }
            }
        }
    }
}

void DataHelper::listToMap(const std::vector<ArchData>& source, ArchData target[kMapSize][kMapSize])
{
    unsigned char size = 0, x = 0, y = 0;
    for (int i = 0; i < kMapSize; i++) {
        for (int j = 0; j < kMapSize; j++) {
            target[i][j].no_ = kInvalidArchNo;
        }
    }

    for (unsigned short i = 0; i < source.size(); i++) {
        x = source[i].x_;
        y = source[i].y_;
        target[x][y].no_ = source[i].no_;
        target[x][y].level_ = source[i].level_;
        target[x][y].x_ = source[i].x_;  // 此处填充的x统一为min(x)
        target[x][y].y_ = source[i].y_;  // min(y)
        target[x][y].current_hp_ = kArchInfo.at(source[i].no_)[source[i].level_ - 1].hp_;
        target[x][y].remaining_upgrade_time_ = source[i].remaining_upgrade_time_;
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

bool DataHelper::readArchData(const std::string& file_name, time_t& time, ArchData target[kMapSize][kMapSize])
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

bool DataHelper::writeArchData(const std::string& file_name, time_t time, const ArchData source[kMapSize][kMapSize])
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

bool DataHelper::readSourceData(const std::string& file_name, unsigned long long& gold, unsigned long long& elixir,
                                unsigned long long& jewel)
{
    std::ifstream infile(file_name, std::ios::binary);
    if (!infile) {
        return false;
    }
    infile.read(reinterpret_cast<char*>(&gold), sizeof(unsigned long long));
    infile.read(reinterpret_cast<char*>(&elixir), sizeof(unsigned long long));
    infile.read(reinterpret_cast<char*>(&jewel), sizeof(unsigned long long));
    infile.close();
    return true;
}

bool DataHelper::readReplayData(const std::string& filename, std::vector<ReplayData>& data)
{
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        return false;
    }
    size_t count = 0;
    infile.read(reinterpret_cast<char*>(&count), sizeof(count));
    data.resize(count);
    for (size_t i = 0; i < count; ++i) {
        size_t deploy_count = 0;
        infile.read(reinterpret_cast<char*>(&deploy_count), sizeof(deploy_count));
        data[i].deployments_.resize(deploy_count);
        if (deploy_count > 0) {
            infile.read(reinterpret_cast<char*>(data[i].deployments_.data()), deploy_count * sizeof(DeploymentInfo));
        }
        infile.read(reinterpret_cast<char*>(&data[i].level_), sizeof(data[i].level_));
        infile.read(reinterpret_cast<char*>(&data[i].timestamp_), sizeof(data[i].timestamp_));
    }
    infile.close();
    return true;
}

bool DataHelper::writeSourceData(const std::string& file_name, const unsigned long long gold,
                                 const unsigned long long elixir, const unsigned long long jewel)
{
    std::ofstream outfile(file_name, std::ios::binary);
    if (!outfile) {
        return false;
    }
    outfile.write(reinterpret_cast<const char*>(&gold), sizeof(unsigned long long));
    outfile.write(reinterpret_cast<const char*>(&elixir), sizeof(unsigned long long));
    outfile.write(reinterpret_cast<const char*>(&jewel), sizeof(unsigned long long));
    return true;
}

bool DataHelper::readLevelData(const std::string& file_name, std::vector<LevelInfo>& level_info_list)
{
    unsigned short num = 0;
    std::ifstream infile(file_name, std::ios::binary);
    if (!infile) {
        return false;
    }
    infile.read(reinterpret_cast<char*>(&num), sizeof(unsigned short));
    level_info_list.resize(num);
    infile.read(reinterpret_cast<char*>(level_info_list.data()), sizeof(LevelInfo) * num);
    infile.close();
    return true;
}

bool DataHelper::writeLevelData(const std::string& file_name, const std::vector<LevelInfo>& level_info_list)
{
    unsigned short num = static_cast<unsigned short>(level_info_list.size());
    std::ofstream outfile(file_name, std::ios::binary);
    if (!outfile) {
        return false;
    }
    outfile.write(reinterpret_cast<const char*>(&num), sizeof(unsigned short));
    if (num > 0) {
        outfile.write(reinterpret_cast<const char*>(level_info_list.data()), sizeof(LevelInfo) * num);
    }
    return true;
}

bool DataHelper::addReplayData(const std::string& filename, const ReplayData& data)
{
    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        return false;
    }
    size_t count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    count++;
    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    file.seekp(0, std::ios::end);

    size_t deploy_count = data.deployments_.size();
    file.write(reinterpret_cast<const char*>(&deploy_count), sizeof(deploy_count));
    if (deploy_count > 0) {
        file.write(reinterpret_cast<const char*>(data.deployments_.data()), deploy_count * sizeof(DeploymentInfo));
    }
    file.write(reinterpret_cast<const char*>(&data.level_), sizeof(data.level_));
    file.write(reinterpret_cast<const char*>(&data.timestamp_), sizeof(data.timestamp_));

    file.close();
    return true;
}

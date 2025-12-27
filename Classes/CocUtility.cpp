#include "CocUtility.h"

#include <stdexcept>

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

static std::string getSaveFilePath(const std::string& original_path)
{
    std::string filename;
    size_t pos = original_path.find_last_of("/\\");
    if (pos != std::string::npos) {
        filename = original_path.substr(pos + 1);
    }
    else {
        filename = original_path;
    }
    return cocos2d::FileUtils::getInstance()->getWritablePath() + filename;
}

static cocos2d::Data readFileData(const std::string& original_path)
{
    std::string savePath = getSaveFilePath(original_path);
    auto fileUtils = cocos2d::FileUtils::getInstance();
    if (fileUtils->isFileExist(savePath)) {
        return fileUtils->getDataFromFile(savePath);
    }
    return fileUtils->getDataFromFile(original_path);
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

void DataHelper::readArchData(const std::string& file_name, time_t& time, ArchData target[kMapSize][kMapSize])
{
    cocos2d::Data data = readFileData(file_name);
    if (data.isNull()) {
        throw std::runtime_error("Failed to open file: " + file_name);
    }

    const unsigned char* bytes = data.getBytes();
    size_t size = data.getSize();
    size_t offset = 0;

    unsigned long long time_val;
    memcpy(&time_val, bytes + offset, sizeof(unsigned long long));
    time = (time_t)time_val;
    offset += sizeof(unsigned long long);

    unsigned short num;
    memcpy(&num, bytes + offset, sizeof(unsigned short));
    offset += sizeof(unsigned short);

    std::vector<ArchData> buffer(num);
    if (num > 0) {
        memcpy(buffer.data(), bytes + offset, sizeof(ArchData) * num);
    }

    listToMap(buffer, target);
}

void DataHelper::writeArchData(const std::string& file_name, time_t time, const ArchData source[kMapSize][kMapSize])
{
    std::vector<ArchData> data;
    mapToList(source, data);
    unsigned short num = static_cast<unsigned short>(data.size());

    size_t totalSize = sizeof(unsigned long long) + sizeof(unsigned short) + sizeof(ArchData) * num;

    cocos2d::Data fileData;
    unsigned char* buffer = (unsigned char*)malloc(totalSize);
    size_t offset = 0;

    unsigned long long time_val = (unsigned long long)time;
    memcpy(buffer + offset, &time_val, sizeof(unsigned long long));
    offset += sizeof(unsigned long long);

    memcpy(buffer + offset, &num, sizeof(unsigned short));
    offset += sizeof(unsigned short);

    if (num > 0) {
        memcpy(buffer + offset, data.data(), sizeof(ArchData) * num);
    }

    fileData.fastSet(buffer, totalSize);

    std::string savePath = getSaveFilePath(file_name);
    if (!cocos2d::FileUtils::getInstance()->writeDataToFile(fileData, savePath)) {
        throw std::runtime_error("Failed to write file: " + savePath);
    }
}

void DataHelper::readSourceData(const std::string& file_name, unsigned long long& gold, unsigned long long& elixir,
                                unsigned long long& jewel)
{
    cocos2d::Data data = readFileData(file_name);
    if (data.isNull()) {
        throw std::runtime_error("Failed to open file: " + file_name);
    }

    const unsigned char* bytes = data.getBytes();
    size_t size = data.getSize();
    size_t offset = 0;

    memcpy(&gold, bytes + offset, sizeof(unsigned long long));
    offset += sizeof(unsigned long long);
    memcpy(&elixir, bytes + offset, sizeof(unsigned long long));
    offset += sizeof(unsigned long long);
    memcpy(&jewel, bytes + offset, sizeof(unsigned long long));
}

void DataHelper::readReplayData(const std::string& filename, std::vector<ReplayData>& data)
{
    cocos2d::Data fileData = readFileData(filename);
    if (fileData.isNull()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    const unsigned char* bytes = fileData.getBytes();
    size_t size = fileData.getSize();
    size_t offset = 0;

    size_t count = 0;
    memcpy(&count, bytes + offset, sizeof(count));
    offset += sizeof(count);

    data.resize(count);
    for (size_t i = 0; i < count; ++i) {
        size_t deploy_count = 0;
        memcpy(&deploy_count, bytes + offset, sizeof(deploy_count));
        offset += sizeof(deploy_count);

        data[i].deployments_.resize(deploy_count);
        if (deploy_count > 0) {
            size_t deploySize = deploy_count * sizeof(DeploymentInfo);
            memcpy(data[i].deployments_.data(), bytes + offset, deploySize);
            offset += deploySize;
        }

        memcpy(&data[i].level_, bytes + offset, sizeof(data[i].level_));
        offset += sizeof(data[i].level_);
        memcpy(&data[i].timestamp_, bytes + offset, sizeof(data[i].timestamp_));
        offset += sizeof(data[i].timestamp_);
    }
}

void DataHelper::writeSourceData(const std::string& file_name, const unsigned long long gold,
                                 const unsigned long long elixir, const unsigned long long jewel)
{
    size_t totalSize = sizeof(unsigned long long) * 3;
    cocos2d::Data fileData;
    unsigned char* buffer = (unsigned char*)malloc(totalSize);
    size_t offset = 0;

    memcpy(buffer + offset, &gold, sizeof(unsigned long long));
    offset += sizeof(unsigned long long);
    memcpy(buffer + offset, &elixir, sizeof(unsigned long long));
    offset += sizeof(unsigned long long);
    memcpy(buffer + offset, &jewel, sizeof(unsigned long long));

    fileData.fastSet(buffer, totalSize);

    std::string savePath = getSaveFilePath(file_name);
    if (!cocos2d::FileUtils::getInstance()->writeDataToFile(fileData, savePath)) {
        throw std::runtime_error("Failed to write file: " + savePath);
    }
}

void DataHelper::readLevelData(const std::string& file_name, std::vector<LevelInfo>& level_info_list)
{
    cocos2d::Data data = readFileData(file_name);
    if (data.isNull()) {
        throw std::runtime_error("Failed to open file: " + file_name);
    }

    const unsigned char* bytes = data.getBytes();
    size_t size = data.getSize();
    size_t offset = 0;

    unsigned short num;
    memcpy(&num, bytes + offset, sizeof(unsigned short));
    offset += sizeof(unsigned short);

    level_info_list.resize(num);
    if (num > 0) {
        memcpy(level_info_list.data(), bytes + offset, sizeof(LevelInfo) * num);
    }
}

void DataHelper::writeLevelData(const std::string& file_name, const std::vector<LevelInfo>& level_info_list)
{
    unsigned short num = static_cast<unsigned short>(level_info_list.size());
    size_t totalSize = sizeof(unsigned short) + sizeof(LevelInfo) * num;

    cocos2d::Data fileData;
    unsigned char* buffer = (unsigned char*)malloc(totalSize);
    size_t offset = 0;

    memcpy(buffer + offset, &num, sizeof(unsigned short));
    offset += sizeof(unsigned short);

    if (num > 0) {
        memcpy(buffer + offset, level_info_list.data(), sizeof(LevelInfo) * num);
    }

    fileData.fastSet(buffer, totalSize);

    std::string savePath = getSaveFilePath(file_name);
    if (!cocos2d::FileUtils::getInstance()->writeDataToFile(fileData, savePath)) {
        throw std::runtime_error("Failed to write file: " + savePath);
    }
}

void DataHelper::addReplayData(const std::string& filename, const ReplayData& data)
{
    cocos2d::Data fileData = readFileData(filename);
    if (fileData.isNull()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    const unsigned char* bytes = fileData.getBytes();
    size_t size = fileData.getSize();

    size_t count;
    memcpy(&count, bytes, sizeof(size_t));
    count++;

    size_t deploy_count = data.deployments_.size();
    size_t newDataSize = sizeof(size_t) + deploy_count * sizeof(DeploymentInfo) + sizeof(int) + sizeof(time_t);
    size_t totalSize = size + newDataSize;

    cocos2d::Data newFileData;
    unsigned char* buffer = (unsigned char*)malloc(totalSize);

    memcpy(buffer, bytes, size);
    memcpy(buffer, &count, sizeof(size_t));
    size_t offset = size;
    memcpy(buffer + offset, &deploy_count, sizeof(size_t));
    offset += sizeof(size_t);

    if (deploy_count > 0) {
        memcpy(buffer + offset, data.deployments_.data(), deploy_count * sizeof(DeploymentInfo));
        offset += deploy_count * sizeof(DeploymentInfo);
    }

    memcpy(buffer + offset, &data.level_, sizeof(int));
    offset += sizeof(int);
    memcpy(buffer + offset, &data.timestamp_, sizeof(time_t));

    newFileData.fastSet(buffer, totalSize);

    std::string savePath = getSaveFilePath(filename);
    if (!cocos2d::FileUtils::getInstance()->writeDataToFile(newFileData, savePath)) {
        throw std::runtime_error("Failed to write file: " + savePath);
    }
}

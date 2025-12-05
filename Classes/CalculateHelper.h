#ifndef __CALCULATE_HELPER_H__
#define __CALCULATE_HELPER_H__
#include<cmath>
#include"cocos2d.h"

class CalculateHelper {
public:
    // 计算点到方形的最小距离
    static float calculateDistanceToSquare(const cocos2d::Vec2& point, const cocos2d::Vec2& square_center, float square_size) {
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

};

#endif // __CALCULATE_HELPER_H__
#ifndef __COCCONTROLLER_H__
#define __COCCONTROLLER_H__

#include <vector>
#include "CocUtility.h"

// 控制程序运行流程的单例类（实际上相当于对 Director 又做了一层封装？）
class CocManager {
private:
    CocManager();
    CocManager(const CocManager&) = delete;

    // 一个记录当前正在运行的场景的变量，0=MainVillage，n=当前level
    int current_scene_;
    bool is_replay_ = false;

public:
    // 获取单例实例，第一次调用时创建
    static CocManager* getInstance();

    // 启动游戏
    void run();

    // 结束游戏
    void endGame();

    // 切换当前场景，在任何时候的 Scene 栈中最多只有两个 Scene：自己村庄的 Scene、正在攻击的村庄的 Scene
    void changeScene(int level_no = 0);

    int getCurrentScene() const { return current_scene_; }

    void setReplay(bool is_replay) { is_replay_ = is_replay; }
    bool isReplay() const { return is_replay_; }

    // 通关数据
    std::vector<LevelInfo> level_info_list_;
};

#endif  // __COCCONTROLLER_H__
#ifndef __COCCONTROLLER_H__
#define __COCCONTROLLER_H__

// 控制程序运行流程的单例类（实际上相当于对 Director 又做了一层封装？）
class CocController {
private:
    // 私有构造函数，防止外部创建实例
    CocController();
    CocController(const CocController&) = delete;

    // 变量示例：
    // 一个记录当前正在运行的场景的变量，0=MainVillage，1=EnemyVillage
    int current_scene_;

public:
    // 获取单例实例，第一次调用时创建
    static CocController* getInstance();

    // 结束游戏
    void endGame();

    // 控制器函数示例：
    // 切换当前场景，此处的暂定机制为：在任何时候的 Scene 栈中最多只有两个 Scene：自己村庄的 Scene、正在攻击的村庄的 Scene
    void changeScene();
};

#endif // __COCCONTROLLER_H__
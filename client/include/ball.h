#ifndef BALL_H
#define BALL_H

#include <memory>     // 智能指针
#include <optional>   // 可选值
#include <functional> // 函数对象
#include <graphics.h> // 图形库
#include <windows.h>  // 定义COLORREF和RGB
#include <cmath>
#include <algorithm>
#include <tchar.h> // For _stprintf_s
#include <string>
#include <random>
#include <vector>
#include <iostream>
#include <sstream>
#include <utility> // for std::pair
#include <memory>  // for std::shared_ptr
#include <chrono>
#include <stdio.h>
#include <random> // 用于随机数引擎
#include <ctime>  // 用于时间种子
#include <string>

#include <game_config.h>
#include <server/ws_mouse.h>
#include <music.h>

extern std::vector<int> randomBallIds;
extern void sendMSG(const MOUSEMSG &msg);

// 定义颜色常量
constexpr COLORREF PURPLE = RGB(128, 0, 128);
constexpr COLORREF ORANGE = RGB(255, 165, 0);
constexpr COLORREF CUSTOM_BROWN = RGB(143, 77, 16);

// 物理参数
constexpr float BALL_FRICTION = 0.98f;
constexpr float TABLE_FRICTION = 0.99f;
constexpr float MIN_VELOCITY = 0.1f;
constexpr float BALL_RADIUS = 9.0f;

// 在物理常量区添加
constexpr float COEFFICIENT_OF_RESTITUTION = 0.95f; // 碰撞恢复系数
constexpr float TANGENT_FRICTION = 0.2f;            // 切向摩擦系数

namespace PoolGame
{

    // 球的类型枚举 (强类型枚举)
    enum class BallType
    {
        White,   // 白球(0号)
        Solid,   // 实色球(1-7号)
        Striped, // 花色球(9-15号)
        Black    // 黑球(8号)
    };

    // 球的状态枚举
    enum class BallState
    {
        OnTable,  // 球在台面上
        Pocketed, // 球已落袋
        InHand    // 白球处于可自由放置状态
    };

    // 游戏状态枚举 (专门用于台球游戏)
    enum class BallGameState
    {
        Aiming,         // 玩家正在瞄准
        Shooting,       // 球杆击球状态
        BallMoving,     // 球正在移动
        Foul,           // 犯规状态
        PlaceCue,       // 需要放置白球
        AssignType,     // 需要分配球类型(分球后)
        GameOver,       // 游戏结束
        WaitingAiming,  // 等待对方瞄准击球
        WaitingPlaceCue // 等待对方放置白球
    };

    // 二维点结构
    struct Point
    {
        float x;
        float y;
        Point(float x = 0, float y = 0) : x(x), y(y) {}
    };

    /**
     * @brief 球类，表示单个台球
     */
    class Ball
    {
    public:
        /**
         * @brief 构造函数
         * @param id 球ID (0-15)
         * @param type 球类型
         * @param x 初始x坐标
         * @param y 初始y坐标
         * @param radius 球半径
         * @param color 球颜色
         */
        Ball(int id, BallType type, float x, float y, float radius, COLORREF color)
            : id_(id), type_(type), state_(BallState::OnTable),
              x_(x), y_(y), vx_(0.0f), vy_(0.0f),
              radius_(radius), color_(color), next_(nullptr)
        {
            // 根据球ID设置默认颜色（如果未指定）
            if (color_ == 0)
            {
                switch (id_)
                {
                case 0:
                    color_ = WHITE;
                    break; // 白球
                case 1:
                    color_ = YELLOW;
                    break; // 1号黄球
                case 2:
                    color_ = BLUE;
                    break; // 2号蓝球
                case 3:
                    color_ = RED;
                    break; // 3号红球
                case 4:
                    color_ = PURPLE;
                    break; // 4号紫球
                case 5:
                    color_ = ORANGE;
                    break; // 5号橙球
                case 6:
                    color_ = GREEN;
                    break; // 6号绿球
                case 7:
                    color_ = CUSTOM_BROWN;
                    break; // 7号棕球
                case 8:
                    color_ = BLACK;
                    break; // 8号黑球
                case 9:
                    color_ = YELLOW;
                    break; // 9号黄球
                case 10:
                    color_ = BLUE;
                    break; // 10号蓝球
                case 11:
                    color_ = RED;
                    break; // 11号红球
                case 12:
                    color_ = PURPLE;
                    break; // 12号紫球
                case 13:
                    color_ = ORANGE;
                    break; // 13号橙球
                case 14:
                    color_ = GREEN;
                    break; // 14号绿球
                case 15:
                    color_ = CUSTOM_BROWN;
                    break; // 15号棕球
                default:
                    color_ = WHITE;
                    break;
                }
            }
        }

        // 获取球属性
        int getId() const { return id_; }
        BallType getType() const { return type_; }
        BallState getState() const { return state_; }
        float getX() const { return x_; }
        float getY() const { return y_; }
        float getVx() const { return vx_; }
        float getVy() const { return vy_; }
        float getRadius() const { return radius_; }
        COLORREF getColor() const { return color_; }
        std::shared_ptr<Ball> getNext() const { return next_; }

        // 设置球属性
        void setState(BallState state) { state_ = state; }
        void setPosition(float x, float y)
        {
            x_ = x;
            y_ = y;
        }
        void setVelocity(float vx, float vy)
        {
            vx_ = vx;
            vy_ = vy;
        }
        void setNext(std::shared_ptr<Ball> next) { next_ = next; }

        /**
         * @brief 更新球的位置
         * @param dt 时间增量(秒)
         */
        void updatePosition(float dt)
        {
            // 只有台面上的球需要更新位置
            if (state_ != BallState::OnTable)
            {
                return;
            }

            // 应用速度
            x_ += vx_ * dt;
            y_ += vy_ * dt;

            // 应用摩擦力
            vx_ *= BALL_FRICTION * TABLE_FRICTION;
            vy_ *= BALL_FRICTION * TABLE_FRICTION;

            // 当速度低于阈值时停止
            if (std::abs(vx_) < MIN_VELOCITY && std::abs(vy_) < MIN_VELOCITY)
            {
                vx_ = 0.0f;
                vy_ = 0.0f;
            }
        }

        /**
         * @brief 绘制球
         */
        void draw() const
        {
            // 只绘制台面上的球
            if (state_ != BallState::OnTable)
            {
                return;
            }

            // 设置填充颜色
            setfillcolor(color_);

            // 绘制球体
            fillcircle(static_cast<int>(x_), static_cast<int>(y_), static_cast<int>(radius_));

            setlinestyle(PS_SOLID, 2);
            setlinecolor(BLACK);
            circle(static_cast<int>(x_), static_cast<int>(y_), static_cast<int>(radius_));

            // 如果是花色球，绘制白色条纹
            if (type_ == BallType::Striped && id_ != 0)
            {
                setfillcolor(WHITE);

                // 主条纹（水平方向）
                int mainWidth = static_cast<int>(radius_ * 1.8f);
                int mainHeight = static_cast<int>(radius_ * 0.5f);
                fillrectangle(
                    static_cast<int>(x_ - mainWidth / 2),
                    static_cast<int>(y_ - mainHeight / 2),
                    static_cast<int>(x_ + mainWidth / 2),
                    static_cast<int>(y_ + mainHeight / 2));

                // 辅助条纹（垂直方向，稍窄）
                int subWidth = static_cast<int>(radius_ * 0.5f);
                int subHeight = static_cast<int>(radius_ * 1.2f);
                fillrectangle(
                    static_cast<int>(x_ - subWidth / 2),
                    static_cast<int>(y_ - subHeight / 2),
                    static_cast<int>(x_ + subWidth / 2),
                    static_cast<int>(y_ + subHeight / 2));
            }

            // 绘制球编号
            // if (id_ > 0)
            // {
            //     settextcolor(id_ <= 8 ? WHITE : BLACK);
            //     setbkmode(TRANSPARENT);
            //     settextstyle(static_cast<int>(radius_ * 0.6f), 0, _T("Arial"));

            //     // 使用std::to_string替代sprintf_s
            //     std::string numStr = std::to_string(id_);
            //     int textWidth = textwidth(numStr.c_str());
            //     int textHeight = textheight(numStr.c_str());

            //     // 计算文本位置（避开条纹区域）
            //     float offsetX = (type_ == BallType::Striped) ? radius_ * 0.4f : 0.0f;
            //     outtextxy(
            //         static_cast<int>(x_ - textWidth / 2 + offsetX),
            //         static_cast<int>(y_ - textHeight / 2),
            //         numStr.c_str());
            // }
        }

        // 获取设置的X和Y
        void setX(float x) { x_ = x; }
        void setY(float y) { y_ = y; }

    private:
        int id_;                     // 球唯一标识
        BallType type_;              // 球类型
        BallState state_;            // 当前状态
        float x_, y_;                // 位置坐标
        float vx_, vy_;              // 速度分量
        float radius_;               // 半径
        COLORREF color_;             // 颜色
        std::shared_ptr<Ball> next_; // 链表下一个节点指针
    };

    /**
     * @brief 台球游戏主类
     */
    class Game
    {
    public:
        // void sendMessageWithClient(const string &message);

        Game()
            : // currentState_(BallGameState::Aiming),
              balls_(nullptr),
              cueBall_(nullptr),
              //   currentPlayer_(0),
              foul_(false),
              isSelfBallPocketed(false),
              firstContactValid_(false),
              firstPocketed_(nullptr),
              isContinueAiming(false),
              isWin(false),
              foul_pocket_8(false),
              is_normal_pocket_8(false)
        {
            // 初始化玩家状态
            for (int i = 0; i < 2; ++i)
            {
                playerTypes_[i] = BallType::Solid; // 初始默认值
                hasAssigned_[i] = false;
                pocketedCount_[i] = 0;
            }

            // 初始化球
            initializeBalls();

            // 初始化玩家
            if (current_player_id == global_user_id)
            {
                currentPlayer_ = 0;
                currentState_ = BallGameState::Aiming;
                cerr << "到您了！请开球" << endl;
            }
            else
            {
                currentPlayer_ = 1;
                currentState_ = BallGameState::WaitingAiming;
                cerr << "对方开球" << endl;
            }
        }

        /**
         * @brief 更新游戏状态
         * @param dt 时间增量(秒)
         */
        void update(float dt)
        {
            bool ballsWereMoving = !allBallsStopped();
            switch (currentState_)
            {
            case BallGameState::BallMoving:
                handleCollisions();      // 先检测碰撞
                updateBallsPosition(dt); // 再更新位置
                checkPockets();

                if (ballsWereMoving || !allBallsStopped())
                {
                    needRedraw_ = true;
                }

                if (allBallsStopped())
                {
                    checkRules();
                    if (!foul_pocket_8 && !is_normal_pocket_8)
                    {
                        if (!foul_)
                        {
                            if (!isContinueAiming)
                            {
                                switchPlayer();
                                isContinueAiming = false;
                            }
                            if (currentPlayer_ == 1)
                            {
                                currentState_ = BallGameState::WaitingAiming;

                                isContinueAiming = false;
                                log("等待对方击球");
                            }
                            else
                            {
                                currentState_ = BallGameState::Aiming;
                                isContinueAiming = false;
                                log("请继续击球");
                            }
                            // currentState_ = BallGameState::Aiming;
                        }
                        else
                        {
                            currentState_ = BallGameState::Foul;
                        }
                    }
                }
                break;

            case BallGameState::Foul:
                handleFoul();
                break;

            case BallGameState::PlaceCue:
                // 放置白球逻辑
                break;

            default:
                break;
            }
        }

    private:
        std::chrono::steady_clock::time_point lastSendTime_;

    public:
        /**
         * @brief 处理用户输入并显示球杆预览
         */
        void handleInput()
        {
            // switch (currentState_) // 这里是处理对手击球的函数
            // {
            // case BallGameState::WaitingAiming:
            //     break;
            //     // handleEnemyAiming();
            // case BallGameState::WaitingPlaceCue:
            //     break;
            //     // handleEnemyPlaceCue();
            // }
            // 先清空消息缓冲区
            while (PeekMessage(NULL, NULL, 0, 0, PM_REMOVE))
            {
            }

            // 调试用，看看WSMouseHit是否有数据
            // if (WSMouseHit())
            // {
            //     // cerr << "收到WSMouseHit消息" << endl;
            // }

            // 处理WS鼠标消息(用于联机)
            while (WSMouseHit())
            {
                // cerr << "WSMouseHit" << endl;
                WSMSG msg = GetWSMouseMsg();
                switch (currentState_)
                {
                case BallGameState::WaitingAiming:
                    handleEnemyAiming(msg);
                    break;
                case BallGameState::WaitingPlaceCue:
                    handleEnemyPlaceCue(msg);
                    break;
                }
            }

            // 然后在有鼠标事件的时候调用
            while (MouseHit())
            {
                MOUSEMSG msg = GetMouseMsg();
                switch (currentState_)
                {
                case BallGameState::Aiming:
                    handleAimingState(msg);
                    break;

                case BallGameState::PlaceCue:
                    handlePlaceCueState(msg); // 新增处理函数
                    break;
                    // default:
                    //     log("未处理的状态: " + std::to_string(static_cast<int>(currentState_)));
                    //     break;
                }
                static int counter = 0;
                if (++counter % 5 == 0)
                {
                    FlushBatchDraw();
                }
            }
        }

        void handleEnemyAiming(const WSMSG &msg)
        {

            // 计算角度和力量
            float dx = cueBall_->getX() - msg.x;
            float dy = cueBall_->getY() - msg.y;
            currentCueAngle_ = atan2(dy, dx);
            currentPower_ = std::min(sqrt(dx * dx + dy * dy) / 10.0f, 20.0f) * 130;

            // 点击处理
            if (msg.uMsg == 513)
            {
                cerr << "点击捕获 X:" << msg.x << " Y:" << msg.x << endl;

                applyForceToCue(currentPower_, currentCueAngle_);
                currentState_ = BallGameState::BallMoving;
                log("击球成功 力量:" + std::to_string(currentPower_));
            }

            requestRedraw();
        }

        void handleEnemyPlaceCue(const WSMSG &msg)
        {
            setTempCuePosition(msg.x, msg.y);

            if (msg.uMsg == 513)
            {
                if (confirmPlaceCue())
                {
                    currentState_ = BallGameState::WaitingAiming;
                    log("白球放置成功");
                }
            }

            requestRedraw();
        }
        void handleAimingState(const MOUSEMSG &msg)
        {
            // 获取当前时间
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendTime_);

            // 计算最小间隔（1000ms / 6次 ≈ 166ms）
            constexpr int MIN_INTERVAL_MS = 1000 / 6; // 精确值 166ms

            // 强制发送条件：鼠标按下 或 距离上次发送超过1秒
            bool forceSend = (elapsed.count() >= MIN_INTERVAL_MS);

            // 计算角度和力量
            float dx = cueBall_->getX() - msg.x;
            float dy = cueBall_->getY() - msg.y;
            currentCueAngle_ = atan2(dy, dx);
            currentPower_ = std::min(sqrt(dx * dx + dy * dy) / 10.0f, 20.0f) * 130;

            // 点击处理
            if (msg.uMsg == WM_LBUTTONDOWN)
            {
                cerr << "点击捕获 X:" << msg.x << " Y:" << msg.y << endl;
                applyForceToCue(currentPower_, currentCueAngle_);
                currentState_ = BallGameState::BallMoving;
                log("击球成功 力量:" + std::to_string(currentPower_));
                forceSend = true; // 鼠标点击时强制发送消息
            }

            if (forceSend)
            {
                sendMSG(msg);
                lastSendTime_ = now; // 更新时间戳
            }

            requestRedraw();
        }

        void handlePlaceCueState(const MOUSEMSG &msg)
        {
            // 获取当前时间
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendTime_);

            // 计算最小间隔（1000ms / 6次 ≈ 166ms）
            constexpr int MIN_INTERVAL_MS = 1000 / 6; // 精确值 166ms

            // 强制发送条件：鼠标按下 或 距离上次发送超过1秒
            bool forceSend = (elapsed.count() >= MIN_INTERVAL_MS);

            setTempCuePosition(msg.x, msg.y);

            if (msg.uMsg == WM_LBUTTONDOWN)
            {
                if (confirmPlaceCue())
                {
                    currentState_ = BallGameState::Aiming;
                    log("白球放置成功");
                }
                forceSend = true; // 鼠标点击时强制发送消息
            }

            if (forceSend)
            {
                sendMSG(msg);
                lastSendTime_ = now; // 更新时间戳
            }

            requestRedraw();
        }

        /**x
         * @brief 对白球施加力
         * @param power 力量大小
         * @param angle 击球角度(弧度)
         */
        void applyForceToCue(float power, float angle)
        {
            if (currentState_ != BallGameState::Aiming && currentState_ != BallGameState::WaitingAiming)
            {
                return;
            }

            // 计算速度分量
            float vx = 1.5 * power * std::cos(angle);
            float vy = 1.5 * power * std::sin(angle);

            cueBall_->setVelocity(vx, vy);
            currentState_ = BallGameState::BallMoving;
            firstContactValid_ = false;
            if (setting_ball_sound)
            {
                AudioManager::play(CUEHIT_SOUND, "", false, 500 + (power / 3600 * 500));
            }
        }

        // 生成一个随机的 ballId 数组（包含 0-6 和 8-14）
        std::vector<int> generateRandomBallIds()
        {
            std::vector<int> ballIds;

            // 添加所有有效的球 ID（不包含 7，因为 8 号球固定）
            for (int i = 1; i <= 7; ++i)
                ballIds.push_back(i);

            for (int i = 9; i <= 15; ++i)
                ballIds.push_back(i);

            // 使用随机种子进行洗牌
            std::shuffle(ballIds.begin(), ballIds.end(), std::default_random_engine(static_cast<unsigned int>(time(0))));

            return ballIds;
        }

        /**
         * @brief 初始化所有球的位置
         */
        void initializeBalls()
        {
            // 清空现有球
            freeBalls(balls_);
            balls_ = nullptr;
            cueBall_ = nullptr;

            // 球桌边界参数
            const float tableLeft = 159.0f;
            const float tableRight = 636.0f;
            const float tableTop = 109.0f;
            const float tableBottom = 347.0f;
            const float tableCenterY = (tableTop + tableBottom) / 2.0f; // 垂直中心

            const float tableWidth = tableRight - tableLeft;

            // 物理常量
            const float radius = BALL_RADIUS;
            const float diameter = 2 * radius;
            const float sqrt3 = 1.7320508f; // √3

            // 三角形球堆参数（顶点朝左，横向放置）
            const float rackRightEdge = tableRight - 150.0f; // 球堆距离右边界 150 像素
            const float rackX = rackRightEdge - radius;      // 三角形最左侧球的 X 坐标
            const float rackY = tableCenterY;                // 三角形中心 Y 坐标

            // 创建白球（保持原位）
            cueBall_ = std::make_shared<Ball>(0, BallType::White,
                                              tableWidth / 4.0f + tableLeft, rackY,
                                              BALL_RADIUS, WHITE);
            addBall(cueBall_);

            // 球间距计算（行间距作用在 `x` 方向，列间距作用在 `y` 方向）
            const float rowSpacing = BALL_RADIUS * 2.0f;
            const float colSpacing = BALL_RADIUS * sqrt3;

            // std::vector<int> randomBallIds = generateRandomBallIds();
            // 这里在client_server中处理

            // 预分配15颗目标球（1-15号）
            std::vector<std::shared_ptr<Ball>> targetBalls(15);

            // --- 固定位置球 ---
            // 1号球（顶点）
            int ballIdPoint = randomBallIds[0]; // 从随机数组中获取球 ID
            BallType typePoint = (ballIdPoint <= 7) ? BallType::Solid : BallType::Striped;

            targetBalls[0] = std::make_shared<Ball>(ballIdPoint, typePoint, rackX, rackY, BALL_RADIUS, 0);

            // 8号球（固定在第三行第二列）
            targetBalls[7] = std::make_shared<Ball>(8, BallType::Black,
                                                    rackX + 2 * rowSpacing, rackY,
                                                    BALL_RADIUS, BLACK);

            // --- 其他球分配 ---

            for (int i = 1; i < 15; ++i)
            {
                int ballId = (i <= 7) ? randomBallIds[i] : randomBallIds[i - 1]; // 从随机数组中获取球 ID
                BallType type = (ballId <= 7) ? BallType::Solid : BallType::Striped;
                if (i == 7)
                    continue; // 跳过 8 号球（已放置）

                float x, y;
                int row = getRowForPosition(i);
                int col = getColInRow(i, row);

                x = rackX + (row - 1) * rowSpacing;
                y = rackY - (col - (row - 1) / 2.0f) * colSpacing;

                // **确保1-7号球是纯色，9-15号球是花色**
                // int ballId;
                // BallType type;

                targetBalls[i] = std::make_shared<Ball>(ballId, type, x, y, BALL_RADIUS, 0);
            }

            // 添加到链表
            for (auto &ball : targetBalls)
            {
                addBall(ball);
            }
        }

        // 修改后的辅助函数：计算球所在的行（1-5）
        int getRowForPosition(int ballIndex)
        {
            if (ballIndex == 0)
                return 1; // 第一行
            if (ballIndex <= 2)
                return 2; // 第二行
            if (ballIndex <= 4)
                return 3; // 第三行
            if (ballIndex <= 9)
                return 4; // 第四行
            return 5;     // 第五行
        }

        // 修改后的辅助函数：计算球在该行的列序号（从 0 开始）
        int getColInRow(int ballIndex, int row)
        {
            // 确保8号球固定位置
            if (ballIndex == 7)
                return 1; // 8 号球固定在第三行第二列

            switch (row)
            {
            case 1:
                return 0; // 第一行（1 号球）
            case 2:
                return ballIndex - 1; // 第二行（2 3）
            case 3:
                if (ballIndex == 7)
                {
                    return 1; // 8号球固定在第二列
                }
                else if (ballIndex == 3)
                {
                    return ballIndex - 3; // 其他球的位置
                }
                else if (ballIndex == 4)
                {
                    return ballIndex - 2;
                }
            case 4:
                if (ballIndex < 7)
                {
                    return ballIndex - 5; // 第四行
                }
                else
                {
                    return ballIndex - 6;
                }

            case 5:
                return ballIndex - 10; // 第五行
            default:
                return 0;
            }
        }

        /**
         * @brief 添加球到链表
         * @param ball 要添加的球
         */
        void addBall(std::shared_ptr<Ball> ball)
        {
            if (!balls_)
            {
                balls_ = ball;
            }
            else
            {
                auto last = balls_;
                while (last->getNext())
                {
                    last = last->getNext();
                }
                last->setNext(ball);
            }
        }

        /**
         * @brief 根据ID查找球
         * @param id 球ID
         * @return 找到的球指针，未找到返回nullptr
         */
        std::shared_ptr<Ball> findBall(int id) const
        {
            auto ball = balls_;
            while (ball)
            {
                if (ball->getId() == id)
                {
                    return ball;
                }
                ball = ball->getNext();
            }
            return nullptr;
        }

        /**
         * @brief 检查所有球是否静止
         * @return 所有球静止返回true
         */
        bool allBallsStopped() const
        {
            auto ball = balls_;
            while (ball)
            {
                if (ball->getState() == BallState::OnTable)
                {
                    if (std::abs(ball->getVx()) > MIN_VELOCITY || std::abs(ball->getVy()) > MIN_VELOCITY)
                    {
                        return false;
                    }
                }
                ball = ball->getNext();
            }
            return true;
        }

        /**
         * @brief 检查球是否落袋
         */
        void checkPockets()
        {
            auto ball = balls_;
            while (ball)
            {
                if (ball->getState() == BallState::OnTable)
                {
                    float x = ball->getX();
                    float y = ball->getY();

                    // 检查6个袋口
                    if (isInPocket(x, y))
                    {
                        ball->setState(BallState::Pocketed);
                        if (setting_ball_sound)
                        {
                            AudioManager::play(POCKET_SOUND, "",false,500);
                        }

                        // 记录第一个落袋的球
                        if (!firstPocketed_ && ball->getId() != 0)
                        {
                            firstPocketed_ = ball;
                        }

                        // 如果是白球
                        if (ball->getId() == 0)
                        {
                            foul_ = true;
                            firstPocketed_ = nullptr;
                        }

                        // 更新玩家进球数
                        if (ball->getId() != 0)
                        {
                            if (ball->getType() == playerTypes_[currentPlayer_])
                            {
                                pocketedCount_[currentPlayer_]++;
                                isContinueAiming = true;
                            }
                            else
                            {
                                pocketedCount_[1 - currentPlayer_]++;
                            }
                        }
                    }
                }
                ball = ball->getNext();
            }
        }

        /**
         * @brief 检查游戏规则
         */
        void checkRules()
        {
            // 检查8号球是否提前进袋
            auto eightBall = findBall(8);
            if (eightBall && eightBall->getState() == BallState::Pocketed)
            {
                // 检查是否合法击打8号球
                if (!isLegal8BallPocket())
                {
                    // foul_ = true;
                    foul_pocket_8 = true;
                    if (currentPlayer_ == 0)
                    {
                        isWin = false;
                    }
                    else if (currentPlayer_ == 1)
                    {
                        isWin = true;
                    }
                    currentState_ = BallGameState::GameOver;

                    cerr << "检查到了黑8提前进袋(checkRules中)" << endl;
                    return; // 直接返回，跳过后续检查
                }
                else
                {
                    is_normal_pocket_8 = true;
                    cerr << "游戏正常结束" << endl;
                    if (currentPlayer_ == 0)
                    {
                        isWin = true;
                    }
                    else if (currentPlayer_ == 1)
                    {
                        isWin = false;
                    }
                    currentState_ = BallGameState::GameOver;

                    return;
                }
            }

            // if (firstPocketed_ && firstPocketed_->getType() != playerTypes_[currentPlayer_])
            // {
            //     foul_ = true;
            // }

            // 检查是否需要分球
            if (firstPocketed_ && !hasAssigned_[currentPlayer_])
            {
                currentState_ = BallGameState::AssignType;
                // 根据 firstPocketed_ 的ID设置玩家类型
                if (firstPocketed_->getId() <= 7)
                {
                    playerTypes_[currentPlayer_] = BallType::Solid;
                }
                else if (firstPocketed_->getId() >= 9)
                {
                    playerTypes_[currentPlayer_] = BallType::Striped;
                }
                hasAssigned_[currentPlayer_] = true;
                // 对手自动分配另一类型
                playerTypes_[1 - currentPlayer_] =
                    (playerTypes_[currentPlayer_] == BallType::Solid) ? BallType::Striped : BallType::Solid;
                hasAssigned_[1 - currentPlayer_] = true;
                isContinueAiming = true; // 继续瞄准
                switch (playerTypes_[0])
                {
                case BallType::Solid:
                    log("已分球，您需要击打实心球！");
                    break;
                case BallType::Striped:
                    log("已分球，您需要击打花色球！");
                    break;
                default:
                    break;
                }
                return;
            }

            // 检查是否犯规
            if (foul_)
            {
                currentState_ = BallGameState::Foul;
                return;
            }

            // 检查游戏是否结束
            // if (checkGameOver())
            // {
            //     cerr << "游戏结束" << endl;
            //     if (currentPlayer_ = 0)
            //     {
            //         isWin = true;
            //     }
            //     else if (currentPlayer_ = 1)
            //     {
            //         isWin = false;
            //     }
            //     currentState_ = BallGameState::GameOver;

            //     return;
            // }

            // // 如果没有犯规，切换到下一个玩家
            // if (!foul_)
            // {
            //     if (!isContinueAiming)
            //     {
            //         switchPlayer();
            //         isContinueAiming = false;
            //     }
            // }
        }

        /**
         * @brief 检查球是否碰库
         * @param ball 要检查的球
         * @return 碰库返回true
         */
        bool checkBallHitRail(const std::shared_ptr<Ball> &ball) const
        {
            if (ball->getState() != BallState::OnTable)
            {
                return false;
            }

            float x = ball->getX();
            float y = ball->getY();
            float radius = ball->getRadius();

            // 球桌边界 (159,109)到(636,347)
            const float left = 159.0f + radius;
            const float right = 636.0f - radius;
            const float top = 109.0f + radius;
            const float bottom = 347.0f - radius;

            // 检查是否碰到边界
            return (x <= left || x >= right || y <= top || y >= bottom);
        }

        /**
         * @brief 绘制游戏场景
         */
        void draw() const
        {
            if (!needRedraw_)
                return;

            // 绘制所有球
            auto ball = balls_;
            while (ball)
            {
                // 如果是放置白球状态且是白球，不绘制（用临时位置代替）
                if (!(currentState_ == BallGameState::PlaceCue && ball.get() == cueBall_.get()) && !(currentState_ == BallGameState::WaitingPlaceCue && ball.get() == cueBall_.get()))
                {
                    ball->draw();
                }
                ball = ball->getNext();
            }

            // 绘制临时白球（放置状态时）
            if (currentState_ == BallGameState::PlaceCue || currentState_ == BallGameState::WaitingPlaceCue)
            {
                if (!(currentState_ == BallGameState::WaitingPlaceCue && !isValidCuePosition(tempCueX_, tempCueY_)))
                {
                    setfillcolor(WHITE);
                    // 有效位置用白色，无效位置用半透明红色
                    if (!isValidCuePosition(tempCueX_, tempCueY_))
                    {
                        setfillcolor(RGB(255, 0, 0));
                        settextcolor(RED);
                        outtextxy(static_cast<int>(tempCueX_) - 20,
                                  static_cast<int>(tempCueY_) - 40,
                                  _T("无效位置"));
                    }
                    if (currentState_ == BallGameState::WaitingPlaceCue)
                    {
                        setfillcolor(RGB(156, 156, 156));
                    }
                    fillcircle(static_cast<int>(tempCueX_),
                               static_cast<int>(tempCueY_),
                               static_cast<int>(BALL_RADIUS));
                }
            }

            // 绘制已落袋的球
            drawPocketedBalls();

            // 如果是瞄准状态，绘制球杆
            if (currentState_ == BallGameState::Aiming || currentState_ == BallGameState::WaitingAiming)
            {
                drawCue();
            }

            // 绘制状态栏
            drawStatusBar();

            needRedraw_ = false;
        }

        /**
         * @brief 绘制所有球
         */
        void drawBalls() const
        {
            auto ball = balls_;
            while (ball)
            {
                ball->draw();
                ball = ball->getNext();
            }
        }

        /**
         * @brief 绘制已落袋的球
         */
        void drawPocketedBalls() const
        {
            // 落袋区位置
            const int pocketedX = 50;
            const int pocketedY = 400;
            const int spacing = 30;

            int count = 0;
            auto ball = balls_;
            while (ball)
            {
                if (ball->getState() == BallState::Pocketed)
                {
                    // 保存原始位置
                    float oldX = ball->getX();
                    float oldY = ball->getY();

                    // 使用 const_cast 临时修改位置
                    const_cast<Ball *>(ball.get())->setX(pocketedX + (count % 5) * spacing);
                    const_cast<Ball *>(ball.get())->setY(pocketedY + (count / 5) * spacing);

                    ball->draw();

                    // 恢复原始位置
                    const_cast<Ball *>(ball.get())->setX(oldX);
                    const_cast<Ball *>(ball.get())->setY(oldY);
                    count++;
                }
                ball = ball->getNext();
            }
        }

        /**
         * @brief 设置临时白球位置（跟随鼠标）
         * @param x 鼠标x坐标
         * @param y 鼠标y坐标
         */
        void setTempCuePosition(int x, int y)
        {
            tempCueX_ = static_cast<float>(x);
            tempCueY_ = static_cast<float>(y);
            needRedraw_ = true;
        }

        /**
         * @brief 确认放置白球
         * @return 放置是否成功
         */
        bool confirmPlaceCue()
        {
            if (isValidCuePosition(tempCueX_, tempCueY_))
            {
                cueBall_->setPosition(tempCueX_, tempCueY_);
                cueBall_->setState(BallState::OnTable);
                if (currentPlayer_ == 0)
                {
                    currentState_ = BallGameState::Aiming;
                }
                else
                {
                    currentState_ = BallGameState::WaitingAiming;
                }
                return true;
            }
            return false;
        }

        /**
         * @brief 预测白球路径上的第一次碰撞
         * @param startX 起始X坐标
         * @param startY 起始Y坐标
         * @param angle 运动角度
         * @param maxDistance 最大预测距离
         * @return 返回碰撞点坐标和碰撞球指针（若无碰撞返回nullptr）
         */
        std::pair<Point, std::shared_ptr<Ball>> predictFirstCollision(
            float startX, float startY, float angle, float maxDistance) const
        {
            const float step = 2.0f; // 检测步长
            float distance = 0;

            while (distance < maxDistance)
            {
                distance += step;
                float currentX = startX + distance * cos(angle);
                float currentY = startY + distance * sin(angle);

                // 检查是否出界
                if (!isInTable(currentX, currentY))
                {
                    return {Point{-1, -1}, nullptr}; // 无效坐标表示无碰撞
                }

                // 检查与其他球的碰撞
                auto ball = balls_;
                while (ball)
                {
                    if (ball->getState() == BallState::OnTable && ball.get() != cueBall_.get())
                    {
                        float dx = currentX - ball->getX();
                        float dy = currentY - ball->getY();
                        float dist = sqrt(dx * dx + dy * dy);

                        if (dist < (cueBall_->getRadius() + ball->getRadius()))
                        {
                            // 计算精确碰撞点（后退半步避免浮点误差）
                            float backStep = step * 0.5f;
                            return {
                                Point{
                                    currentX - backStep * cos(angle),
                                    currentY - backStep * sin(angle)},
                                ball};
                        }
                    }
                    ball = ball->getNext();
                }
            }
            return {Point{-1, -1}, nullptr};
        }

        // 辅助方法：检查坐标是否在球桌内
        bool isInTable(float x, float y) const
        {
            return (x > 159 + BALL_RADIUS && x < 636 - BALL_RADIUS &&
                    y > 109 + BALL_RADIUS && y < 347 - BALL_RADIUS);
        }

        /**
         * @brief 绘制球杆（带力量预览）
         */
        void drawCue() const
        {
            if (!cueBall_ || cueBall_->getState() != BallState::OnTable)
                return;

            // 球杆参数
            const float cueLength = 150.0f;
            const float maxPredictLength = 500.0f;            // 最大预测长度
            const float powerRatio = currentPower_ / 1300.0f; // 力量比例（0~1）

            // 计算球杆端点
            float endX = cueBall_->getX() - cueLength * cos(currentCueAngle_);
            float endY = cueBall_->getY() - cueLength * sin(currentCueAngle_);

            // 绘制球杆主体
            setlinestyle(PS_SOLID, 3);
            setlinecolor(RGB(200, 150, 50)); // 木质颜色
            line(cueBall_->getX(), cueBall_->getY(), endX, endY);

            // 预测碰撞
            auto [collisionPoint, hitBall] = predictFirstCollision(
                cueBall_->getX() + cos(currentCueAngle_) * cueBall_->getRadius(),
                cueBall_->getY() + sin(currentCueAngle_) * cueBall_->getRadius(),
                currentCueAngle_,
                maxPredictLength);

            // 绘制力量指示条（在球杆旁边）
            float indicatorX = cueBall_->getX() + 30 * sin(currentCueAngle_);
            float indicatorY = cueBall_->getY() - 30 * cos(currentCueAngle_);

            setlinestyle(PS_SOLID, 5);
            setlinecolor(RGB(255, 0, 0)); // 红色力量条
            if (currentState_ == BallGameState::Aiming)
            {
                line(indicatorX,
                     indicatorY,
                     indicatorX + 50 * powerRatio * cos(currentCueAngle_),
                     indicatorY + 50 * powerRatio * sin(currentCueAngle_));
            }

            // 绘制球杆头部（与白球接触的部分）
            setfillcolor(RGB(240, 240, 240));
            fillcircle(cueBall_->getX(), cueBall_->getY(), 3);

            // 绘制预测线
            setlinestyle(PS_DASH, 1);
            setlinecolor(RGB(100, 100, 100));

            if (currentState_ == BallGameState::Aiming)
            {
                if (collisionPoint.x >= 0)
                { // 有碰撞
                  // 绘制到碰撞点的预测线
                    line(cueBall_->getX() + cos(currentCueAngle_) * cueBall_->getRadius(),
                         cueBall_->getY() + sin(currentCueAngle_) * cueBall_->getRadius(),
                         collisionPoint.x,
                         collisionPoint.y);
                    // 在碰撞点绘制标记
                    // setfillcolor(RGB(255, 255, 0)); // 黄色标记
                    circle(collisionPoint.x, collisionPoint.y, cueBall_->getRadius());
                    // fillcircle(collisionPoint.x, collisionPoint.y, 3);
                }
                else
                { // 无碰撞
                    // 只绘制短距离预测线（不画出球桌）
                    float safeDistance = std::min(100.0f, maxPredictLength);
                    line(cueBall_->getX() + cos(currentCueAngle_) * cueBall_->getRadius(),
                         cueBall_->getY() + sin(currentCueAngle_) * cueBall_->getRadius(),
                         cueBall_->getX() + cos(currentCueAngle_) * (cueBall_->getRadius() + safeDistance),
                         cueBall_->getY() + sin(currentCueAngle_) * (cueBall_->getRadius() + safeDistance));
                }
            }
        }

        /**
         * @brief 绘制状态栏
         */
        void drawStatusBar() const
        {
            // 绘制标题栏文字
            set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(BLACK);
            string state_text;
            switch (currentState_)
            {
            case BallGameState::Aiming:
                if (!hasAssigned_[currentPlayer_])
                {
                    state_text = "请瞄准击球 - 未分球";
                }
                else if (playerTypes_[currentPlayer_] == BallType::Solid)
                {
                    state_text = "请瞄准击球 - 实心球";
                }
                else if (playerTypes_[currentPlayer_] == BallType::Striped)
                {
                    state_text = "请瞄准击球 - 花色球";
                }
                break;
            case BallGameState::WaitingAiming:
                state_text = "等待对方击球...";
                break;
            case BallGameState::PlaceCue:
                state_text = "请放置白球";
                break;
            case BallGameState::WaitingPlaceCue:
                state_text = "等待对方放置白球...";
                break;
            case BallGameState::BallMoving:
                state_text = "球在运动中...";
                break;
            }
            int state_text_x = 281.8 + (236 - textwidth(state_text.c_str())) / 2;
            outtextxy(state_text_x, 30, state_text.c_str());
        }

        // 获取游戏状态
        BallGameState getState() const { return currentState_; }
        int getCurrentPlayer() const { return currentPlayer_; }

        void log(const std::string &message) const
        {
            std::cerr << "[Game] " << message << std::endl;
        }

        void logMouseEvent(const MOUSEMSG &msg) const
        {
            if (isLogMouseEventInGame)
            {
                std::ostringstream oss;
                oss << "Mouse event: "
                    << "X=" << msg.x << " Y=" << msg.y
                    << " Btn=" << msg.uMsg << " Wheel=" << msg.wheel;
                log(oss.str());
            }
        }

    private:
        std::thread gameThread;
        std::atomic<bool> gameRunning{false};

    public:
        void startGameThread()
        {
            gameRunning = true;
            gameThread = std::thread([this]()
                                     {
            while(gameRunning) {
                this->update(0.008f); // 120fps的帧间隔
                std::this_thread::sleep_for(std::chrono::milliseconds(8));
            } });
        }

        void stopGameThread()
        {
            gameRunning = false;
            if (gameThread.joinable())
            {
                gameThread.join();
            }
        }

        bool getIsWin()
        {
            return isWin;
        }
        ~Game()
        {
            stopGameThread();
        }

    private:
        /**
         * @brief 更新所有球的位置
         * @param dt 时间增量(秒)
         */
        void updateBallsPosition(float dt)
        {
            auto ball = balls_;
            while (ball)
            {
                ball->updatePosition(dt);
                ball = ball->getNext();
            }
        }

        /**
         * @brief 处理所有碰撞
         */
        /**
         * @brief 改进的碰撞处理主函数
         */
        void handleCollisions()
        {
            // 先处理边界碰撞
            auto ball = balls_;
            while (ball)
            {
                handleWallCollision(ball);
                ball = ball->getNext();
            }

            // 球与球碰撞（两阶段检测）
            auto ball1 = balls_;
            while (ball1)
            {
                auto ball2 = ball1->getNext();
                while (ball2)
                {
                    if (checkBallCollision(ball1, ball2))
                    {
                        // 第一阶段：离散检测
                        handleBallCollision(ball1, ball2);
                        resolvePenetration(ball1, ball2);

                        // 第二阶段：连续检测（仅高速球）
                        if (isHighVelocity(ball1) || isHighVelocity(ball2))
                        {
                            handleContinuousCollision(ball1, ball2);
                        }
                    }
                    ball2 = ball2->getNext();
                }
                ball1 = ball1->getNext();
            }
        }

        // 判断球是否高速移动
        bool isHighVelocity(const std::shared_ptr<Ball> &ball) const
        {
            float speedSq = ball->getVx() * ball->getVx() + ball->getVy() * ball->getVy();
            return speedSq > 100.0f; // 速度阈值10^2
        }

        // 在resolvePenetration中添加速度修正
        void resolvePenetration(std::shared_ptr<Ball> a, std::shared_ptr<Ball> b)
        {
            const float minDist = a->getRadius() + b->getRadius();
            const float dx = b->getX() - a->getX();
            const float dy = b->getY() - a->getY();
            const float dist = std::hypot(dx, dy);

            if (dist < minDist)
            {
                const float penetration = minDist - dist;
                const float nx = dx / dist;
                const float ny = dy / dist;

                // 位置修正
                a->setPosition(a->getX() - nx * penetration * 0.5f, a->getY() - ny * penetration * 0.5f);
                b->setPosition(b->getX() + nx * penetration * 0.5f, b->getY() + ny * penetration * 0.5f);

                // 速度修正（防止粘滞）
                const float approachSpeed = (b->getVx() - a->getVx()) * nx + (b->getVy() - a->getVy()) * ny;
                if (approachSpeed < 0)
                {
                    const float delta = -approachSpeed * 0.5f;
                    a->setVelocity(a->getVx() + delta * nx, a->getVy() + delta * ny);
                    b->setVelocity(b->getVx() - delta * nx, b->getVy() - delta * ny);
                }
            }
        }

        /**
         * @brief 检查两个球是否碰撞
         * @param a 第一个球
         * @param b 第二个球
         * @return 碰撞返回true
         */
        bool checkBallCollision(const std::shared_ptr<Ball> &a, const std::shared_ptr<Ball> &b)
        {
            if (a->getState() != BallState::OnTable || b->getState() != BallState::OnTable)
            {
                return false;
            }

            float dx = b->getX() - a->getX();
            float dy = b->getY() - a->getY();
            float distance = std::sqrt(dx * dx + dy * dy);

            return distance < (a->getRadius() + b->getRadius());
        }

        /**
         * @brief 处理两个球的碰撞
         * @param a 第一个球
         * @param b 第二个球
         */
        void handleBallCollision(const std::shared_ptr<Ball> &a, const std::shared_ptr<Ball> &b)
        {
            // 计算碰撞法线
            float nx = b->getX() - a->getX();
            float ny = b->getY() - a->getY();
            float distance = std::sqrt(nx * nx + ny * ny);
            nx /= distance;
            ny /= distance;

            // 计算相对速度
            float vx = b->getVx() - a->getVx();
            float vy = b->getVy() - a->getVy();

            // 计算碰撞冲量
            float impulse = 2.0f * (vx * nx + vy * ny) / (1.0f + 1.0f); // 假设质量相同

            // 更新速度
            a->setVelocity(a->getVx() + impulse * nx, a->getVy() + impulse * ny);
            b->setVelocity(b->getVx() - impulse * nx, b->getVy() - impulse * ny);

            if (setting_ball_sound && distance <= BALL_RADIUS)
            {
                float collisionStrength = std::sqrt(vx * vx + vy * vy);
                if (collisionStrength > 2000)
                {
                    collisionStrength = 2000;
                }
                AudioManager::play(BALLHIT_SOUND, "", false, collisionStrength/2);
            }

            // 标记第一次接触
            if (!firstContactValid_)
            {
                firstContactValid_ = true;

                // 获取碰撞球（排除白球）
                std::shared_ptr<Ball> hitBall = (a == cueBall_) ? b : a;

                // --- 规则验证逻辑 ---
                // 情况1：尚未分球（开球阶段）
                if (!hasAssigned_[currentPlayer_])
                {
                    // if (hitBall->getType() == BallType::Black)
                    // {
                    //     foul_ = true; // 开球直接碰黑球犯规
                    //     log("犯规：开球首次接触黑球");
                    // }
                }
                // 情况2：已分球
                else
                {
                    BallType playerType = playerTypes_[currentPlayer_];

                    // 检查是否碰对手球或黑球
                    if ((hitBall->getType() != playerType) &&
                        (hitBall->getType() != BallType::Black))
                    {
                        foul_ = true;
                        log("犯规：首次接触对手球");
                    }

                    // 检查是否过早碰黑球
                    if (hitBall->getType() == BallType::Black)
                    {
                        // 验证是否已打完己方球
                        bool allPocketed = true;
                        auto ball = balls_;
                        while (ball)
                        {
                            if (ball->getState() == BallState::OnTable &&
                                ball->getType() == playerType)
                            {
                                allPocketed = false;
                                break;
                            }
                            ball = ball->getNext();
                        }

                        if (!allPocketed)
                        {
                            foul_ = true;
                            log("犯规：未打完己方球时接触黑球");
                        }
                    }
                }
            }
        }

        /**
         * @brief 处理球与边界的碰撞
         * @param ball 要处理的球
         */
        void handleWallCollision(const std::shared_ptr<Ball> &ball)
        {
            if (ball->getState() != BallState::OnTable)
            {
                return;
            }

            float x = ball->getX();
            float y = ball->getY();
            float radius = ball->getRadius();

            // 球桌边界 (159,109)到(636,347)
            const float left = 159.0f + radius;
            const float right = 636.0f - radius;
            const float top = 109.0f + radius;
            const float bottom = 347.0f - radius;

            // 检查左右边界
            if (x < left)
            {
                ball->setX(left);
                ball->setVelocity(-ball->getVx() * 0.8f, ball->getVy());
                float velocityHit = sqrt(ball->getVx() * ball->getVx() + ball->getVy() * ball->getVy());
                if (setting_ball_sound)
                {
                    if(velocityHit>1000){
                        velocityHit = 1000;
                    }
                    AudioManager::play(WALLHIT_SOUND, "", false, velocityHit/2);
                }
            }
            else if (x > right)
            {
                ball->setX(right);
                ball->setVelocity(-ball->getVx() * 0.8f, ball->getVy());
                float velocityHit = sqrt(ball->getVx() * ball->getVx() + ball->getVy() * ball->getVy());
                if (setting_ball_sound)
                {
                    if(velocityHit>1000){
                        velocityHit = 1000;
                    }
                    AudioManager::play(WALLHIT_SOUND, "", false, velocityHit/2);
                }
            }

            // 检查上下边界
            if (y < top)
            {
                ball->setY(top);
                ball->setVelocity(ball->getVx(), -ball->getVy() * 0.8f);
                float velocityHit = sqrt(ball->getVx() * ball->getVx() + ball->getVy() * ball->getVy());
                if (setting_ball_sound)
                {
                    if(velocityHit>1000){
                        velocityHit = 1000;
                    }
                    AudioManager::play(WALLHIT_SOUND, "", false, velocityHit/2);
                }
            }
            else if (y > bottom)
            {
                ball->setY(bottom);
                ball->setVelocity(ball->getVx(), -ball->getVy() * 0.8f);
                float velocityHit = sqrt(ball->getVx() * ball->getVx() + ball->getVy() * ball->getVy());
                if (setting_ball_sound)
                {
                    if(velocityHit>1000){
                        velocityHit = 1000;
                    }
                    AudioManager::play(WALLHIT_SOUND, "", false, velocityHit/2);
                }
            }
        }

        // 添加连续碰撞检测方法
        void handleContinuousCollision(std::shared_ptr<Ball> a, std::shared_ptr<Ball> b)
        {
            // 计算相对速度
            float relVx = b->getVx() - a->getVx();
            float relVy = b->getVy() - a->getVy();

            // 计算相对位置
            float dx = b->getX() - a->getX();
            float dy = b->getY() - a->getY();

            // 计算碰撞时间（二次方程求根）
            float aCoeff = relVx * relVx + relVy * relVy;
            float bCoeff = 2 * (dx * relVx + dy * relVy);
            float cCoeff = dx * dx + dy * dy - (a->getRadius() + b->getRadius()) * (a->getRadius() + b->getRadius());

            float discriminant = bCoeff * bCoeff - 4 * aCoeff * cCoeff;
            if (discriminant < 0)
                return; // 无实数解

            float t = (-bCoeff - sqrt(discriminant)) / (2 * aCoeff);
            if (t < 0 || t > 1)
                return; // 不在当前帧内碰撞

            // 回退到碰撞时刻
            a->setPosition(a->getX() - a->getVx() * t, a->getY() - a->getVy() * t);
            b->setPosition(b->getX() - b->getVx() * t, b->getY() - b->getVy() * t);

            // 处理碰撞
            handleBallCollision(a, b);

            // 前进剩余时间
            a->setPosition(a->getX() + a->getVx() * t, a->getY() + a->getVy() * t);
            b->setPosition(b->getX() + b->getVx() * t, b->getY() + b->getVy() * t);
        }
        // // 改进的连续碰撞检测
        // void handleContinuousCollision(std::shared_ptr<Ball> a, std::shared_ptr<Ball> b)
        // {
        //     // 相对位置和速度
        //     float relX = b->getX() - a->getX();
        //     float relY = b->getY() - a->getY();
        //     float relVelX = b->getVx() - a->getVx();
        //     float relVelY = b->getVy() - a->getVy();

        //     float radiusSum = a->getRadius() + b->getRadius();
        //     float minDistance = radiusSum * 0.95f; // 加入5%的容差

        //     // 精确求解碰撞时间
        //     float aCoeff = relVelX * relVelX + relVelY * relVelY;
        //     float bCoeff = 2.0f * (relX * relVelX + relY * relVelY);
        //     float cCoeff = relX * relX + relY * relY - minDistance * minDistance;

        //     float discriminant = bCoeff * bCoeff - 4 * aCoeff * cCoeff;
        //     if (discriminant < 0)
        //         return;

        //     float sqrtDiscriminant = std::sqrt(discriminant);
        //     float t = (-bCoeff - sqrtDiscriminant) / (2 * aCoeff);
        //     if (t < 0 || t > 1)
        //         return;

        //     // 保存原始状态
        //     float aVx = a->getVx(), aVy = a->getVy();
        //     float bVx = b->getVx(), bVy = b->getVy();

        //     // 回退到碰撞时刻
        //     a->setPosition(a->getX() - aVx * t, a->getY() - aVy * t);
        //     b->setPosition(b->getX() - bVx * t, b->getY() - bVy * t);

        //     // 处理碰撞
        //     handleBallCollision(a, b);

        //     // 前进剩余时间
        //     a->setPosition(a->getX() + a->getVx() * t, a->getY() + a->getVy() * t);
        //     b->setPosition(b->getX() + b->getVx() * t, b->getVy() * t);
        // }

        /**
         * @brief 检查坐标是否在袋口内
         * @param x x坐标
         * @param y y坐标
         * @return 在袋口内返回true
         */
        bool isInPocket(float x, float y) const
        {
            // 袋口位置和半径
            const float pocketRadius = 18.0f;
            const std::vector<std::pair<float, float>> pockets = {
                {161.0f, 111.0f}, // 左上
                {634.0f, 111.0f}, // 右上
                {398.0f, 106.0f}, // 上中
                {161.0f, 345.0f}, // 左下
                {634.0f, 345.0f}, // 右下
                {398.0f, 350.0f}  // 下中
            };

            for (const auto &pocket : pockets)
            {
                float dx = x - pocket.first;
                float dy = y - pocket.second;
                if (dx * dx + dy * dy < pocketRadius * pocketRadius)
                {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief 处理犯规
         */
        void handleFoul()
        {

            // 白球自由球
            cueBall_->setState(BallState::InHand);
            if (currentPlayer_ == 0)
            {
                currentState_ = BallGameState::WaitingPlaceCue;
            }
            else
            {
                currentState_ = BallGameState::PlaceCue;
            }

            // 切换到对手回合
            switchPlayer();

            foul_ = false;
            firstContactValid_ = false;
        }

        /**
         * @brief 切换玩家
         */
        void switchPlayer()
        {
            currentPlayer_ = 1 - currentPlayer_;
            firstContactValid_ = false;
        }

        /**
         * @brief 检查8号球是否合法进袋
         * @return 合法返回true
         */
        bool isLegal8BallPocket() const
        {
            // 检查当前玩家是否已经打完自己的球
            auto ball = balls_;
            while (ball)
            {
                if (ball->getState() == BallState::OnTable)
                {
                    // 如果还有当前玩家的球在台面上，击打8号球犯规
                    if ((playerTypes_[currentPlayer_] == BallType::Solid && ball->getType() == BallType::Solid && ball->getId() != 8) ||
                        (playerTypes_[currentPlayer_] == BallType::Striped && ball->getType() == BallType::Striped))
                    {
                        return false;
                    }
                }
                ball = ball->getNext();
            }
            return true;
        }

        /**
         * @brief 检查游戏是否结束
         * @return 游戏结束返回true
         */
        // bool checkGameOver() const
        // {
        //     // 检查8号球是否进袋
        //     auto eightBall = findBall(8);
        //     if (eightBall && eightBall->getState() == BallState::Pocketed)
        //     {
        //         return true;
        //     }

        //     // 检查是否有玩家已经打完自己的球
        //     for (int i = 0; i < 2; ++i)
        //     {
        //         if (hasAssigned_[i])
        //         {
        //             bool allPocketed = true;
        //             auto ball = balls_;
        //             while (ball)
        //             {
        //                 if (ball->getState() == BallState::OnTable)
        //                 {
        //                     if ((playerTypes_[i] == BallType::Solid && ball->getType() == BallType::Solid && ball->getId() != 8) ||
        //                         (playerTypes_[i] == BallType::Striped && ball->getType() == BallType::Striped))
        //                     {
        //                         allPocketed = false;
        //                         break;
        //                     }
        //                 }
        //                 ball = ball->getNext();
        //             }
        //             if (allPocketed)
        //             {
        //                 return true;
        //             }
        //         }
        //     }

        //     return false;
        // }

        /**
         * @brief 计算球杆角度
         * @return 球杆角度(弧度)
         */
        float calculateCueAngle() const
        {
            // 这里应该根据鼠标位置计算角度
            // 简化实现：返回固定角度
            return 0.0f; // 水平方向
        }

        /**
         * @brief 释放球链表
         * @param head 链表头节点
         */
        void freeBalls(std::shared_ptr<Ball> head)
        {
            while (head)
            {
                auto next = head->getNext();
                head->setNext(nullptr);
                head = next;
            }
        }

        BallGameState currentState_;          // 当前游戏状态
        std::shared_ptr<Ball> balls_;         // 球链表头指针
        std::shared_ptr<Ball> cueBall_;       // 白球指针
        int currentPlayer_;                   // 当前玩家索引(0或1)
        BallType playerTypes_[2];             // 玩家分配的球类型
        bool hasAssigned_[2];                 // 玩家是否已分配球类型
        bool firstContactValid_;              // 第一次接触是否有效
        std::shared_ptr<Ball> firstPocketed_; // 第一个落袋的球
        bool foul_;                           // 是否犯规
        bool isSelfBallPocketed;              // 是否是自家球进袋
        int pocketedCount_[2];                // 玩家进球计数
        bool isContinueAiming;                // 玩家是否继续击球
        bool foul_pocket_8;                   // 提前打进8号球犯规
        bool is_normal_pocket_8;              // 是否正常打进8号球

    public:
        bool isWin; // 玩家是否胜利
    public:
        /**
         * @brief 标记需要重绘
         */
        void requestRedraw() { needRedraw_ = true; }

        /**
         * @brief 检查是否需要重绘
         */
        bool needsRedraw() const { return needRedraw_; }

        /**
         * @brief 重置重绘标志
         */
        void resetRedrawFlag() { needRedraw_ = false; }

    private:
        mutable bool needRedraw_ = true; // 使用mutable修饰符
        float currentCueAngle_ = 0.0f;   // 当前球杆角度
        float currentPower_ = 0.0f;      // 当前蓄力值

        float tempCueX_ = 0; // 临时白球位置X
        float tempCueY_ = 0; // 临时白球位置Y
        /**
         * @brief 检查白球位置是否合法
         */
        bool isValidCuePosition(float x, float y) const
        {
            // 检查是否在球桌内
            if (x < 159 + BALL_RADIUS || x > 636 - BALL_RADIUS ||
                y < 109 + BALL_RADIUS || y > 347 - BALL_RADIUS)
            {
                return false;
            }

            // 检查是否与其他球重叠
            auto ball = balls_;
            while (ball)
            {
                if (ball->getState() == BallState::OnTable)
                {
                    float dx = x - ball->getX();
                    float dy = y - ball->getY();
                    float distSq = dx * dx + dy * dy;
                    float minDist = BALL_RADIUS * 2;
                    if (distSq < minDist * minDist)
                    {
                        return false;
                    }
                }
                ball = ball->getNext();
            }
            return true;
        }
    };

} // namespace PoolGame

// #include <server/client_server.h>

// inline void PoolGame::Game::sendMessageWithClient(const string &message) {
//     sendMessage(const string &message);
// }

#endif // BALL_H
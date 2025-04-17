// Author: 刘瀚霖
#include <graphics.h> // EasyX 图形库
#include <conio.h>    // _getch() 函数
#include <winsock2.h> // Windows 套接字 API
#include <string>     // std::string
#include <windows.h>  // SetConsoleOutputCP() 函数
#include <iostream>   // std::cout
#include <fstream>    // 文件读写
#include <locale>     // setlocale() 函数
#include <codecvt>    // std::codecvt_utf8
#include <ctime>      // std::time_t
#include <gdiplus.h>  // GDI+ 图形库
#include <algorithm>  // for min and remove functions
#include <sys/stat.h> // 判断文件是否存在
#include <io.h>
#include <curl/curl.h> // libcurl 库
#include <sstream>     // std::stringstream
#include <direct.h>    // _mkdir() 函数
#include <iomanip>     // setprecision 函数
#include <vector>      // std::vector
#include <thread>      // std::thread
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <queue>
#include <map>
#include <mmsystem.h>
#include <optional>
#include <chrono>
#include <unordered_map>
#include <tchar.h>
#include <shellapi.h>

// 自定义头文件
#include <game_config.h>          // 游戏配置
#include <logger.h>               // 日志记录器类
#include <httplib.h>              // HTTP 请求相关函数
#include <json.h>                 // JSON 解析器类
#include <alipayF2F.h>            // 支付宝 F2F 支付类
#include <graphic_functions.h>    // 窗口相关函数
#include <server/client_server.h> // 客户端服务器
#include <ball.h>                 // 球类控制器
#include <server/client_bridge.h> // 客户端桥接器
#include <music.h>                // 音乐播放函数
#include <manage_txt.h>           // 文本管理器

using namespace std; // 使用 std 命名空间

// 声明

void stateSwitch(); // 状态切换函数声明
void initSetting(); // 初始化设置函数声明

// void update_user_list(const std::vector<int> &online_users_arr, int page_id, int my_user_id);

std::atomic_bool g_NetworkRunning{false}; // 网络线程运行标志
std::thread g_NetworkThread;              // 网络线程句柄

// 用户账号相关类
class User_account
{
public:
    // 获取用户输入的用户名和密码
    void get_user_credentials(string &username, string &password)
    {
        // cout << "响袋台球 | 24027202 刘瀚霖" << endl;
        // cout << "登录&注册（未注册自动注册）" << endl;
        // cout << "用户名: ";
        // cin >> username;
        // cout << "密码: ";
        // cin >> password; // 这里不隐藏输入，避免 `_getch()` 兼容性问题

        // 设置字体
        set_custom_font(17, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf",
                        L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(WHITE);
        setbkmode(TRANSPARENT);

        // 初始绘制
        cleardevice();
        resizeImage("./images/login_bg.png", 0, 0, 300, 377);
        resizeImage("./images/login_bar.png", 80, 137, 194, 45);
        resizeImage("./images/login_bar.png", 80, 187, 194, 45);
        resizeImage("./images/login_button.png", 90, 264, 122, 35);

        // 绘制静态文本
        outtextxy(20, 149, "用户名");
        outtextxy(20, 199, "  密码");
        set_custom_font(16, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf",
                        L"Fusion Pixel 12px monospaced zh_hans");
        outtextxy(116, 271, "登录/注册");

        // 绘制输入内容
        set_custom_font(15, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf",
                        L"Fusion Pixel 12px monospaced zh_hans");

        settextcolor(BLACK);
        outtextxy(95, 152, g_username.c_str());
        string hidden_pw(g_password.size(), '*');
        outtextxy(95, 202, hidden_pw.c_str());

        FlushBatchDraw();

        // 主循环
        while (!login_successful)
        {
            BeginBatchDraw();
            // 只有需要重绘时才更新界面
            if (g_needs_redraw)
            {
                // cleardevice();
                resizeImage("./images/login_bg.png", 0, 0, 300, 377);
                // ImageLoader::drawImageFromURLOnLogin("https://cdn.sa.net/2025/03/27/b8Cao5wlu3MZRyL.png", 0, 0);

                // 高亮当前活动输入框
                if (g_input_active)
                {
                    setlinecolor(WHITE);
                    rectangle(79, 136, 274, 182); // 用户名框边框
                }
                else
                {
                    setlinecolor(WHITE);
                    rectangle(79, 186, 274, 232);
                }

                resizeImage("./images/login_bar.png", 80, 137, 194, 45);
                resizeImage("./images/login_bar.png", 80, 187, 194, 45);
                resizeImage("./images/login_button.png", 90, 264, 122, 35);

                // 重绘静态文本
                set_custom_font(17, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf",
                                L"Fusion Pixel 12px monospaced zh_hans");
                settextcolor(WHITE);
                outtextxy(20, 149, "用户名");
                outtextxy(20, 199, "  密码");
                set_custom_font(16, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf",
                                L"Fusion Pixel 12px monospaced zh_hans");
                outtextxy(116, 271, "登录/注册");

                // 绘制输入内容
                set_custom_font(15, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf",
                                L"Fusion Pixel 12px monospaced zh_hans");

                settextcolor(BLACK);
                outtextxy(95, 152, g_username.c_str());
                string hidden_pw(g_password.size(), '*');
                outtextxy(95, 202, hidden_pw.c_str());

                FlushBatchDraw();
                g_needs_redraw = false;
            }

            // 处理鼠标点击
            if (MouseHit())
            {
                MOUSEMSG msg = GetMouseMsg();
                if (msg.uMsg == WM_LBUTTONDOWN)
                {
                    // 检查用户名框点击
                    if (msg.x >= 80 && msg.x <= 274 && msg.y >= 137 && msg.y <= 182)
                    {
                        g_input_active = true;
                        g_needs_redraw = true;
                        inputUsername = true;
                        inputPassword = false;
                    }
                    // 检查密码框点击
                    else if (msg.x >= 80 && msg.x <= 274 && msg.y >= 187 && msg.y <= 232)
                    {
                        g_input_active = false;
                        g_needs_redraw = true;
                        inputUsername = false;
                        inputPassword = true;
                    }
                    // 检查登录按钮点击
                    else if (msg.x >= 90 && msg.x <= 212 && msg.y >= 264 && msg.y <= 299)
                    {
                        username = std::string(g_username.begin(), g_username.end());
                        password = std::string(g_password.begin(), g_password.end());
                        login_button(username, password);
                        // return;
                    }
                }
            }

            processKeyboardInput(); // 处理键盘输入
            EndBatchDraw();
            Sleep(10); // 降低CPU占用
        }
    }

    // 登录函数（API 版）
    bool login(const string &username, const string &password)
    {
        stringstream url_stream;
        url_stream << API_URL << "/login";
        string url = url_stream.str();
        string postData = "username=" + username + "&password=" + password;

        string response = sendPostRequest(url, postData);
        if (jsonParser.success(response))
        {
            cerr << "登录成功！" << endl;
            return true;
        }
        else
        {
            // cerr << "登录失败: " << jsonParser.error(response) << endl;
            return false;
        }
    }

    // 检查用户名是否存在（API 版）
    bool username_exists(const string &username)
    {
        stringstream url_stream;
        url_stream << API_URL << "/check_username?username=" << username;
        string url = url_stream.str();
        string response = sendGetRequest(url);

        jsonParser.success(response);
        if (jsonParser.success(response) == 1)
        {
            return true;
        }
        else
        {
            return false;
        }
        // cerr <<jsonParser.success(response) << endl;
        // return jsonParser.success(response);
    }

    // 注册用户（API 版）
    void register_user(const string &username, const string &password)
    {
        stringstream url_stream;
        url_stream << API_URL << "/register";
        string url = url_stream.str();
        string postData = "username=" + username + "&password=" + password;

        string response = sendPostRequest(url, postData);
        if (jsonParser.success(response))
        {
            cout << "注册成功" << endl;
        }
        else
        {
            cerr << "注册失败: " << jsonParser.error(response) << endl;
        }
    }

    // 保存登录信息到文件
    void save_login_info(const string &username, const string &password)
    {
        ofstream file("login_info.txt");
        if (file.is_open())
        {
            file << username << endl;
            file << password << endl;
            file.close();
        }
        else
        {
            cerr << "无法打开文件保存登录信息" << endl;
        }
    }

    // 读取登录信息
    bool load_login_info(string &username, string &password)
    {
        ifstream file("login_info.txt");
        if (file.is_open())
        {
            getline(file, username);
            getline(file, password);
            file.close();
            return true;
        }
        return false;
    }

    // 处理用户登录（自动读取文件）
    void handle_login_with_file()
    {
        string username, password;

        if (load_login_info(username, password))
        {
            if (login(username, password))
            {
                // cout << "自动登录成功！" << endl;
                login_successful = true;
                create_centered_window(800, 400, L"响袋台球", L"./images/app-icon.ico");
                modify_window(800, 400, L"响袋台球");
            }
            else
            {
                cout << "自动登录失败，请手动登录。" << endl;
                username.clear();
                password.clear();
            }
        }

        if (!login_successful)
        {
            create_centered_window(800, 400, L"响袋台球", L"./images/app-icon.ico");
            modify_window(300, 377, L"响袋台球 | 登录");
            get_user_credentials(username, password);
        }

        while (!login_successful)
        {
            Sleep(100);
        }
    }

    // 处理用户登录（自动读取文件）
    void relogin_and_save_file()
    {
        login_successful = false;
        modify_window(300, 377, L"响袋台球 | 登录");

        string username, password;

        if (!login_successful)
        {
            get_user_credentials(username, password);
        }

        while (true)
        {
            Sleep(100);
            if (login_successful)
            {
                currentState = MAIN_MENU;
                stateSwitch();
                break;
            }
        }
    }

    void login_button(string username, string password)
    {
        cerr << username << endl;
        cerr << password << endl;
        if (login(username, password))
        {
            save_login_info(username, password);
            ImageLoader::clearAll();
            login_successful = true;
            modify_window(800, 400, L"响袋台球");
        }
        else
        {
            if (username_exists(username))
            {
                cout << "密码错误！请重新输入。" << endl;
                setlinecolor(RED);
                rectangle(79, 186, 274, 232);
                rectangle(79, 136, 274, 182);
                settextcolor(RGB(255, 78, 59));
                set_custom_font(12, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf",
                                L"Fusion Pixel 12px monospaced zh_hans");
                outtextxy(105, 232, "用户名密码不匹配,请重新输入！");

                // closegraph();
                // get_user_credentials(username, password);
            }
            else
            {
                cout << "用户名不存在，正在注册新用户..." << endl;
                register_user(username, password);
                save_login_info(username, password);
                login_successful = true;
                ImageLoader::clearAll();
                // 更改窗口大小
                modify_window(800, 400, L"响袋台球");
            }
        }
    }

    // 给已登录用户增加add_number个金币的函数
    void add_coins_to_user(int add_number)
    {
        // 读取用户名和密码
        string username;
        string password;
        ifstream file("login_info.txt");
        if (file.is_open())
        {
            getline(file, username);
            getline(file, password);
            file.close();
        }
        else
        {
            logger.error("无法打开文件读取登录信息");
            return;
        }

        string url_add_coins = string(API_URL) + "/add_coins";
        string data_add_coins = "username=" + username + "&password=" + password + "&addcoins=" + to_string(add_number);
        string response_add_coins = sendPostRequest(url_add_coins, data_add_coins);
        while (jsonParser.success(response_add_coins) == false)
        {
            cerr << "充值金币失败: " << jsonParser.error(response_add_coins) << endl;
            cerr << "正在重试..." << endl;
            string response_add_coins = sendPostRequest(url_add_coins, data_add_coins);
        }
        cout << "成功充值金币" << add_number << "个" << endl;
    }

    void update_total_count(bool isWin)
    {
        string url_update_total_count = string(API_URL) + "/update_win_count_and_coins";
        string data_update_total_count;
        if (isWin)
        {
            data_update_total_count = "user_id=" + to_string(global_user_id) + "&isWin=1";
        }
        else
        {
            data_update_total_count = "user_id=" + to_string(global_user_id) + "&isWin=0";
        }
        // cerr << data_update_total_count << endl;
        string response_update_total_counts = sendPostRequest(url_update_total_count, data_update_total_count);
        // cerr << response_update_total_counts << endl;
        cout << "获得金币2个,奖牌1个！" << endl;
    }

    void startClientServer()
    {
        string username;
        ifstream file("login_info.txt");
        if (file.is_open())
        {
            getline(file, username);
            file.close();
        }
        else
        {
            logger.error("无法打开文件读取登录信息");
        }
        // 查询用户id（获取ID后储存到全局变量中）
        string url_check_id = string(API_URL) + "/get_user_id?username=" + username;
        string response_id = sendGetRequest(url_check_id);
        string user_id = jsonParser.extractJsonValue(response_id, "user_id");

        // 已经获取了用户ID, 保存到全局变量
        global_user_id = stoi(user_id);
        // cerr << "用户ID: " << global_user_id << endl;

        // string message_type = "get_online_users";
        // std::unordered_map<std::string, std::string> data;
        // string getOnlineUsersMessage = jsonParser.createJson(message_type, data); // 构建json字符串

        // 启动网络线程
        g_NetworkRunning.store(true);
        g_NetworkThread = std::thread([this, user_id]()
                                      {
    while (g_NetworkRunning.load()) {
        try {
            if (client.connectServer(user_id)) {
                client.receiveMessages(); //
                
            } else {
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        } catch (const std::exception& e) {
            std::cerr << "网络异常: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
    client.closeConnection(); });

        // 分离网络线程
        g_NetworkThread.detach();

        // if (client.connectServer(user_id))
        // {

        //     thread receiver(&WebSocketClient::receiveMessages, &client);
        //     receiver.join();
        // }
        // else
        // {
        //     drawMessageBox("服务器连接失败", "请检查网络连接", []()
        //                    { exit(1); }, false);
        // }
    }

    void processKeyboardInput()
    {
        // 监听所有可打印字符（字母、数字、符号）
        for (char ch = '0'; ch <= 'Z'; ch++) // '0'~'9' + 'A'~'Z'
        {
            if (GetAsyncKeyState(ch) & 0x8000) // 检测按键
            {
                bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000);
                bool changed = false;

                char inputChar = ch;

                // 处理小写字母
                if (ch >= 'A' && ch <= 'Z' && !shiftPressed) // 未按 Shift，转换为小写
                {
                    inputChar = ch + 32;
                }

                if (inputUsername)
                {
                    g_username += inputChar;
                    changed = true;
                }
                else if (inputPassword)
                {
                    g_password += inputChar;
                    changed = true;
                }

                if (changed)
                {
                    g_needs_redraw = true;
                }

                Sleep(100); // 避免重复触发
            }
        }

        // 监听退格键（VK_BACK）
        if (GetAsyncKeyState(VK_BACK) & 0x8000)
        {
            bool changed = false;

            if (inputUsername && !g_username.empty())
            {
                g_username.pop_back();
                changed = true;
            }
            else if (inputPassword && !g_password.empty())
            {
                g_password.pop_back();
                changed = true;
            }

            if (changed)
            {
                g_needs_redraw = true;
            }

            Sleep(100); // 避免触发过快
        }
    }

private:
    // 全局状态
    string g_username;
    string g_password;
    bool g_login_clicked = false;
    bool g_needs_redraw = true; // 初始需要绘制
    bool g_input_active = true; // 输入状态标志
    bool inputUsername = true;
    bool inputPassword = false;
    bool login_successful = false;
};
User_account user_account;

// CharacterItem结构体用于储存皮肤信息
struct CharacterItem
{
    string id;
    string name;
    string description;
    string price;
    string url_big;
    string url_small;
    string url_bg;
    string url_game_bg;
};
vector<CharacterItem> characterItem;
// RechargeItem结构体用于储存充值item信息
struct RechargeItem
{
    int id;
    int coin_amount;
    double price;
    string image_url;
};
vector<RechargeItem> rechargeItems;
// **imageLoader静态成员初始化**
vector<shared_ptr<ImageLoader>> ImageLoader::instances;
mutex ImageLoader::instanceMutex;
atomic<bool> ImageLoader::exitFlag{false};
std::thread ImageLoader::eventThread;
// 定义全局变量（实际分配内存）
std::atomic<bool> g_GameRunning{false}; // 初始值为false
std::mutex g_GraphicsMutex;

// 预加载类
class Preload
{
public:
    // 预加载时的进度条显示
    void drawProgressBar(int progress, int total)
    {
        // 绘制加载背景
        resizeImage("./images/loading.png", 0, 0, 800, 400);

        int barWidth = 400; // 进度条宽度
        int barHeight = 30; // 进度条高度
        int startX = 200;   // 进度条左上角 X 坐标
        int startY = 300;   // 进度条左上角 Y 坐标
        int percent = (progress * 100) / total;

        setfillcolor(RGB(50, 50, 50)); // 灰色背景
        fillrectangle(startX, startY, startX + barWidth, startY + barHeight);

        setfillcolor(RGB(0, 150, 0)); // 绿色进度
        fillrectangle(startX, startY, startX + (barWidth * percent / 100), startY + barHeight);

        set_custom_font(17, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);

        string progressText;
        if (percent >= 100)
        {
            progressText = "加载完成";
        }
        else
        {
            progressText = "Loading... " + to_string(percent) + "%";
        }
        outtextxy(startX - 20 + barWidth / 2 - 50, startY + barHeight + 10, progressText.c_str());
        outtextxy(startX - 80 + barWidth / 2 - 50, startY - 30, "首次加载速度较慢，请耐心等待");
    }

    // 提前加载在线图片
    void preload_online_image()
    {
        string urls[] =
            {
                "https://cdn.sa.net/2025/03/08/rH63hbFUVqIgNQG.png", // 主背景
                "https://cdn.sa.net/2025/03/02/WZOfovSqQwxT5rV.png", // 充值加号
                "https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", // 常规灰色border
                "https://cdn.sa.net/2025/03/02/vqmEKAhjc9rsPpZ.png", // 返回按钮
                "https://cdn.sa.net/2025/03/08/6yHDwNIuRs23oZa.png", // 支付二维码框
                "https://cdn.sa.net/2025/03/03/gwbjP56Mu7AWs4Y.png", // 支付界面价格框
                "https://cdn.sa.net/2025/03/03/PvIfru6ioRhzLYV.png", // 购物车
                "https://cdn.sa.net/2025/03/08/NOZgC3SahLqTPGK.jpg", // Loading
                "https://cdn.sa.net/2025/03/08/7OBJl293ygKcmeD.png", // 长灰色border
                "https://cdn.sa.net/2025/03/08/V3ZIapxgBWRKSw9.png", // 支付刷新
                "https://cdn.sa.net/2025/03/08/T6mv9U1YtnBciDd.png", // 欢迎使用支付宝
                "https://cdn.sa.net/2025/03/03/HOxQKXN4kuYgsGp.png", // 常规灰色border
                // 充值商品图片
                "https://cdn.sa.net/2025/03/03/F5KjtHs8nfIkeJO.png",
                "https://cdn.sa.net/2025/03/03/HT6DBlWcmUk1iSh.png",
                "https://cdn.sa.net/2025/03/03/2CybUdQf6rMXzSt.png",

                // 皮肤商店
                "https://cdn.sa.net/2025/03/14/y9CR1M4OglutWjA.png", // 使用中
                "https://cdn.sa.net/2025/03/14/tuCLnWNxsmkU6qE.png", // 使用
                "https://cdn.sa.net/2025/03/14/tuCLnWNxsmkU6qE.png", // 切换中
                "https://cdn.sa.net/2025/03/14/mpSykqXuBvjNH73.png", // 购买
                "https://cdn.sa.net/2025/03/15/XYixOCkhF5n8Z4f.png", // 金币不足
                "https://cdn.sa.net/2025/03/15/PvAcDduhZi5TJUm.png", // 充值入口
                "https://cdn.sa.net/2025/03/15/SxHN58MpUk7Ormj.png", // 确认购买
                "https://cdn.sa.net/2025/03/13/Vsm9rktM1uYASQe.png", // 切换左按钮
                "https://cdn.sa.net/2025/03/13/4JYTMRofy2jaSIP.png", // 切换右按钮

                // 皮肤信息characters
                // 1
                "https://cdn.sa.net/2025/03/09/ACqfzEWPeBM3Jbx.png",
                "https://cdn.sa.net/2025/03/09/BGaHlsypMo8qkDT.png",
                "https://cdn.sa.net/2025/03/13/hIBMYG38vl6nFKQ.png",
                "https://cdn.sa.net/2025/03/28/ine9MolpWCTtQ8y.png",
                // 2
                "https://cdn.sa.net/2025/03/09/8Mo1uxiHDELtk3b.png",
                "https://cdn.sa.net/2025/03/09/MdVFIpxbqClXKet.png",
                "https://cdn.sa.net/2025/03/13/zH6oUCvsPnRMSib.png",
                "https://cdn.sa.net/2025/03/28/7nT3uPzeRLKUbQ9.png",
                // 3
                "https://cdn.sa.net/2025/03/13/fvMUT3Qo4kxNFWm.png",
                "https://cdn.sa.net/2025/03/13/9Uh1KEVdrOmzlsv.png",
                "https://cdn.sa.net/2025/03/27/ytSeRcqYl5nDkrJ.png",
                "https://cdn.sa.net/2025/03/28/XlWydIE6KgF3Rcz.png",
                // 4
                "https://cdn.sa.net/2025/03/14/3bBi4kueIdHh5RQ.png",
                "https://cdn.sa.net/2025/03/14/XZtp4v9TOGw1rlQ.png",
                "https://cdn.sa.net/2025/03/14/IOGm4kDqBtwCcJ1.png",
                "https://cdn.sa.net/2025/03/28/SLcGeEYpACxVryH.png",
                // 5
                "https://cdn.sa.net/2025/03/14/apijc4Buoqx6Kv5.png",
                "https://cdn.sa.net/2025/03/14/BKTwlthkUIPWZy5.png",
                "https://cdn.sa.net/2025/03/15/XLytVD9roxCW8sd.png",
                "https://cdn.sa.net/2025/03/28/3tvWNxPFwKSd6Vl.png",
                // 6
                "https://cdn.sa.net/2025/03/15/or7Ys8U39VIdyJB.png",
                "https://cdn.sa.net/2025/03/15/bp5yNKuswZPrOAR.png",
                "https://cdn.sa.net/2025/03/20/g3Q1vEt7CRxb4au.png",
                "https://cdn.sa.net/2025/03/28/KlNbf1F7HMTYoZh.png",
                // 7
                "https://cdn.sa.net/2025/03/20/uDKw9ozOaW6S8rU.png",
                "https://cdn.sa.net/2025/03/20/qnacmdRtQj2ClbN.png",
                "https://cdn.sa.net/2025/03/20/jskdcuGL8TKixFg.png",
                "https://cdn.sa.net/2025/03/28/L2aDmbJRFQWUMrz.png",
                // 8
                "https://cdn.sa.net/2025/03/20/JZ6KCPOzQn2Hqhb.png",
                "https://cdn.sa.net/2025/03/20/MLQl8UtYkWneusR.png",
                "https://cdn.sa.net/2025/03/20/7uziFSXhG6Nltaw.png",
                "https://cdn.sa.net/2025/03/28/IopBMQt631xHJny.png"

            };
        int i = 0;
        int size = sizeof(urls) / sizeof(urls[0]); // 计算数组长度

        while (i < size)
        {
            ImageLoader::preDownloadImage(urls[i]);
            i++;
            drawProgressBar(i + 1, size); // 更新进度条
            FlushBatchDraw();             // 刷新 EasyX 画面

            // this_thread::sleep_for(chrono::milliseconds(200)); // 模拟下载延迟
            // cerr << "下载图片：" << urls[i] << endl;
        }
    }

    // 提前获取角色信息
    void preloadCharacterItem()
    {
        characterItem.clear(); // 清空原有数据
        string url_preload_character_item = string(API_URL) + "/get_character_details";
        string response = sendGetRequest(url_preload_character_item);

        size_t pos = 0;
        while ((pos = response.find("{", pos)) != string::npos)
        {
            size_t endPos = response.find("}", pos); // 找到当前对象的 `}`
            if (endPos == string::npos)
                break;

            string jsonObject = response.substr(pos, endPos - pos + 1); // 提取单个 JSON 对象
            pos = endPos + 1;                                           // 移动 `pos`，避免死循环

            CharacterItem item;
            item.id = jsonParser.extractJsonValue(jsonObject, "id");
            item.name = jsonParser.extractJsonValue(jsonObject, "name");
            item.description = jsonParser.extractJsonValue(jsonObject, "description");
            item.price = jsonParser.extractJsonValue(jsonObject, "price");
            item.url_big = jsonParser.extractJsonValue(jsonObject, "url_big");
            item.url_small = jsonParser.extractJsonValue(jsonObject, "url_small");
            item.url_bg = jsonParser.extractJsonValue(jsonObject, "url_bg");
            item.url_game_bg = jsonParser.extractJsonValue(jsonObject, "url_game_bg");

            characterItem.push_back(item); // 存入 vector

            // 输出调试信息，确保数据正确
            // cerr << "加载皮肤: " << item.id << " - " << item.name << endl;
        }
        // 查询角色总数 characters_total_number
        string url_get_characters_total_numnber = string(API_URL) + "/get_characters_total_number";
        string response_get_characters_total_numnber = sendGetRequest(url_get_characters_total_numnber);

        characters_total_number = stoi(jsonParser.extractJsonValue(response_get_characters_total_numnber, "nums"));
    }

    // 获取商品信息
    void preloadRechargeItem()
    {
        rechargeItems.clear(); // 清空原有数据
        string url_preload_recharge_item = string(API_URL) + "/get_recharge_shop_items";
        string response = sendGetRequest(url_preload_recharge_item);

        size_t pos = 0;
        while ((pos = response.find("{", pos)) != string::npos)
        {
            size_t endPos = response.find("}", pos); // 找到当前对象的 `}`
            if (endPos == string::npos)
                break;

            string jsonObject = response.substr(pos, endPos - pos + 1); // 提取单个 JSON 对象
            pos = endPos + 1;                                           // 移动 `pos`，避免死循环

            RechargeItem item;
            item.id = stoi(jsonParser.extractJsonValue(jsonObject, "id"));
            item.coin_amount = stoi(jsonParser.extractJsonValue(jsonObject, "coin_amount"));
            item.price = stod(jsonParser.extractJsonValue(jsonObject, "price"));
            item.image_url = jsonParser.extractJsonValue(jsonObject, "image_url");

            rechargeItems.push_back(item); // 存入 vector

            // 输出调试信息，确保数据正确
            // cerr << "加载商品: " << item.id << " - " << item.coin_amount << " - " << item.price << endl;
        }
    }

    // 获取用户皮肤设置信息
    void preLoadUserCharacter()
    {
        // 获取用户名
        string username;
        ifstream file("login_info.txt");
        if (file.is_open())
        {
            getline(file, username);
            file.close();
        }
        else
        {
            logger.error("无法打开文件读取登录信息");
            return;
        }
        // 查询用户角色 character_set_id
        stringstream url_get_user_character_set_stream;
        url_get_user_character_set_stream << API_URL << "/get_user_character_set?username=" << username;
        string url_get_user_character_set = url_get_user_character_set_stream.str();
        string response_get_user_character_set = sendGetRequest(url_get_user_character_set);

        character_set_id = stoi(jsonParser.extractJsonValue(response_get_user_character_set, "user_character_set"));
    }
};
Preload preload;

// 按钮函数
class Button
{
public:
    void go_to_SETTING()
    {
        play_button_sound();
        currentState = SETTING;
        stateSwitch();
    }
    void go_to_MAIN_MENU()
    {
        play_button_sound();
        currentState = MAIN_MENU;
        stateSwitch();
    }
    void go_to_SKIN_SHOP()
    {
        play_button_sound();
        currentState = SKIN_SHOP;
        stateSwitch();
    }
    void go_to_RECHARGE_SHOP()
    {
        play_button_sound();
        currentState = RECHARGE_SHOP;
        stateSwitch();
    }
    void go_to_GAME_PREPARING()
    {
        play_button_sound();
        currentState = GAME_PREPARING;
        stateSwitch();
    }
    void buy_button_in_RECHARGE_SHOP()
    {
        play_button_sound();
    }
    void refresh_button_in_paying_page()
    {
        play_button_sound();
    }
    void change_page_button_in_SKIN_SHOP()
    {
        play_button_sound();
    }
    void only_play_button_sound()
    {
        play_button_sound();
    }

private:
    void play_button_sound()
    {
        // PlaySound(TEXT("packs\\kenney-ui-pack\\Sounds\\click-b.wav"), NULL, SND_FILENAME | SND_ASYNC);
        if (setting_click_sound)
        {
            AudioManager::play("packs/kenney-ui-pack/Sounds/click-b.wav", "click", false);
        }
    }
};
Button button;

// 绘制页面类
class DrawPages
{
public:
    // 绘制设置页面
    void draw_setting_page()
    {
        initSetting();
        auto config = std::make_shared<BoolConfigManager>("app_settings.txt");
        // BoolConfigManager config("app_settings.txt");
        BeginBatchDraw(); // 开始批量绘制
        // 绘制游戏准备画面背景图片
        cleardevice();
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/24/ARG8Z92qeiOaMJv.png", 0, 0);
        // 绘制返回主界面按钮
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/02/vqmEKAhjc9rsPpZ.png", 5, 5, true, [=]()
                                      { button.go_to_MAIN_MENU(); });
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", 60, 5);
        set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);
        outtextxy(88, 17, "游戏设置");

        set_custom_font(24, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        outtextxy(580, 50, "关于");

        set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        string aboutText = "响袋台球 " + gameVersion + " by 刘瀚霖\n\n高级语言程序课设做的游戏,素材都是用PPT做的,略显粗糙。\n\n开源库:EasyX, Openssl, Curl\n\n项目Github:\nlhl77/billiards-online-cpp\n\n";
        drawTextWithWrap(450, 100, aboutText, 300);

        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/28/Cyp5uLwqEOTi1FR.png", 660, 350, true, []()
                                      {
                                          const char *filename = "login_info.txt";

                                          if (std::remove(filename) == 0)
                                          {
                                              std::cout << "文件 " << filename << " 删除成功。\n";
                                              // user_account.relogin_and_save_file();
                                              closegraph();
                                              exit(0);
                                          }
                                          else
                                          {
                                              std::perror("删除文件失败"); // 输出错误信息
                                          }; });
        set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(WHITE);
        outtextxy(682, 361, "退出登录");

        set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);

        string setting_roll1[5][2] = {
            {"setting_main_bgm", "主界面BGM"},
            {"setting_play_bgm", "游戏BGM"},
            {"setting_ball_sound", "台球碰撞"},
            {"setting_click_sound", "按钮点击"},
            {"setting_nontification_sound", "通知音效"}};

        int roll1_x = 5;
        int roll1_y = 78;
        set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);

        for (int i = 0; i < 5; i++)
        {
            if (i == 0)
            {
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", roll1_x, roll1_y);
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/16/WNEpaJVRUOi4HFG.png", roll1_x + 13, roll1_y + 10);
                outtextxy(roll1_x + 62, roll1_y + 12, "声音");
                roll1_y += 50;
            }
            // 长条bar
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/7OBJl293ygKcmeD.png", roll1_x, roll1_y);
            // 更改按钮
            if (stringToBool(setting_roll1[i][0]))
            {
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/16/mZQsj5JaklotMGp.png", roll1_x + 106, roll1_y, true, [setting_roll1, i, config]()
                                              {
                    button.only_play_button_sound();
                    config->setSetting(setting_roll1[i][0], false);
                    initSetting();
                    currentState = SETTING;
                    stateSwitch(); });
            }
            else
            {
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/16/Ju3ngr9OSD1e58B.png", roll1_x + 106, roll1_y, true, [setting_roll1, i, config]()
                                              {
                    button.only_play_button_sound();
                    config->setSetting(std::string(setting_roll1[i][0]), true);
                    initSetting();
                    currentState = SETTING;
                    stateSwitch(); });
            }
            outtextxy(roll1_x + 13, roll1_y + 13, setting_roll1[i][1].c_str());
            roll1_y += 50;
        }

        string setting_roll2[4][2] = {
            {"Github项目", "https://cdn.sa.net/2025/04/16/rUNw1g9X6kA34Ry.png"}};

        int roll2_x = 209;
        int roll2_y = 78;
        set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);

        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/16/4tZNq2FHRmX1c8f.png", roll2_x, roll2_y);
        outtextxy(roll2_x + 55, roll2_y + 12, "链接");
        roll2_y += 54;
        ImageLoader::drawImageFromURL(setting_roll2[0][1], roll2_x, roll2_y, true, [&]()
                                      { openUrlInDefaultBrowser("https://github.com/lhl77/billiards-online-cpp"); });
        settextcolor(WHITE);
        outtextxy(roll2_x + 13, roll2_y + 12, setting_roll2[0][0].c_str());

        EndBatchDraw();
    }
    // 绘制用户信息（主页面）
    void draw_user_info()
    {
        string message_type = "get_online_users";
        std::unordered_map<std::string, std::string> data;
        string getOnlineUsersMessage = jsonParser.createJson(message_type, data); // 构建json字符串
        // 每次回到主页面都获取在线用户列表
        client.sendMessage(getOnlineUsersMessage);

        // 读取用户名
        string username;
        ifstream file("login_info.txt");
        if (file.is_open())
        {
            getline(file, username);
            file.close();
        }
        else
        {
            cerr << "无法打开文件读取登录信息" << endl;
            return;
        }

        // 获取用户基本信息（金币、胜场数）
        stringstream url_get_user_info_stream;
        url_get_user_info_stream << API_URL << "/get_user_info?username=" << username;
        string url_get_user_info = url_get_user_info_stream.str();
        string response = sendGetRequest(url_get_user_info);

        string coins = jsonParser.extractJsonValue(response, "coins");
        string win_count = jsonParser.extractJsonValue(response, "win_count");
        string user_id = jsonParser.extractJsonValue(response, "user_id");

        // cerr << "coins: " << coins << ",win_count: " << ",user_id: " << user_id <<endl;
        if (coins.empty() || win_count.empty() || user_id.empty())
        {
            cerr << "获取用户信息失败" << endl;
            return;
        }

        // 获取用户角色信息
        stringstream url_get_user_character_set_stream;
        url_get_user_character_set_stream << API_URL << "/get_user_character_set?username=" << username;
        string url_get_user_character_set = url_get_user_character_set_stream.str();
        response = sendGetRequest(url_get_user_character_set);

        string character_id = jsonParser.extractJsonValue(response, "user_character_set");
        // cerr << character_id <<endl;
        if (character_id.empty())
        {
            cerr << "获取用户角色失败" << endl;
            return;
        }

        // 获取角色详细信息
        stringstream url_get_character_details_stream;
        url_get_character_details_stream << API_URL << "/get_character_details?id=" << character_id;
        string url_get_character_details = url_get_character_details_stream.str();

        response = sendGetRequest(url_get_character_details);

        string character_image_url = jsonParser.extractJsonValue(response, "url_big");
        string character_name = jsonParser.extractJsonValue(response, "name");
        string character_description = jsonParser.extractJsonValue(response, "description");

        // cerr << character_image_url <<","<< character_name <<","<< character_description << endl;
        if (character_image_url.empty() || character_name.empty() || character_description.empty())
        {
            cerr << "获取角色详情失败" << endl;
            return;
        }

        // 绘制主菜单背景图片
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/rH63hbFUVqIgNQG.png", 0, 0);

        // 绘制角色信息
        ImageLoader::drawImageFromURL(character_image_url, 30, 100, true, []()
                                      { button.go_to_SKIN_SHOP(); }); // 绘制角色图片
        // logger.info("成功绘制角色图片");

        /* 充值按钮 */
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/02/WZOfovSqQwxT5rV.png", 235, 18, true, []()
                                      { button.go_to_RECHARGE_SHOP(); });

        // 绘制角色名
        // 角色边框
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/09/w9ORf4udoPIZqjB.png", 75, 290, true, []()
                                      { button.go_to_SKIN_SHOP(); });
        set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);

        // 计算文字居中坐标
        int text_width = textwidth(character_name.c_str()); // 获取文字宽度
        int image_center_x = 75 + 88 / 2;                   // 图片水平中心点（假设图片宽度为 75）
        int text_x = image_center_x - text_width / 2;       // 文字居中坐标
        outtextxy(text_x, 300, character_name.c_str());

        /* 绘制用户名 */
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", 5, 5);
        set_custom_font(16, L"./packs/kenney-ui-pack/Font/Kenney Future.ttf", L"Kenney Future");
        outtextxy(20, 20, username.c_str());

        /* 设置字体 */
        set_custom_font(16, L"./packs/kenney-ui-pack/Font/Kenney Future.ttf", L"Kenney Future");
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);

        /* 绘制金币 */
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/Oo2f47wMFdJeklC.png", 145, 5);
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/KJ8XNW2VCoxpsIQ.png", 155, 19);
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/ov4E6nYmekp7rJz.png", 190, 5);
        int coins_text_x = 191 + (45 - textwidth(coins.c_str())) / 2;
        outtextxy(coins_text_x, 20, coins.c_str());

        /* 绘制胜场数 */
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/JpxbGusIPyZv7Qd.png", 265, 5);
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/ov4E6nYmekp7rJz.png", 310, 5);
        int win_text_x = 311 + (45 - textwidth(win_count.c_str())) / 2;
        outtextxy(win_text_x, 20, win_count.c_str());

        checkIfInRoomInMAINMENU();
    }
    // 绘制皮肤商城
    void draw_skin_shop(int switchCharacterID, bool refreshItems = false, bool showLoadingPage = false)
    {

        if (switchCharacterID == 0 & showLoadingPage == true)
        { // 只有在非切换角色界面才绘制loading图片
          // 绘制loading图片
          // 占位符
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/15/NuV3Op6TXw9rkUo.png", 0, 0);
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/02/vqmEKAhjc9rsPpZ.png", 5, 5);
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", 60, 5);
            set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(BLACK);
            setbkmode(TRANSPARENT);
            outtextxy(88, 17, "皮肤商城");
        };

        // 获取用户名
        string username;
        ifstream file("login_info.txt");
        if (file.is_open())
        {
            getline(file, username);
            file.close();
        }
        else
        {
            logger.error("无法打开文件读取登录信息");
            return;
        }

        // 查询用户角色 character_set_id
        stringstream url_get_user_character_set_stream;
        url_get_user_character_set_stream << API_URL << "/get_user_character_set?username=" << username;
        string url_get_user_character_set = url_get_user_character_set_stream.str();
        string response_get_user_character_set = sendGetRequest(url_get_user_character_set);

        character_set_id = stoi(jsonParser.extractJsonValue(response_get_user_character_set, "user_character_set"));

        // 查询用户全部角色 user_characters
        string url_get_user_characters = string(API_URL) + "/get_user_characters?username=" + username;
        string response_get_user_characters = sendGetRequest(url_get_user_characters);
        string user_characters = jsonParser.extractJsonValue(response_get_user_characters, "user_characters");

        // 获取用户金币数 coins
        stringstream url_get_coins_stream;
        url_get_coins_stream << API_URL << "/get_coins?username=" << username;
        string url_get_coins = url_get_coins_stream.str();
        string response_get_coins = sendGetRequest(url_get_coins);

        int coins = stoi(jsonParser.extractJsonValue(response_get_coins, "coins"));

        // 将全局变量global_user_id赋值到user_id
        int character_current_number;

        // 设置current_character_number来判断显示哪个皮肤
        if (switchCharacterID == 0)
        {
            character_current_number = character_set_id; // 目前界面的皮肤id
        }
        else
        {
            character_current_number = switchCharacterID; // 将character_current_number设置为传入的switchCharacterID
        }
        switch_skin_shop_page(coins, character_set_id, user_characters, character_current_number);
    }
    // 皮肤商城切换角色函数（不调用API减少卡顿）
    void switch_skin_shop_page(int coins, int character_set_id, string user_characters, int character_current_number)
    {
        // 先清理画布
        ImageLoader::clearAll();

        BeginBatchDraw(); // 开始批量绘制
        cleardevice();

        // 绘制函数，对于character_current_number可以切换当前的界面
        if (auto item = getCharacterById(character_current_number))
        {

            // 画角色对应的bg
            ImageLoader::drawImageFromURL(item->url_bg, 0, 0);
            // 画角色的小url
            ImageLoader::drawImageFromURL(item->url_small, 60, 80);
            // 画角色名+角色描述
            set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(BLACK);
            setbkmode(TRANSPARENT);
            // 绘制角色名
            int character_name_x = 60 + (100 - textwidth(item->name.c_str())) / 2;
            outtextxy(character_name_x, 190, item->name.c_str());
            set_custom_font(14, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            // 绘制角色描述
            drawTextWithWrap(35, 220, item->description, 160);

            // **皮肤ID切换**
            // 获取当前id到currentPageId
            int currentPageId = stoi(item->id);
            // 绘制页码
            set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            outtextxy(90, 313, to_string(currentPageId).c_str());
            outtextxy(108, 313, "/");
            outtextxy(121, 313, to_string(characters_total_number).c_str());

            // 角色的使用/使用中/购买按钮
            if (currentPageId == character_set_id)
            {
                // cerr << "已设定" << endl;
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/14/y9CR1M4OglutWjA.png", 645, 5, true, []()
                                              { button.change_page_button_in_SKIN_SHOP(); });
                set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
                settextcolor(WHITE);
                outtextxy(685, 17, "使用中");
            }
            else if (isIdInString(user_characters, to_string(currentPageId)))
            {
                // cerr << "拥有未设定" << endl;
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/14/tuCLnWNxsmkU6qE.png", 683, 5, true, [=]()
                                              {
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/14/tuCLnWNxsmkU6qE.png",683,5);
        set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(WHITE);
        outtextxy(695,17,"切换中");
        button.change_page_button_in_SKIN_SHOP();
        update_user_character(currentPageId,global_user_id);   //更新用户角色
        ImageLoader::clearAll();
        draw_skin_shop(0); });
                set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
                settextcolor(WHITE);
                outtextxy(707, 17, "使用");
            }
            else
            {
                // cerr << "未拥有" << endl;
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/14/mpSykqXuBvjNH73.png", 646, 5, true, [=]()
                                              {
        button.change_page_button_in_SKIN_SHOP();
        if(coins<stoi(item->price)){
            //提示金币不足
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/15/XYixOCkhF5n8Z4f.png",521,5);
            set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(WHITE);
            outtextxy(554,16,"金币不足");
            //充值入口
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/15/PvAcDduhZi5TJUm.png",396,5,true,[](){
                button.go_to_RECHARGE_SHOP();
            });
            set_custom_font(17, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(WHITE);
            outtextxy(430,16,"充值金币");
        }else{
            //确认购买按钮
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/15/SxHN58MpUk7Ormj.png",521,5,true,[=](){
                //购买函数
                button.change_page_button_in_SKIN_SHOP();
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/15/SxHN58MpUk7Ormj.png",521,5);
                set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
                settextcolor(BLACK);
                outtextxy(535,18,"处理交易中");
                buy_character_by_id(coins,global_user_id,currentPageId,stoi(item->price));
                update_user_character(currentPageId,global_user_id);
                ImageLoader::clearAll();
                draw_skin_shop(0);
            });
            set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(BLACK);
            outtextxy(540,18,"确认购买?");
        } });
                set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
                settextcolor(WHITE);
                int character_price_x = 655 + (20 - textwidth(item->price.c_str())) / 2;
                outtextxy(character_price_x, 17, item->price.c_str());
                // cerr << item->price << endl;
                set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
                outtextxy(712, 17, "购买");
            }

            // 角色切换按钮
            if (currentPageId != 1)
            { // 如果界面id是1，那么不显示左按钮
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/13/Vsm9rktM1uYASQe.png", 30, 300, true, [=]()
                                              {
        button.change_page_button_in_SKIN_SHOP();
        int newPageId = currentPageId - 1;
        switch_skin_shop_page(coins,character_set_id,user_characters,newPageId); }); // 左按钮
            }

            if (currentPageId != characters_total_number)
            { // 如果界面id等于总角色数，不显示右按钮
                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/13/4JYTMRofy2jaSIP.png", 150, 300, true, [=]()
                                              {
        button.change_page_button_in_SKIN_SHOP();
        int newPageId = currentPageId + 1;
        switch_skin_shop_page(coins,character_set_id,user_characters,newPageId); }); // 右按钮
            }

            // 绘制皮肤商城内容
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/02/vqmEKAhjc9rsPpZ.png", 5, 5, true, []()
                                          { button.go_to_MAIN_MENU(); }); // 返回键
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", 60, 5);
            set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(BLACK);
            setbkmode(TRANSPARENT);
            outtextxy(88, 17, "皮肤商城");
            // 金币边框
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/Oo2f47wMFdJeklC.png", 215, 5);
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/KJ8XNW2VCoxpsIQ.png", 225, 19);
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/ov4E6nYmekp7rJz.png", 260, 5);

            // 绘制金币数
            settextcolor(BLACK);
            setbkmode(TRANSPARENT);
            set_custom_font(16, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            int coins_text_x = 261 + (45 - textwidth(to_string(coins).c_str())) / 2;
            outtextxy(coins_text_x, 20, to_string(coins).c_str());
        }
        else
        {
            logger.error("无法获取当前的皮肤ID");
        }

        EndBatchDraw();
    }
    // 绘制充值商城
    void draw_recharge_shop()
    {

        BeginBatchDraw(); // 开始批量绘制
        cleardevice();
        // 绘制充值商城背景图片
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/rH63hbFUVqIgNQG.png", 0, 0);
        // 绘制返回主界面按钮
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/02/vqmEKAhjc9rsPpZ.png", 5, 5, true, [&]()
                                      { button.go_to_MAIN_MENU(); });
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", 60, 5);
        set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);
        outtextxy(88, 17, "充值商城");
        // 金币边框
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/Oo2f47wMFdJeklC.png", 215, 5);
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/KJ8XNW2VCoxpsIQ.png", 225, 19);
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/ov4E6nYmekp7rJz.png", 260, 5);
        // 绘制充值二维码背景
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/6yHDwNIuRs23oZa.png", 590, 80);

        // 获取用户金币数并绘制
        string username;
        ifstream file("login_info.txt");
        if (file.is_open())
        {
            getline(file, username);
            file.close();
        }
        else
        {
            logger.error("无法打开文件读取登录信息");
            return;
        }

        // 获取用户金币数
        stringstream url_get_coins_stream;
        url_get_coins_stream << API_URL << "/get_coins?username=" << username;
        string url_get_coins = url_get_coins_stream.str();
        string response_get_coins = sendGetRequest(url_get_coins);

        int coins = stoi(jsonParser.extractJsonValue(response_get_coins, "coins"));

        // GET请求获取商品信息
        string url_get_recharge_items = string(API_URL) + "/get_recharge_items";

        // // 查询用户id（暂时）
        // string url_check_id = string(API_URL) + "/get_user_id";
        // string data_check_id = "username=" + username;
        // string response_id = sendPostRequest(url_check_id, data_check_id);
        // // cerr << response_id << endl;
        // int user_id = stoi(jsonParser.extractJsonValue(response_id, "user_id"));
        // // cerr << user_id << endl;

        // 将全局变量global_user_id赋值到user_id
        int user_id = global_user_id;

        // 绘制金币数
        set_custom_font(16, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        int coins_text_x = 261 + (45 - textwidth(to_string(coins).c_str())) / 2;
        outtextxy(coins_text_x, 20, to_string(coins).c_str());

        // 示例：绘制充值商城商品
        int y_offset = 60; // 初始 y 坐标

        set_custom_font(14, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);

        for (const auto &item : rechargeItems)
        {
            // 绘制商品图片
            ImageLoader::drawImageFromURL(item.image_url, 30, y_offset, false);

            // 绘制商品信息
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/03/gwbjP56Mu7AWs4Y.png", 128, y_offset + 10);

            string info1 = to_string(item.coin_amount) + "金币";
            stringstream stream;
            stream << fixed << setprecision(0) << item.price;
            string info2 = "  " + stream.str() + " 元";
            outtextxy(135, y_offset + 20, info1.c_str());
            outtextxy(135, y_offset + 50, info2.c_str());

            // 绘制购买按钮
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/03/PvIfru6ioRhzLYV.png", 210, y_offset + 20, true, [=]()
                                          {
            button.buy_button_in_RECHARGE_SHOP();
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2);  // 强制两位小数格式
            ss << item.price;
            stringstream orderInfo;
            orderInfo << "台球游戏充值" << item.coin_amount << "金币";
            string result_of_qrcode_and_order_id = alipay.generateQRcode(ss.str(), orderInfo.str());
            string order_id = jsonParser.extractJsonValue(result_of_qrcode_and_order_id, "orderNo");
            string qr_code = jsonParser.extractJsonValue(result_of_qrcode_and_order_id, "qr_code");
            ImageLoader::clearAll();
            cleardevice();
            // 绘制loading图片
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/NOZgC3SahLqTPGK.jpg",0,0);
            //再次绘制支付二维码背景防止被loading覆盖
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/6yHDwNIuRs23oZa.png",590,80);
            //进入充值界面
            currentState = PAYING_PAGE;
            draw_paying_page(qr_code,order_id,ss.str()); });
            y_offset += 100; // 更新 y 坐标，避免重叠
        }
        EndBatchDraw();
    }
    // 绘制充值界面
    void draw_paying_page(string pay_url, string order_id, string price)
    {
        BeginBatchDraw();
        cleardevice();
        // 绘制充值商城背景图片
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/rH63hbFUVqIgNQG.png", 0, 0);
        // 绘制充值二维码背景
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/6yHDwNIuRs23oZa.png", 590, 80);
        // 绘制返回主界面按钮
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/02/vqmEKAhjc9rsPpZ.png", 5, 5, true, [&]()
                                      { button.go_to_RECHARGE_SHOP(); });
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", 60, 5);
        set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);
        outtextxy(88, 17, "充值界面");

        // 购买详情
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/7OBJl293ygKcmeD.png", 28, 87);
        stringstream recharge_info;
        recharge_info << "充值金额 " << price << " 元";
        outtextxy(48, 100, recharge_info.str().c_str());

        // 购买状态，默认为未支付
        set_custom_font(15, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", 28, 225);
        outtextxy(42, 240, "支付状态");
        settextcolor(RED);
        outtextxy(106, 240, "未扫码");

        // 刷新购买状态按钮
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/V3ZIapxgBWRKSw9.png", 177, 225, true, [order_id, price]()
                                      {
        //播放按钮声音（下面这个方法中只播放声音的目前）
        button.buy_button_in_RECHARGE_SHOP(); 
        //调用支付接口，获取订单状态
        string status = alipay.queryOrderStatus(order_id);
        //判断支付状态并输出
        if (status == "TRADE_SUCCESS") {
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png",28,225);
            settextcolor(BLACK);
            outtextxy(42, 240, "支付状态");
            settextcolor(GREEN);
            outtextxy(106,240,"已支付");
            int add_coins_number;
            stringstream ss(price);
            ss >> add_coins_number;
            user_account.add_coins_to_user(add_coins_number*10);
            currentState = MAIN_MENU;
            Sleep(1000);
            stateSwitch();
        } else if (status == "WAIT_BUYER_PAY") {
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png",28,225);
            settextcolor(BLACK);
            outtextxy(42, 240, "支付状态");
            settextcolor(RGB(224, 135, 45));
            outtextxy(106,240,"未支付");
        } else if (status == "error"){
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png",28,225);
            settextcolor(BLACK);
            outtextxy(42, 240, "支付状态");
            settextcolor(RED);
            outtextxy(106,240,"未扫码");
        }else  {
            // cerr << "未知状态: " << status << endl;
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png",28,225);
            settextcolor(BLACK);
            outtextxy(42, 240, "支付状态");
            settextcolor(RED);
            outtextxy(106,240,"未知");
        } });

        EndBatchDraw();

        // 画支付二维码，调用API有延迟，故这个放在最后调用
        pay_url.erase(std::remove(pay_url.begin(), pay_url.end(), '\\'), pay_url.end());
        ImageLoader::drawImageFromURL("https://api.qrtool.cn/?text=" + pay_url + "&size=150", 600, 90);
        // 欢迎使用支付宝图片
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/T6mv9U1YtnBciDd.png", 591, 250);
    }

    // 绘制游戏准备界面（GAME_PREPARING)
    void draw_game_preparing_page()
    {
        BeginBatchDraw();
        cleardevice();
        // 绘制游戏准备画面背景图片
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/24/ARG8Z92qeiOaMJv.png", 0, 0);
        if (!isInRoom)
        {
            // 绘制返回主界面按钮
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/02/vqmEKAhjc9rsPpZ.png", 5, 5, true, [=]()
                                          { current_user_list_page_id=0;currentState=MAIN_MENU;button.go_to_MAIN_MENU(); });
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", 60, 5);
            set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(BLACK);
            setbkmode(TRANSPARENT);
            outtextxy(88, 17, "对战组队");
        }
        else
        {
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/lb3LMm6Xfu89Q2j.png", 5, 5);
            set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(BLACK);
            setbkmode(TRANSPARENT);
            outtextxy(33, 17, "对战组队");
        }

        set_custom_font(23, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        outtextxy(460, 50, "在线玩家列表");

        // Online user IDs

        // online_users_arr = {1,20,21,22,23,24,25,26,27}; //这里给简单赋值是调试用的
        // online_users_arr自动获取并赋值，在client_server.h中

        int page_id = current_user_list_page_id; // 当前页面id

        draw_user_list(online_users_arr, page_id, global_user_id);

        // 绘制房间详情（头像和开始按钮）
        draw_room_info();
        EndBatchDraw();
    }

    // 绘制游戏准备界面中的在线用户列表
    void draw_user_list(const std::vector<int> &online_users_arr, int page_id, int my_user_id)
    {
        // 绘制刷新按钮
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/V3ZIapxgBWRKSw9.png", USER_LIST_X + USER_LIST_WIDTH - 45, USER_LIST_Y + USER_LIST_HEIGHT + 10, true, [=]()
                                      {
                                          button.only_play_button_sound();
                                          // 如果在第一页，跳转到最后一页，否则跳转到上一页
                                          string message_type = "get_online_users";
                                          std::unordered_map<std::string, std::string> data;
                                          string getOnlineUsersMessage = jsonParser.createJson(message_type, data); // 构建json字符串
                                          client.sendMessage(getOnlineUsersMessage);
                                          current_user_list_page_id = 0;
                                          button.go_to_GAME_PREPARING(); // 更新准备界面
                                      });

        int total_users = online_users_arr.size();
        if (total_users == 0)
        {
            set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            outtextxy(USER_LIST_X + 10, USER_LIST_Y, "暂无其他在线玩家...");
            return;
        } // 如果没有用户，直接返回

        int total_pages = (total_users + MAX_USERS_PER_PAGE - 1) / MAX_USERS_PER_PAGE; // 计算总页数

        // 确保 page_id 在合法范围
        if (page_id < 0 || page_id >= total_pages)
            return;

        // 根据页面ID计算显示的用户范围
        int start_index = page_id * MAX_USERS_PER_PAGE;
        int end_index = std::min(start_index + MAX_USERS_PER_PAGE, total_users);

        int y_position = USER_LIST_Y;

        // 绘制在线用户名
        for (int i = start_index; i < end_index; ++i)
        {
            if (online_users_arr[i] != my_user_id)
            { // 不显示自己的信息,虽然已经删除了自己的id
                settextcolor(BLACK);
                set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");

                string url_get_username = string(API_URL) + "/get_username?user_id=" + to_string(online_users_arr[i]); // 获取user_id对应的username
                string response_get_username = sendGetRequest(url_get_username);
                string username = jsonParser.extractJsonValue(response_get_username, "username");
                // outtextxy(USER_LIST_X+10, y_position, std::to_string(online_users_arr[i]).c_str());
                outtextxy(USER_LIST_X + 10, y_position, username.c_str());

                // 绘制邀请按钮，注意，这里的user_id是 to_string(online_users_arr[i]
                string message_type = "invite";
                std::unordered_map<std::string, std::string> data;
                data["user_id"] = to_string(online_users_arr[i]);                 // 被邀请者的用户ID
                string inviteMessage = jsonParser.createJson(message_type, data); // 构建json字符串

                ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/24/zQj8HvTtSJADgVm.png", USER_LIST_X + 233, y_position - 5, true, [=]()
                                              {
                    button.only_play_button_sound();
                    client.sendMessage(inviteMessage); });

                set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
                settextcolor(WHITE);
                outtextxy(USER_LIST_X + 250, y_position + 2, "邀请");

                y_position += USER_NAME_Y_OFFSET; // 调整下一个用户名的位置
            }
        }

        // 绘制上一页按钮
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/13/Vsm9rktM1uYASQe.png",
                                      USER_LIST_X, USER_LIST_Y + USER_LIST_HEIGHT + 10, true, [=]()
                                      {
                                          button.only_play_button_sound();
                                          // 如果在第一页，跳转到最后一页，否则跳转到上一页
                                          int new_page_id = (page_id == 0) ? total_pages - 1 : page_id - 1;
                                          current_user_list_page_id = new_page_id;
                                          button.go_to_GAME_PREPARING(); // 更新准备界面
                                      });

        // 绘制下一页按钮
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/13/4JYTMRofy2jaSIP.png",
                                      USER_LIST_X + USER_LIST_WIDTH - 100, USER_LIST_Y + USER_LIST_HEIGHT + 10, true, [=]()
                                      {
                                          button.only_play_button_sound();
                                          // 如果在最后一页，跳转到第一页，否则跳转到下一页
                                          int new_page_id = (page_id == total_pages - 1) ? 0 : page_id + 1;
                                          current_user_list_page_id = new_page_id;
                                          button.go_to_GAME_PREPARING(); // 更新准备界面
                                      });
    }

    // 绘制房间详情
    void draw_room_info()
    {
        // 用户头像框
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/27/V38ncFQGhtSmYT7.png", 50, 120);

        if (auto item = getCharacterById(character_set_id))
        {
            ImageLoader::drawImageFromURL(item->url_small, 60, 130);
        }
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/27/V38ncFQGhtSmYT7.png", 220, 120);

        if (isInRoom)
        {

            // 从room_users中提出enemy_user_id
            if (room_users.size() != 2)
            {
                std::cerr << "Error: Expected 2 users, but got " << room_users.size() << std::endl;
                return;
            }

            int enemy_user_id = (room_users[0] == global_user_id) ? room_users[1] : room_users[0];
            cerr << "对手ID：" << enemy_user_id << endl;

            // 查询对方的username
            string url_check_enemy_username = string(API_URL) + "/get_username?user_id=" + to_string(enemy_user_id);
            string response_enemy_username = sendGetRequest(url_check_enemy_username);
            string enemy_username = jsonParser.extractJsonValue(response_enemy_username, "username");
            cerr << "对手username: " << enemy_username << endl;

            // 查询用户角色 character_set_id
            stringstream url_get_enemy_character_set_stream;
            url_get_enemy_character_set_stream << API_URL << "/get_user_character_set?username=" << enemy_username;
            string url_get_enemy_character_set = url_get_enemy_character_set_stream.str();
            string response_get_enemy_character_set = sendGetRequest(url_get_enemy_character_set);

            string enemy_character_set_id_str = jsonParser.extractJsonValue(response_get_enemy_character_set, "user_character_set");

            enemy_character_set_id = stoi(enemy_character_set_id_str);

            if (auto item = getCharacterById(enemy_character_set_id))
            {
                ImageLoader::drawImageFromURL(item->url_small, 230, 130);
            }

            // leave_room消息构建
            std::unordered_map<std::string, std::string> leaveRoom_data;
            leaveRoom_data["user_id"] = to_string(global_user_id);                         // 被邀请者的用户ID
            string leaveRoomMessage = jsonParser.createJson("leave_room", leaveRoom_data); // 构建json字符串

            // 退出房间
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/28/Cyp5uLwqEOTi1FR.png", 50, 250, true, [=]()
                                          {
button.only_play_button_sound();
client.sendMessage(leaveRoomMessage); });
            settextcolor(WHITE);
            set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            outtextxy(70, 260, "退出房间");

            // room_game_start消息构建
            std::unordered_map<std::string, std::string> roomGameStart_data;
            roomGameStart_data["room_id"] = to_string(room_id);                                         // 被邀请者的用户ID
            string roomGameStartMessage = jsonParser.createJson("room_game_start", roomGameStart_data); // 构建json字符串

            // 开始对战
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/28/gLjYeyRSQt1VsP8.png", 220, 250, true, [=]()
                                          {
                button.only_play_button_sound();
                client.sendMessage(roomGameStartMessage); });
            outtextxy(240, 260, "开始对战");
        }
        else
        {
            // 退出房间
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/28/6UjAZVSklRFnc1s.png", 50, 250);
            settextcolor(WHITE);
            set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            outtextxy(70, 260, "退出房间");

            // 开始对战
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/28/6UjAZVSklRFnc1s.png", 220, 250);
            outtextxy(240, 260, "开始对战");
        }
    }

    // 游戏运行函数
    void game_running()
    {
        game_client();
    }

    void game_client()
    {
        // 初始化游戏运行标志
        g_GameRunning.store(true);

        // 在这里启动新线程来运行游戏逻辑
        std::thread gameThread([=]()
                               {

        // 初始化游戏实例 
        PoolGame::Game game;

        BeginBatchDraw(); // 开始批量绘制
        // cleardevice();

        //游戏主循环
        while (g_GameRunning.load() && currentState == GAME_RUNNING)
        {

            // 处理本地输入
            game.handleInput();

            // 更新游戏状态
            // game.update(0.016f); // 假设60FPS，每帧16ms
            game.update(0.008f); // 假设120FPS，每帧8ms

            // 绘制逻辑带互斥锁
            {
                std::lock_guard<std::mutex> lock(g_GraphicsMutex);
                if (game.needsRedraw())
                {
                    // 清屏
                    cleardevice();
    
                    // 绘制背景
                    if (auto item = getCharacterById(character_set_id))
                    {
                        string game_bg_url = item->url_game_bg;
                        ImageLoader::drawImageFromURL(game_bg_url, 0, 0);
                        // 绘制状态栏
                        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/12/7IESuL8iZJtjzqD.png",281.8,15);
                        ImageLoader::drawImageFromURL(item->url_small, 5, 5);
                        
                    }
                    if (auto item = getCharacterById(enemy_character_set_id))
                    {
                        ImageLoader::drawImageFromURL(item->url_small, 700, 5);
                    }

                    // 绘制游戏
                    game.draw();
    
                    // 刷新显示（一次性将内存中的绘制结果输出到屏幕）
                    FlushBatchDraw();
    
                    game.resetRedrawFlag();
                }
            }
            
            // 非阻塞式帧率控制
            auto frameStart = std::chrono::high_resolution_clock::now();
            std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(16));

            // 检查游戏状态
            // if (game.getState() == PoolGame::BallGameState::GameOver)
            // {
            //     break; // 游戏结束退出循环
            // }
            // 检查游戏结束状态
            if (game.getState() == PoolGame::BallGameState::GameOver) {
                g_GameRunning.store(false);
                // 然后切换游戏状态，再stateSwitch()
                cerr << "main.cpp中获取到了gameOver" << endl;
                cerr << game.getIsWin() << endl;
                draw_game_over_page(game.getIsWin());
                break;
            }

        }
        // 结束批量绘制
        EndBatchDraw(); });
        gameThread.detach(); // 分离线程，允许它在后台运行

        // 等待游戏线程完成（如果你想在此处等待线程结束）
        // if (gameThread.joinable())
        // {
        //     gameThread.join();
        // }
        // 重置游戏状态
        // currentState = MAIN_MENU;
        // stateSwitch();
    }

    void draw_game_over_page(bool isWin)
    {
        currentState = GAME_OVER;
        BeginBatchDraw();
        cleardevice();
        if (isWin)
        {
            // ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/15/eHOTD8VF9QCUmhB.jpg", 0, 0);// 有文字
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/15/8Wo3kghqL6FAzGK.png", 0, 0); // 没文字只有背景
            set_custom_font(38, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(RGB(128, 53, 14));
            outtextxy(360, 50, "完胜");
            set_custom_font(144, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            outtextxy(165, 75, "VICTORY");

            // 返回房间按钮
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/28/gLjYeyRSQt1VsP8.png", 343, 320, true, [=]()
                                          { button.go_to_GAME_PREPARING(); });
            settextcolor(WHITE);
            set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            outtextxy(363, 330, "返回房间");

            set_custom_font(24, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(BLACK);
            outtextxy(240, 252, "您获得:");

            // 金币
            set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/Oo2f47wMFdJeklC.png", 345, 245);
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/KJ8XNW2VCoxpsIQ.png", 355, 259);
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/ov4E6nYmekp7rJz.png", 390, 245);
            outtextxy(403, 257, "+2");

            // 胜场数
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/JpxbGusIPyZv7Qd.png", 465, 245);
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/ov4E6nYmekp7rJz.png", 510, 245);
            outtextxy(523, 257, "+1");
        }
        else
        {
            // ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/15/rVswZitpuJ9XgCq.jpg", 0, 0); // 有文字
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/04/15/1TS8IjxhXgzuKAk.png", 0, 0); // 没文字只有背景
            set_custom_font(38, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            settextcolor(RGB(174, 174, 174));
            outtextxy(360, 80, "败北");
            set_custom_font(144, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            outtextxy(190, 105, "DEFEAT");

            // 返回房间按钮
            ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/28/gLjYeyRSQt1VsP8.png", 343, 300, true, [=]()
                                          { button.go_to_GAME_PREPARING(); });
            settextcolor(WHITE);
            set_custom_font(19, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
            outtextxy(363, 310, "返回房间");
        }

        EndBatchDraw();

        // 调用api
        user_account.update_total_count(isWin);
    }

private:
    int current_user_list_page_id = 0; // 用于跟踪当前页面IDF
    const int MAX_USERS_PER_PAGE = 4;  // 每页最多显示的用户数量
    const int USER_LIST_X = 450;       // 用户列表的起始X坐标
    const int USER_LIST_Y = 100;       // 用户列表的起始Y坐标
    const int USER_LIST_WIDTH = 300;   // 用户列表宽度
    const int USER_LIST_HEIGHT = 150;  // 用户列表高度
    const int USER_NAME_Y_OFFSET = 40; // 每个用户名的Y偏移量

    // 皮肤商城变量

    // 判断某一个id的character用户是否拥有
    bool isIdInString(const std::string &str, const std::string &targetId)
    {
        std::istringstream iss(str);
        std::string token;

        while (std::getline(iss, token, ','))
        {
            std::string cleaned = trim(token);
            if (!cleaned.empty() && cleaned == targetId)
            {
                return true;
            }
        }
        return false;
    }
    // 封装函数：根据 ID 查询皮肤信息（返回 optional 避免无效值）
    std::optional<CharacterItem> getCharacterById(int targetId)
    {
        for (const auto &item : characterItem)
        {
            if (stoi(item.id) == targetId)
            {
                return item; // 找到则返回对应项
            }
        }
        return std::nullopt; // 未找到返回空值
    }
    // 给已经登录用户更换character
    void update_user_character(int new_character_id, int user_id)
    {
        // 读取用户名
        string username;
        string password;
        ifstream file("login_info.txt");
        if (file.is_open())
        {
            getline(file, username);
            getline(file, password);
            file.close();
        }
        else
        {
            logger.error("无法打开文件读取登录信息");
            return;
        }

        // 调用api
        string url_update_character = string(API_URL) + "/update_set_skin";
        string data_update_character = "username=" + username + "&password=" + password + "&skin_id=" + to_string(new_character_id);
        string response_update_character = sendPostRequest(url_update_character, data_update_character);
        while (jsonParser.success(response_update_character) == false)
        {
            cerr << "更换皮肤失败: " << jsonParser.error(response_update_character) << endl;
            cerr << "正在重试..." << endl;
            string response_update_character = sendPostRequest(url_update_character, data_update_character);
        }
    }
    // 皮肤购买按钮函数
    void buy_character_by_id(int current_coins, int user_id, int item_id, int item_price)
    {
        // 读取用户名
        string username;
        string password;
        ifstream file("login_info.txt");
        if (file.is_open())
        {
            getline(file, username);
            getline(file, password);
            file.close();
        }
        else
        {
            logger.error("无法打开文件读取登录信息");
            return;
        }

        string url_buy_character_by_id = string(API_URL) + "/buy_character.php";
        string data_buy_character_by_id = "username=" + username +
                                          "&user_id=" + to_string(user_id) +
                                          "&item_id=" + to_string(item_id) +
                                          "&item_price=" + to_string(item_price);

        string response = sendPostRequest(url_buy_character_by_id, data_buy_character_by_id);

        // cout << "服务器响应: " << response << endl;
    }
    // 辅助函数：去除字符串首尾空白字符
    std::string trim(const std::string &s)
    {
        auto start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos)
            return "";

        auto end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    }

    // 创建变量名到变量引用的映射
    std::unordered_map<std::string, bool *> boolVariables = {
        {"setting_main_bgm", &setting_main_bgm},
        {"setting_play_bgm", &setting_play_bgm},
        {"setting_click_sound", &setting_click_sound},
        {"setting_nontification_sound", &setting_nontification_sound},
        {"setting_ball_sound", &setting_ball_sound}};
    // 字符串到布尔值转换函数
    bool stringToBool(const std::string &varName)
    {
        auto it = boolVariables.find(varName);
        if (it != boolVariables.end())
        {
            return *(it->second); // 解引用指针获取值
        }
        throw std::invalid_argument("未知的布尔变量名: " + varName);
    }
    void openUrlInDefaultBrowser(const std::string &url)
    {
        ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
};
DrawPages drawPages;

// 游戏状态切换函数
void stateSwitch()
{
    // 清空绘图窗口
    ImageLoader::clearAll(); // 清空所有可点击图片的实例
    // cleardevice();

    switch (currentState)
    {
    case SETTING:
        if (setting_main_bgm)
        {
            playBGM(MAIN_BGM);
        }
        else
        {
            playBGM("", true, true);
        };
        ;
        // 绘制设置界面
        drawPages.draw_setting_page();

        break;
    case MAIN_MENU:

        if (setting_main_bgm)
        {
            playBGM(MAIN_BGM);
        }
        else
        {
            playBGM("", true, true);
        };
        BeginBatchDraw(); // 开始批量绘制
        // 绘制loading图片
        cleardevice();
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/NOZgC3SahLqTPGK.jpg", 0, 0);

        // 绘制用户信息
        drawPages.draw_user_info();
        // 绘制设置按钮
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/26/7SDgi9yMenHoxhz.png", 735, 5, true, []()
                                      { button.go_to_SETTING(); });

        // 绘制皮肤商城图标
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/03/HOxQKXN4kuYgsGp.png", 590, 5, true, []()
                                      { button.go_to_SKIN_SHOP(); });
        set_custom_font(20, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);
        outtextxy(618, 17, "皮肤商城");

        // 绘制对战按钮
        set_custom_font(25, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/24/HtrI81walKQd2LD.png", 662, 55, true, []()
                                      { button.go_to_GAME_PREPARING(); });
        outtextxy(715, 69, "对战");
        EndBatchDraw();
        break;
    case GAME_RUNNING:
        if (setting_play_bgm)
        {
            playBGM(PLAY_BGM);
        };
        // 绘制游戏运行时的背景图片
        drawPages.game_running();
        break;
    case GAME_OVER:
        // 绘制游戏结束时的背景图片
        // 绘制游戏结束内容
        drawPages.draw_game_over_page(true); // 这里是默认的false，因为从game_running到这里不调用stateSwtich而是直接调用方法
        break;
    case SKIN_SHOP:
        if (setting_main_bgm)
        {
            playBGM(MAIN_BGM);
        }
        else
        {
            playBGM("", true, true);
        };
        drawPages.draw_skin_shop(0, true, true);
        break;
    case GAME_PREPARING:
        // AudioManager::stop("main_bgm");
        if (setting_main_bgm)
        {
            playBGM(MAIN_BGM);
        }
        else
        {
            playBGM("", true, true);
        };
        // 绘制loading图片
        // ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/NOZgC3SahLqTPGK.jpg", 0, 0);
        // 绘制游戏准备中内容
        drawPages.draw_game_preparing_page();
        break;
    case RECHARGE_SHOP:
        if (setting_main_bgm)
        {
            playBGM(MAIN_BGM);
        }
        else
        {
            playBGM("", true, true);
        };
        // 绘制loading图片
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/08/NOZgC3SahLqTPGK.jpg", 0, 0);
        // 绘制皮肤商城内容
        drawPages.draw_recharge_shop();
        break;
    }
}

// 新增系统消息处理函数
void processSystemMessages()
{
    static std::mutex msgMutex;
    static std::queue<std::string> msgQueue;

    // 从网络线程获取消息
    {
        std::lock_guard<std::mutex> lock(msgMutex);
        // 这里需要与WebSocketClient的消息队列对接
        // 根据实际消息队列实现进行数据交换
    }

    // 处理消息
    while (!msgQueue.empty())
    {
        auto msg = msgQueue.front();
        msgQueue.pop();

        // 处理强制下线等系统消息
        if (msg.find("force_logout") != string::npos)
        {
            drawMessageBox("强制下线", "检测到重复登录", []()
                           { exit(EXIT_FAILURE); });
        }
    }
}

// bool setting_main_bgm;
// bool setting_play_bgm;
// bool setting_click_sound;
// bool setting_nontification_sound;

// 初始化设置
void initSetting()
{
    try
    {
        BoolConfigManager config("app_settings.txt");

        setting_main_bgm = config.getSetting("setting_main_bgm");
        setting_play_bgm = config.getSetting("setting_play_bgm");
        setting_click_sound = config.getSetting("setting_click_sound");
        setting_nontification_sound = config.getSetting("setting_nontification_sound");
        setting_ball_sound = config.getSetting("setting_ball_sound");
        cerr << "成功载入设置" << endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "错误: " << e.what() << std::endl;
    }
}
// 检测EasyX窗口是否关闭的函数(只有windows11才会出现问题)
bool isEasyXWindowClosed()
{
    // 尝试获取窗口句柄
    HWND hWnd = GetHWnd();

    // 如果获取不到句柄或窗口不可见，则认为窗口已关闭
    if (hWnd == NULL || !IsWindowVisible(hWnd))
    {
        return true;
    }

    // 检查是否有窗口关闭消息
    if (peekmessage(NULL, WM_CLOSE))
    {
        closegraph();
        return true;
    }

    return false;
}

int main()
{
    // 检查cache目录是否存在
    const char *cache_dir = "images/cache"; // cache目录名称
    if (_access(cache_dir, 0) != 0)
    {
        _mkdir(cache_dir) == 0;
    };

    // 设置控制台输出为 UTF-8 编码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 设置中文字符集
    setlocale(LC_ALL, "zh_CN.UTF-8");

    // 打印logo
    print_logo();

    // 处理用户登录（带文件保存功能）
    user_account.handle_login_with_file();

    // 获取设置
    initSetting();

    // 启动统一监听线程
    ImageLoader::startEventLoop();

    // 绘制loading图片
    resizeImage("./images/loading.png", 0, 0, 800, 400);
    // 提前加载在线图片
    preload.preload_online_image();
    preload.preloadCharacterItem();
    preload.preloadRechargeItem();
    preload.preLoadUserCharacter();
    // Sleep(500); // 加载完成后停留 0.5 秒

    user_account.startClientServer();

    // 主页面
    stateSwitch();

    while (true)
    {
        // 处理其他逻辑
        switch (currentState)
        {
        default:
            // 处理消息队列
            processSystemMessages();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            break;
        }
        // 检查窗口是否关闭
        if (isEasyXWindowClosed())
        {
            std::cout << "终端关闭" << std::endl;
            // throw std::runtime_error("强制终止程序");
            TerminateProcess(GetCurrentProcess(), 0);
            // std::abort();
            break;
        }
    }
    // 清理逻辑
    g_NetworkRunning.store(false);
    if (g_NetworkThread.joinable())
        g_NetworkThread.join();
    return 0;
}

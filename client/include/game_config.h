#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#pragma once
#include <atomic>
#include <mutex>

std::string gameVersion = "v1.0";

int global_user_id;                // 声明全局user_id变量
int characters_total_number;       // 声明角色皮肤总数
std::vector<int> online_users_arr; // 声明在线用户vector数组
bool isInRoom = false;             // 是否在房间内
int room_id;
int enemy_character_set_id;
int character_set_id; // 用户设置的character
std::vector<int> room_users;
std::vector<int> randomBallIds = {1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15};
bool isLogMouseEventInGame = false;
int current_player_id;

std::string MAIN_BGM = ".\\packs\\main.wav";
std::string PLAY_BGM = ".\\packs\\play_bgm.wav";
std::string POCKET_SOUND = ".\\packs\\pocket.wav";
std::string CUEHIT_SOUND = ".\\packs\\cuehit.wav";
std::string BALLHIT_SOUND = ".\\packs\\ballhit.wav";
std::string WALLHIT_SOUND = ".\\packs\\wallhit.wav";

bool setting_main_bgm;
bool setting_play_bgm;
bool setting_click_sound;
bool setting_nontification_sound;
bool setting_ball_sound;


// extern std::unique_ptr<PoolGame::Game> game; // 声明游戏对象的指针
// 在 game_config.h 中添加线程控制标志
extern std::atomic<bool> g_GameRunning;
// 在 game_config.h 中添加全局图形锁
extern std::mutex g_GraphicsMutex;




// 游戏状态枚举
enum GameState
{
    SETTING,
    MAIN_MENU,
    GAME_PREPARING,
    GAME_RUNNING,
    MESSAGE_ALERT,
    GAME_OVER,    // 还没做
    SKIN_SHOP,
    RECHARGE_SHOP,
    PAYING_PAGE
};
GameState currentState = MAIN_MENU; // 初始化为主菜单状态

// API 接口信息
#define API_URL "https://api-billiards.lhl.one"
// LOGGER是否打开
#define IS_LOGGER_ENABLED false

// CURL 最大尝试次数和重试间隔，用来防止网络问题导致的下载失败
const int MAX_RETRY = 10;        // 最大重试次数
const int RETRY_DELAY_MS = 1000; // 重试间隔（毫秒）

#endif // GAME_CONFIG_H
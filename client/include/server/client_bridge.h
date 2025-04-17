#ifndef CLIENT_BRIDGE
#define CLIENT_BRIDGE

#include <string>
#include <server/ws_mouse.h>

// 用于处理鼠标的WS消息
std::queue<MOUSEMSG> wsMouseQueue;
std::mutex wsMouseMutex;
std::condition_variable wsMouseCV;

#include <json.h>
#include <game_config.h>
#include <ball.h>
#include <server/client_server.h>
#include <server/ws_mouse.h>

extern void stateSwitch();

/* 📌client_server中的消息处理搬到这里，当client_server收到文本帧，调用这个方法处理 */
void handleMessage(const std::string &message)
{

    // 消息处理逻辑
    string messageType = jsonParser.parseKeyValue(message, "type");

    if (messageType == "join")
    {
        string data = jsonParser.parseKeyValue(message, "data");
        string user_id = jsonParser.parseKeyValue(data, "user_id");
        std::cerr << "有新用户加入, ID:" << user_id << std::endl;
    }
    else if (messageType == "leave")
    {
        string data = jsonParser.parseKeyValue(message, "data");
        string user_id = jsonParser.parseKeyValue(data, "user_id");
        std::cerr << "有用户断开连接, ID:" << user_id << std::endl;
    }
    else if (messageType == "invite") // 数据实例：{"type": "invite", "inviter_id": "26"}
    {
        // 获取inviter_id
        string inviter_id = jsonParser.parseKeyValue(message, "inviter_id");
        string url_get_username = string(API_URL) + "/get_username?user_id=" + inviter_id;
        string response_get_username = sendGetRequest(url_get_username);
        string inviter_username = jsonParser.extractJsonValue(response_get_username, "username");

        // 不知道为什么有时候可能会有自己邀请自己的情况，所以这里要排除
        if (inviter_id != to_string(global_user_id))
        {
            // 仅在下面的四个页面下显示邀请提示
            if (currentState == MAIN_MENU || currentState == RECHARGE_SHOP || currentState == SKIN_SHOP || currentState == GAME_PREPARING)
            {
                string content = "您收到了 " + inviter_username + " 的对战邀请，是否加入？";
                drawMessageBox("新消息", content, [=]()
                               {
                    string message_type = "accept";
                    std::unordered_map<std::string, std::string> data;
                    data["user_id"] = to_string(global_user_id);
                    string acceptString = jsonParser.createJson(message_type, data); // 构建json字符串
                    client.sendMessage(acceptString); }, true, []()
                               { stateSwitch(); });
            }
        }
    }
    else if (messageType == "room_joined")
    {
        room_id = stoi(jsonParser.parseKeyValue(message, "room_id"));
        room_users = jsonParser.convertStringToVector(jsonParser.parseKeyValue(message, "users"));

        currentState = GAME_PREPARING;
        isInRoom = true;

        stateSwitch();
    }
    else if (messageType == "hanging_room")
    {
        room_id = stoi(jsonParser.parseKeyValue(message, "room_id"));
        room_users = jsonParser.convertStringToVector(jsonParser.parseKeyValue(message, "users"));
        currentState = GAME_PREPARING;
        isInRoom = true;
        stateSwitch();
    }
    else if (messageType == "room_delete")
    {
        string data = jsonParser.parseKeyValue(message, "data");
        string deleted_room_id = jsonParser.parseKeyValue(message, "room_id");
        if (stoi(deleted_room_id) == room_id)
        {
            isInRoom = false;
            currentState = MESSAGE_ALERT;
            drawMessageBox("新消息", "您的房间已解散", [=]()
                           {currentState = GAME_PREPARING; stateSwitch(); });
        }
    }
    else if (messageType == "game_start")
    {
        if (room_id == stoi(jsonParser.parseKeyValue(message, "room_id")))
        {
            string randomBallIds_str = jsonParser.parseKeyValue(message, "randomBallIds");
            randomBallIds.clear();
            randomBallIds = jsonParser.convertStringToVector(randomBallIds_str);
            current_player_id = stoi(jsonParser.parseKeyValue(message, "currentPlayerId"));
            currentState = GAME_RUNNING;
            stateSwitch();
        }
    }
    else if (messageType == "online_users")
    {
        string users = jsonParser.parseKeyValue(message, "users");
        // std::cerr << "在线用户列表: " << users << std::endl;
        // cerr << "在线用户列表：" << users << endl;
        online_users_arr = jsonParser.convertStringToVector(users);
        // 删除数组中的自己的id
        online_users_arr.erase(std::remove(online_users_arr.begin(), online_users_arr.end(), global_user_id), online_users_arr.end());
    }
    else if (messageType == "room_message")
    {
        string sender_id = jsonParser.parseKeyValue(message, "sender_id");
        // string content = jsonParser.parseKeyValue(message, "message");
        // current_player_id = stoi(jsonParser.parseKeyValue(message, "currentPlayerId"));
        // cerr << "收到房间 " << room_id << " 中用户 " << sender_id << " 发来的消息" << content;
        if (jsonParser.parseKeyValue(message, "content_type") == "msg_message")
        {
            // cerr << "收到鼠标消息" << endl;
            try
            {
                // 处理鼠标消息
                int uMsg = stoi(jsonParser.parseKeyValue(message, "uMsg"));
                int msgX = stoi(jsonParser.parseKeyValue(message, "x"));
                int msgY = stoi(jsonParser.parseKeyValue(message, "y"));

                WSMSG msg{uMsg, msgX, msgY};

                {
                    std::lock_guard<std::mutex> lock(wsMutex);
                    wsQueue.push(msg);
                }
                wsCV.notify_one();
            }
            catch (const std::exception &e)
            {
                std::cerr << "[网络异常] 无法解析 WSMSG（可能字段缺失或格式错误）: " << e.what() << std::endl;
                std::cerr << "[异常分析] 您收到的原始消息" << message << std::endl;
            }
        }
    }
}

/* 📌处理ball.h中联机的message (发送message)*/
void sendMSG(const MOUSEMSG &msg)
{
    string message_type = "room_message";
    std::unordered_map<std::string, std::string> data;
    data["content_type"] = "msg_message";
    data["sender_id"] = to_string(global_user_id); // 发送者的用户ID
    data["uMsg"] = std::to_string(msg.uMsg);
    data["x"] = std::to_string(msg.x);
    data["y"] = std::to_string(msg.y);

    string sendString = jsonParser.createJson(message_type, data); // 构建json字符串
    client.sendMessage(sendString);
    // cerr << "发送消息: " << sendString << endl;
}

/* 在MAIN_MENU检查是否在房间内但是没有进入*/
void checkIfInRoomInMAINMENU()
{
    string message_type = "check_if_in_room";
    std::unordered_map<std::string, std::string> data;

    string sendString = jsonParser.createJson(message_type, data); // 构建json字符串
    client.sendMessage(sendString);
}

#endif
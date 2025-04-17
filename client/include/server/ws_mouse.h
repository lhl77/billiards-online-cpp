#ifndef WS_MOUSE_H
#define WS_MOUSE_H

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

struct WSMSG {
    int uMsg;
    int x;
    int y;
};

inline std::queue<WSMSG> wsQueue;
inline std::mutex wsMutex;
inline std::condition_variable wsCV;

inline bool WSMouseHit() {
    std::lock_guard<std::mutex> lock(wsMutex);
    return !wsQueue.empty();
}

inline WSMSG GetWSMouseMsg() {
    std::lock_guard<std::mutex> lock(wsMutex);
    if (!wsQueue.empty()) {
        WSMSG msg = wsQueue.front();
        wsQueue.pop();
        return msg;
    }
    return WSMSG{0, 0, 0};  // 返回空结构
}

inline WSMSG WaitWSMouseMsg() {
    std::unique_lock<std::mutex> lock(wsMutex);
    wsCV.wait(lock, [] { return !wsQueue.empty(); });
    WSMSG msg = wsQueue.front();
    wsQueue.pop();
    return msg;
}

#endif
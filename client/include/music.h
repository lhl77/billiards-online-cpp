#ifndef MUSIC_H
#define MUSIC_H

#include <windows.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <random>

#include <game_config.h>
#include <manage_txt.h>

string currentMusicPathWithPlaySound;

/* 播放背景音乐(使用简单的API-playsound) */
void playBGM(const std::string &filepath, bool loop = true, bool stop = false)
{
    if (currentMusicPathWithPlaySound != filepath)
    {
        currentMusicPathWithPlaySound = filepath;
        if (stop)
        {
            PlaySound(NULL, 0, 0);
        }
        else
        {
            PlaySound(filepath.c_str(), NULL, SND_FILENAME | SND_ASYNC | (loop ? SND_LOOP : 0));
        }
    }
}

class AudioManager
{
public:
    /* 播放音频文件 */
    static bool play(const std::string &filepath, const std::string &alias, bool loop = false, int volume = 1000)
    {
        std::string actualAlias = alias;

        if (actualAlias.empty())
        {
            actualAlias = generateRandomAlias();
        }

        // 关闭已有
        mciSendStringA(("stop " + actualAlias).c_str(), NULL, 0, NULL);
        mciSendStringA(("close " + actualAlias).c_str(), NULL, 0, NULL);

        std::string openCmd = "open \"" + filepath + "\" type mpegvideo alias " + actualAlias;
        if (mciSendStringA(openCmd.c_str(), NULL, 0, NULL) != 0)
        {
            std::cerr << "[AudioManager] Failed to open: " << filepath << std::endl;
            return false;
        }

        std::string playCmd = "play " + actualAlias + (loop ? " repeat" : "");
        mciSendStringA(playCmd.c_str(), NULL, 0, NULL);

        std::string volCmd = "setaudio " + actualAlias + " volume to " + std::to_string(volume);
        mciSendStringA(volCmd.c_str(), NULL, 0, NULL);

        /* ✅ 添加到 activeAliases 中，标记为“正在播放” */
        activeAliases[actualAlias] = true;

        return true;
    }
    /* 生成随机alias */
    static std::string generateRandomAlias()
    {
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::string randomAlias;
        randomAlias.reserve(10); // 10字符长度

        // 使用当前时间作为随机种子
        static std::mt19937 gen(static_cast<unsigned>(time(nullptr)));
        std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2); // -2 是为了排除末尾的null字符

        for (int i = 0; i < 10; ++i)
        {
            randomAlias += alphanum[dis(gen)];
        }

        // 确保alias唯一
        while (activeAliases.find(randomAlias) != activeAliases.end())
        {
            randomAlias += alphanum[dis(gen)]; // 追加一个随机字符
        }

        return randomAlias;
    }

    /* 停止播放并释放 alias */
    static void stop(const std::string &alias)
    {
        // 强制 stop
        MCIERROR stopErr = mciSendStringA(("stop " + alias).c_str(), NULL, 0, NULL);
        if (stopErr != 0)
        {
            char errText[256];
            mciGetErrorStringA(stopErr, errText, 256);
            std::cerr << "[AudioManager] stop error (" << alias << "): " << errText << std::endl;
        }

        // 强制 close
        MCIERROR closeErr = mciSendStringA(("close " + alias).c_str(), NULL, 0, NULL);
        if (closeErr != 0)
        {
            char errText[256];
            mciGetErrorStringA(closeErr, errText, 256);
            std::cerr << "[AudioManager] close error (" << alias << "): " << errText << std::endl;
        }
    }

    /* 停止所有当前播放的音频 */
    static void stopAll()
    {
        for (const auto &[alias, _] : activeAliases)
        {
            stop(alias);
        }
        activeAliases.clear();
    }

    /* 设置指定 alias 的音量（0~1000） */
    static void setVolume(const std::string &alias, int volume)
    {
        if (mciSendStringA(("setaudio " + alias + " volume to " + std::to_string(volume)).c_str(), NULL, 0, NULL) != 0)
        {
            std::cerr << "[AudioManager] Failed to set volume for: " << alias << std::endl;
        }
    }

    /* 暂停播放 */
    static void pause(const std::string &alias)
    {
        if (activeAliases.find(alias) != activeAliases.end())
        {
            if (mciSendStringA(("pause " + alias).c_str(), NULL, 0, NULL) != 0)
            {
                std::cerr << "[AudioManager] Failed to pause: " << alias << std::endl;
            }
        }
        else
        {
            std::cerr << "[AudioManager] Alias not found for pause: " << alias << std::endl;
        }
    }

    /* 恢复播放 */
    static void resume(const std::string &alias)
    {
        if (activeAliases.find(alias) != activeAliases.end())
        {
            if (mciSendStringA(("resume " + alias).c_str(), NULL, 0, NULL) != 0)
            {
                std::cerr << "[AudioManager] Failed to resume: " << alias << std::endl;
            }
        }
        else
        {
            std::cerr << "[AudioManager] Alias not found for resume: " << alias << std::endl;
        }
    }

private:
    inline static std::unordered_map<std::string, bool> activeAliases;
};

// AudioManager audioManager; // 全局音频管理器实例
#endif
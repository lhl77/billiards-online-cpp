#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

#include <game_config.h> // 游戏配置

// 日志记录器类
class Logger
{
public:
    Logger(const std::string &filename) : enabled(IS_LOGGER_ENABLED)
    {
        log_file.open(filename, std::ios::app); // 以追加模式打开日志文件
        if (!log_file)
        {
            std::cerr << "无法打开日志文件: " << filename << std::endl;
        }
    }

    ~Logger()
    {
        if (log_file.is_open())
        {
            log_file.close();
        }
    }

    void log(const std::string &level, const std::string &message)
    {
        if (enabled && log_file.is_open())
        {
            log_file << "[" << current_time() << "] [" << level << "] " << message << std::endl;
        }
    }

    void info(const std::string &message)
    {
        log("INFO", message);
    }

    void warning(const std::string &message)
    {
        log("WARNING", message);
    }

    void error(const std::string &message)
    {
        log("ERROR", message);
    }

    void setEnabled(bool enable)
    {
        enabled = enable;
    }

private:
    std::ofstream log_file;
    bool enabled;

    std::string current_time()
    {
        std::time_t now = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }
};
Logger logger("log.txt"); // 创建日志记录器实例


#endif
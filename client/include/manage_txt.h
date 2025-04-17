#ifndef MANAGE_TXT_H
#define MANAGE_TXT_H


#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

class BoolConfigManager {
public:
    // 构造函数，传入配置文件路径
    explicit BoolConfigManager(const std::string& configPath) 
        : configPath_(configPath) {
        // 初始化默认值
        settings_ = {
            {"setting_main_bgm", true},
            {"setting_play_bgm", true},
            {"setting_click_sound", true},
            {"setting_nontification_sound", true},
            {"setting_ball_sound",true}
        };
        
        // 如果文件存在则加载，不存在则创建
        if (std::filesystem::exists(configPath_)) {
            loadConfig();
        } else {
            saveConfig();
        }
    }
    
    // 获取配置值
    bool getSetting(const std::string& name) const {
        auto it = settings_.find(name);
        if (it == settings_.end()) {
            throw std::invalid_argument("未知的配置项: " + name);
        }
        return it->second;
    }
    
    // 设置配置值
    void setSetting(const std::string& name, bool value) {
        auto it = settings_.find(name);
        if (it == settings_.end()) {
            throw std::invalid_argument("未知的配置项: " + name);
        }
        it->second = value;
        saveConfig();
    }
    
    // 打印当前所有配置（调试用）
    void printAllSettings() const {
        for (const auto& [name, value] : settings_) {
            std::cout << name << " : " << (value ? "true" : "false") << std::endl;
        }
    }

private:
    std::string configPath_;
    std::unordered_map<std::string, bool> settings_;
    
    // 从文件加载配置
    void loadConfig() {
        std::ifstream file(configPath_);
        if (!file.is_open()) {
            throw std::runtime_error("无法打开配置文件: " + configPath_);
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // 移除行首行尾空白字符
            line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) {
                return !std::isspace(ch);
            }));
            line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), line.end());
            
            // 跳过空行和注释行
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // 解析 "name : value" 格式
            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                continue; // 无效行
            }
            
            std::string name = line.substr(0, colonPos);
            // 移除name前后的空白
            name.erase(name.begin(), std::find_if(name.begin(), name.end(), [](int ch) {
                return !std::isspace(ch);
            }));
            name.erase(std::find_if(name.rbegin(), name.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), name.end());
            
            std::string valueStr = line.substr(colonPos + 1);
            // 移除value前后的空白
            valueStr.erase(valueStr.begin(), std::find_if(valueStr.begin(), valueStr.end(), [](int ch) {
                return !std::isspace(ch);
            }));
            valueStr.erase(std::find_if(valueStr.rbegin(), valueStr.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), valueStr.end());
            
            // 转换为小写
            std::transform(valueStr.begin(), valueStr.end(), valueStr.begin(), ::tolower);
            
            // 更新设置
            auto it = settings_.find(name);
            if (it != settings_.end()) {
                if (valueStr == "true") {
                    it->second = true;
                } else if (valueStr == "false") {
                    it->second = false;
                }
                // 其他值保持默认
            }
        }
    }
    
    // 保存配置到文件
    void saveConfig() {
        std::ofstream file(configPath_);
        if (!file.is_open()) {
            throw std::runtime_error("无法写入配置文件: " + configPath_);
        }
        
        file << "# 应用程序配置\n";
        file << "# 格式: 配置项 : 值 (true/false)\n\n";
        
        for (const auto& [name, value] : settings_) {
            file << name << " : " << (value ? "true" : "false") << "\n";
        }
    }
};

#endif
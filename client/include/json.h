#ifndef JSON_H
#define JSON_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// **简单的 JSON 解析函数**
class JsonParser
{
public:
    // 判断用户是否登录成功
    bool login(const string &response)
    {
        if (response.find("\"success\":true") != string::npos)
        {
            return true;
        }
        else if (response.find("\"error\"") != string::npos)
        {
            size_t pos = response.find("\"error\":\"");
            if (pos != string::npos)
            {
                pos += 9; // 跳过 `"error":"`
                size_t endPos = response.find("\"", pos);
                if (endPos != string::npos)
                {
                    string errorMsg = response.substr(pos, endPos - pos);
                    cerr << "登录失败: " << errorMsg << endl;
                }
            }
        }
        return false;
    }
    // JSON 解析（判断 success 字段）
    bool success(const string &response)
    {
        return response.find("\"success\":true") != string::npos;
    }
    // 解析 JSON 错误信息
    string error(const string &response)
    {
        size_t pos = response.find("\"error\":\"");
        if (pos != string::npos)
        {
            pos += 9; // 跳过 `"error":"`
            size_t endPos = response.find("\"", pos);
            if (endPos != string::npos)
            {
                return response.substr(pos, endPos - pos);
            }
        }
        return "未知错误";
    }
    // 提取 JSON 键值，并进行 Unicode 转换 + `\/` 修正
    string extractJsonValue(const string &response, const string &key)
    {
        size_t pos = response.find("\"" + key + "\":\"");
        if (pos != string::npos)
        {
            pos += key.length() + 4; // 跳过 `"key":"`
            size_t endPos = response.find("\"", pos);
            if (endPos != string::npos)
            {
                string value = response.substr(pos, endPos - pos);

                // 处理 `\/` 替换为 `/`
                size_t slashPos;
                while ((slashPos = value.find("\\/")) != string::npos)
                {
                    value.replace(slashPos, 2, "/");
                }

                // 解析 Unicode `\uXXXX`
                return decodeUnicode(value);
            }
        }
        return "";
    }
    // 解析 JSON 数组
    vector<string> extractJsonValues(const string &response, const string &key)
    {
        vector<string> values;
        size_t pos = 0;

        while ((pos = response.find("\"" + key + "\":\"", pos)) != string::npos)
        {
            pos += key.length() + 4;
            size_t endPos = response.find("\"", pos);
            if (endPos != string::npos)
            {
                values.push_back(response.substr(pos, endPos - pos));
            }
            pos = endPos;
        }
        return values;
    }

    // 对于client_server.h中的json解析函数，添加一个新的解析函数
    // 去除字符串前后的空白字符
    string trim(const string &str)
    {
        size_t start = 0;
        size_t end = str.size() - 1;

        while (start <= end && isspace(str[start]))
        {
            start++;
        }
        while (end >= start && isspace(str[end]))
        {
            end--;
        }

        return str.substr(start, end - start + 1);
    }

    // 解析 JSON 字符串中的对象或数组
    string parseJsonValue(const string &json_str)
    {
        // 如果值是对象或数组类型（以 { 或 [ 开始）
        if (json_str[0] == '{' || json_str[0] == '[')
        {
            size_t start_pos = 0;
            size_t end_pos = json_str.size();
            int bracket_count = 0; // 用于匹配括号对

            for (size_t i = 0; i < json_str.size(); ++i)
            {
                if (json_str[i] == '{' || json_str[i] == '[')
                {
                    if (bracket_count == 0)
                        start_pos = i;
                    bracket_count++;
                }
                else if (json_str[i] == '}' || json_str[i] == ']')
                {
                    bracket_count--;
                    if (bracket_count == 0)
                    {
                        end_pos = i + 1;
                        break;
                    }
                }
            }

            // 返回从 { 或 [ 开始到 } 或 ] 结束的部分
            return json_str.substr(start_pos, end_pos - start_pos);
        }

        // 如果值是字符串或数字类型，直接返回该部分
        return json_str;
    }

    // 解析 JSON 字符串并提取键值对
    string parseKeyValue(const string &json_str, const string &key)
    {
        // size_t start_pos = json_str.find(key);
        // if (start_pos == string::npos)
        // {
        //     return ""; // 如果找不到该键，返回空字符串
        // }

        // // 定位到键的后面，跳过 `"key":` 部分
        // start_pos = json_str.find(":", start_pos);
        // if (start_pos == string::npos)
        // {
        //     return ""; // 如果没有找到冒号，说明格式有误
        // }
        // start_pos++;

        // // 跳过空格
        // while (isspace(json_str[start_pos]))
        // {
        //     start_pos++;
        // }

        // 构造带引号的键名匹配模式："key":
        string key_pattern = "\"" + key + "\":";
        size_t start_pos = json_str.find(key_pattern);

        if (start_pos == string::npos)
        {
            return ""; // 未找到带引号的键名
        }

        // 直接跳到冒号后的位置
        start_pos += key_pattern.length();

        // 跳过冒号后的空格
        while (start_pos < json_str.size() && isspace(json_str[start_pos]))
        {
            start_pos++;
        }
        // 获取值（递归处理对象或数组）
        size_t end_pos = start_pos;
        if (json_str[start_pos] == '"')
        {
            // 如果值是字符串，查找并返回字符串内容
            start_pos++; // 跳过开头的双引号
            end_pos = json_str.find('"', start_pos);
            if (end_pos == string::npos)
            {
                return ""; // 如果找不到结束的双引号，说明格式有误
            }
            return json_str.substr(start_pos, end_pos - start_pos);
        }
        else if (isdigit(json_str[start_pos]) || json_str[start_pos] == '-')
        {
            // 如果值是数字，读取数字直到遇到非数字字符
            while (end_pos < json_str.size() && (isdigit(json_str[end_pos]) || json_str[end_pos] == '.'))
            {
                end_pos++;
            }
            return json_str.substr(start_pos, end_pos - start_pos);
        }

        // 如果值是对象或数组类型，递归调用解析
        return parseJsonValue(json_str.substr(start_pos));
    }

    // 将'["1", "3", "10"]'这样的字符串转化为vector数组
    std::vector<int> convertStringToVector(const std::string &str)
    {
        std::vector<int> online_users_arr;
        std::string cleanedStr = str;

        // Remove the brackets and quotes
        cleanedStr.erase(remove(cleanedStr.begin(), cleanedStr.end(), '['), cleanedStr.end());
        cleanedStr.erase(remove(cleanedStr.begin(), cleanedStr.end(), ']'), cleanedStr.end());
        cleanedStr.erase(remove(cleanedStr.begin(), cleanedStr.end(), '"'), cleanedStr.end());

        // Use stringstream to split the string by commas
        std::stringstream ss(cleanedStr);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            // Conversion and error checking
            try
            {
                online_users_arr.push_back(std::stoi(item)); // Convert to int and add to vector
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Invalid number: " << item << std::endl;
            }
            catch (const std::out_of_range &e)
            {
                std::cerr << "Number out of range: " << item << std::endl;
            }
        }

        return online_users_arr;
    }

    // 创建 JSON 字符串，使用方法：
    // 包含其他JSON属性的无序映射
    // std::unordered_map<std::string, std::string> data;
    // data["user_id"] = "user2";         // 被邀请者的用户ID
    // data["inviter_id"] = "user1";      // 邀请者的用户ID
    // std::string jsonMessage = createJson(message_type, data);   // 构建json字符串
    string createJson(const std::string &message_type, const std::unordered_map<std::string, std::string> &data)
    {
        std::ostringstream ss;
        ss << "{"
           << "\"type\": \"" << message_type << "\"";

        // 添加属性
        for (auto it = data.begin(); it != data.end(); ++it)
        {
            ss << ", \"" << it->first << "\": \"" << it->second << "\"";
        }

        ss << "}";
        return ss.str();
    }

private:
    // 将 16 进制字符串转换为整数
    int hexToInt(const string &hex)
    {
        int value;
        stringstream ss;
        ss << hex;
        ss >> std::hex >> value;
        return value;
    }

    // 将 Unicode 码点转换为 UTF-8
    string unicodeToUTF8(int unicode)
    {
        string utf8;
        if (unicode <= 0x7F)
        {
            utf8 += static_cast<char>(unicode);
        }
        else if (unicode <= 0x7FF)
        {
            utf8 += static_cast<char>(0xC0 | (unicode >> 6));
            utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
        }
        else if (unicode <= 0xFFFF)
        {
            utf8 += static_cast<char>(0xE0 | (unicode >> 12));
            utf8 += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
        }
        else if (unicode <= 0x10FFFF)
        {
            utf8 += static_cast<char>(0xF0 | (unicode >> 18));
            utf8 += static_cast<char>(0x80 | ((unicode >> 12) & 0x3F));
            utf8 += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
        }
        return utf8;
    }

    // 解析 Unicode `\uXXXX`
    string decodeUnicode(const string &input)
    {
        string output;
        size_t i = 0;
        while (i < input.length())
        {
            if (input[i] == '\\' && i + 5 < input.length() && input[i + 1] == 'u')
            {
                string hexCode = input.substr(i + 2, 4);
                int unicode = hexToInt(hexCode);
                output += unicodeToUTF8(unicode);
                i += 6; // 跳过 `\uXXXX`
            }
            else
            {
                output += input[i];
                i++;
            }
        }
        return output;
    }
};
JsonParser jsonParser;

#endif // JSON_H
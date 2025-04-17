#ifndef HTTPLIB_H
#define HTTPLIB_H

#include <cstddef> // for size_t
#include <string>  // for string
#include <curl/curl.h>
#include <iostream>
#include <thread>

//关联的自定义头文件
#include <logger.h>
#include <game_config.h>

using namespace std;



// **GET和POST请求的回调函数**
// HTTP 响应回调函数
size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output)
{
    output->append((char *)contents, size * nmemb);
    return size * nmemb;
}
// 发送 GET 请求（失败自动重试）
string sendGetRequest(const string &url)
{
    int attempt = 0;
    string response;

    while (attempt < MAX_RETRY)
    {
        attempt++;
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            cerr << "初始化失败" << endl;
            logger.error("CURL 初始化失败");
            return "";
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 秒超时

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            return response; // 成功返回数据
        }

        // cerr << "CURL GET 请求失败 (" << attempt << "/" << MAX_RETRY << "): "
        //      << curl_easy_strerror(res) << endl;
        logger.error("CURL GET 请求失败: " + string(curl_easy_strerror(res)));

        if (attempt < MAX_RETRY)
        {
            // cerr << "等待 " << (RETRY_DELAY_MS / 1000) << " 秒后重试..." << endl;
            this_thread::sleep_for(chrono::milliseconds(RETRY_DELAY_MS));
        }
    }

    cerr << "CURL GET 请求失败，达到最大重试次数" << endl;
    return "";
}
// 发送 POST 请求（失败自动重试）
string sendPostRequest(const string &url, const string &postData)
{
    int attempt = 0;
    string response;

    while (attempt < MAX_RETRY)
    {
        attempt++;
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            cerr << "初始化失败" << endl;
            logger.error("CURL 初始化失败");
            return "";
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 秒超时

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            return response; // 成功返回数据
        }

        // cerr << "CURL POST 请求失败 (" << attempt << "/" << MAX_RETRY << "): "
        //      << curl_easy_strerror(res) << endl;
        logger.error("CURL POST 请求失败: " + string(curl_easy_strerror(res)));

        if (attempt < MAX_RETRY)
        {
            // cerr << "等待 " << (RETRY_DELAY_MS / 1000) << " 秒后重试..." << endl;
            this_thread::sleep_for(chrono::milliseconds(RETRY_DELAY_MS));
        }
    }

    cerr << "CURL POST 请求失败，达到最大重试次数" << endl;
    return "";
}




#endif // HTTPLIB_H
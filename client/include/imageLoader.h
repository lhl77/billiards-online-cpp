#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <graphics.h> // EasyX 图形库
#include <conio.h>    // _getch() 函数
#include <string>
#include <mutex>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <thread>
#include <logger.h>
#include <curl/curl.h>
#include <game_config.h>

using namespace std;




// 图片加载类
class ImageLoader
{
public:
    // ✅ 让 `make_shared<ImageLoader>` 能够访问构造函数
    ImageLoader(const string &url, int posX, int posY, bool clickable, function<void()> cb)
        : x(posX), y(posY), onClick(cb), active(true)
    {
        string path = downloadImage(url);
        if (!path.empty())
        {
            loadimage(&img, path.c_str());
        }
    }


    // ✅ 直接创建实例并返回 `shared_ptr`
    static shared_ptr<ImageLoader> drawImageFromURL(const string &url, int x, int y,
                                                    bool clickable = false,
                                                    function<void()> onClick = nullptr)
    {
        auto instance = make_shared<ImageLoader>(url, x, y, clickable, onClick);
        if (clickable)
        {
            lock_guard<mutex> lock(instanceMutex);
            instances.push_back(instance);
        }
        instance->draw();
        return instance;
    }

    // 清除所有实例
    static void clearAll()
    {
        lock_guard<mutex> lock(instanceMutex);
        instances.clear();
    }

    // 启动鼠标监听
    static void startEventLoop()
    {
        if (!eventThread.joinable())
        {
            exitFlag.store(false);
            eventThread = std::thread([]
                                      {
                    while (!exitFlag.load()) {
                        if(currentState != GAME_RUNNING){
                            processMouseEvents();
                        }
                        Sleep(10);
                    } });
            eventThread.detach();
        }
    }

    // 绘制图片
    void draw()
    {
        lock_guard<mutex> lock(imgMutex);
        if (img.getwidth() > 0 && img.getheight() > 0)
        {
            putimage(x, y, &img);
        }
        else
        {
            setfillcolor(LIGHTGRAY);
            solidrectangle(x, y, x + 100, y + 100);
            outtextxy(x + 10, y + 10, "加载失败...");
        }
    }

    // 触发点击事件
    void triggerClick()
    {
        if (!active)
            return;
        if (onClick)
        {
            // cerr << "点击图片，触发 triggerClick()" << endl;
            onClick();
        }
        else
        {
            logger.error("onClick为空");
        }
    }

    // 移除实例
    void deactivate()
    {
        active = false;
        lock_guard<mutex> lock(instanceMutex);
        instances.erase(remove_if(instances.begin(), instances.end(),
                                  [this](const shared_ptr<ImageLoader> &inst)
                                  {
                                      return inst.get() == this;
                                  }),
                        instances.end());
    }

    // **下载图片**
    // 图片下载函数
    static string preDownloadImage(const string &url)
    {
        string path = "images/cache/" + to_string(hash<string>{}(url)) + ".png";

        // 检查文件是否已经存在
        ifstream file(path);
        if (file.good())
        {
            return path; // 文件已存在，直接返回
        }

        int attempt = 0;
        while (attempt < MAX_RETRY)
        {
            attempt++;
            CURL *curl = curl_easy_init();
            if (!curl)
            {
                cerr << "CURL 初始化失败" << endl;
                return "";
            }

            FILE *fp = fopen(path.c_str(), "wb");
            if (!fp)
            {
                cerr << "无法创建文件: " << path << endl;
                curl_easy_cleanup(curl);
                return "";
            }

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            fclose(fp);

            if (res == CURLE_OK)
            {
                // 下载成功，检查文件是否有效
                ifstream checkFile(path, ios::binary | ios::ate);
                if (checkFile.good() && checkFile.tellg() > 100)
                { // 检查文件大小 >100B
                    checkFile.close();
                    return path;
                }
                else
                {
                    checkFile.close();
                    cerr << "下载的图片文件损坏，删除文件并重试..." << endl;
                    remove(path.c_str());
                }
            }
            else
            {
                cerr << "CURL 下载失败 (" << attempt << "/" << MAX_RETRY << "): " << curl_easy_strerror(res) << endl;
                remove(path.c_str()); // 删除损坏文件
            }

            if (attempt < MAX_RETRY)
            {
                cerr << "等待 " << (RETRY_DELAY_MS / 1000) << " 秒后重试..." << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
            }
        }

        cerr << "图片下载失败，达到最大重试次数" << endl;
        return "";
    }

private:
    IMAGE img;
    int x, y;
    function<void()> onClick;
    bool active;
    mutable mutex imgMutex;

    // 静态成员
    static vector<shared_ptr<ImageLoader>> instances;
    static mutex instanceMutex;
    static atomic<bool> exitFlag;
    static std::thread eventThread;

    // 处理鼠标事件
    static void processMouseEvents()
    {
        if (!exitFlag.load() && GetHWnd()) {  // ✅ 检查窗口是否有效
            if (!MouseHit())
                return;
        }
            MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg != WM_LBUTTONDOWN)
            return;
        int clickX = msg.x, clickY = msg.y;

        for (auto &inst : instances)
        {
            if (inst->checkHit(clickX, clickY))
            {
                inst->triggerClick();
                break;
            }
        }
    }

    // 点击检测
    bool checkHit(int mx, int my) const
    {
        lock_guard<mutex> lock(imgMutex);
        int width = img.getwidth();
        int height = img.getheight();
        return mx >= x && mx <= x + width && my >= y && my <= y + height;
    }

    // **下载图片**
    static string downloadImage(const string &url)
    {
        string path = "images/cache/" + to_string(hash<string>{}(url)) + ".png";
        ifstream file(path);
        if (file.good())
            return path;

        CURL *curl = curl_easy_init();
        if (!curl)
            return "";

        FILE *fp = fopen(path.c_str(), "wb");
        if (!fp)
        {
            curl_easy_cleanup(curl);
            return "";
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        CURLcode res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        fclose(fp);
        return (res == CURLE_OK) ? path : "";
    }

    static size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        return fwrite(ptr, size, nmemb, (FILE *)stream);
    }
};



#endif
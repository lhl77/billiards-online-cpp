#ifndef GRAPHIC_FUNCTIONS_H
#define GRAPHIC_FUNCTIONS_H

#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <ctime>
#include <gdiplus.h>
#include <windows.h>
#include <algorithm>
#include <sys/stat.h>
#include <io.h>
#include <sstream>
#include <graphics.h> // EasyX 图形库
#include <conio.h>    // _getch() 函数
#include <imageLoader.h>



using namespace std;

#include <logger.h> // 日志记录器类
#include <music.h>
#include <game_config.h> // 游戏配置

// 函数：终端打印logo
void print_logo()
{
    std::cout << "███████╗ ██████╗██╗  ██╗ ██████╗     ██████╗  ██████╗  ██████╗ ██╗     \n";
    std::cout << "██╔════╝██╔════╝██║  ██║██╔═══██╗    ██╔══██╗██╔═══██╗██╔═══██╗██║     \n";
    std::cout << "█████╗  ██║     ███████║██║   ██║    ██████╔╝██║   ██║██║   ██║██║     \n";
    std::cout << "██╔══╝  ██║     ██╔══██║██║   ██║    ██╔═══╝ ██║   ██║██║   ██║██║     \n";
    std::cout << "███████╗╚██████╗██║  ██║╚██████╔╝    ██║     ╚██████╔╝╚██████╔╝███████╗\n";
    std::cout << "╚══════╝ ╚═════╝╚═╝  ╚═╝ ╚═════╝     ╚═╝      ╚═════╝  ╚═════╝ ╚══════╝\n";
}

// 函数：载入自定义字体并设置
void set_custom_font(int size, const wchar_t *font_path, const wchar_t *font_name)
{
    // 加载自定义字体
    int fontID = AddFontResourceExW(font_path, FR_PRIVATE, NULL);
    if (fontID == 0)
    {

        logger.error("字体加载失败：" + std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(font_path));
        return;
    }
    else
    {

        // logger.info("字体加载成功：" + std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(font_path));
    }

    // 通知系统字体更新（可选）
    SendMessage(GetHWnd(), WM_FONTCHANGE, 0, 0);

    // 设置文本样式（高度、宽度、字体名称）
    // Convert wide string to narrow string
    std::wstring ws(font_name);
    std::string font_name_str(ws.begin(), ws.end());
    settextstyle(size, 0, font_name_str.c_str());
}

/* 自动换行绘制文本，支持 \n 换行 */
void drawTextWithWrap(int x, int y, const string &text, int maxWidth)
{
    string line;
    vector<string> lines;

    for (char ch : text)
    {
        if (ch == '\n')
        {
            // 直接换行
            lines.push_back(line);
            line.clear();
            continue;
        }

        string temp = line + ch;
        int textW = textwidth(temp.c_str()); // 计算当前文本宽度

        if (textW > maxWidth)
        {
            // 超出最大宽度，换行
            lines.push_back(line);
            line = ch;
        }
        else
        {
            line += ch;
        }
    }

    if (!line.empty())
    {
        lines.push_back(line); // 最后一行加入
    }

    for (size_t i = 0; i < lines.size(); i++)
    {
        outtextxy(x, y + i * 20, lines[i].c_str()); // 每行间隔 20px
    }
}

// 函数：构建居中窗口
void create_centered_window(int width, int height, const wchar_t *window_title, const wchar_t *icon_path)
{
    // 获取屏幕的宽度和高度
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    // 计算窗口的左上角坐标，使其居中
    int left = (screen_width - width) / 2;
    int top = (screen_height - height) / 2;

    // setaspectratio(1.5, 1.5);

    // 初始化图形窗口并设置大小
    initgraph(width, height);
    // 创建日志记录器实例

    // logger.info("成功创建居中窗口");

    // 获取当前窗口句柄
    HWND hwnd = GetHWnd();

    // 设置窗口标题（使用宽字符版本）
    SetWindowTextW(hwnd, window_title); // 使用宽字符版本的 API
    // 创建日志记录器实例
    // logger.info("成功设置窗口标题和图标");

    // 将宽字符路径转换为多字节字符路径
    int buffer_size = WideCharToMultiByte(CP_ACP, 0, icon_path, -1, NULL, 0, NULL, NULL);
    char *icon_path_multibyte = new char[buffer_size];
    WideCharToMultiByte(CP_ACP, 0, icon_path, -1, icon_path_multibyte, buffer_size, NULL, NULL);

    // 加载图标
    HICON hIcon = (HICON)LoadImage(NULL, icon_path_multibyte, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    delete[] icon_path_multibyte; // 释放动态分配的内存
    if (!hIcon)
    {
        logger.error("图标加载失败：" + std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(icon_path));
    }
    else
    {
        // logger.info("图标加载成功：" + std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(icon_path));
    }

    if (hIcon)
    {
        // 设置窗口图标
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon); // 设置小图标（可选）
        // logger.info("成功设置窗口图标");
    }

    // 将窗口居中
    MoveWindow(hwnd, left, top, width, height, TRUE);
    // logger.info("成功将窗口居中");
}

/**
 * @brief 修改现有窗口的大小、标题、图标，并自动居中
 * @param width 新宽度（像素）
 * @param height 新高度（像素）
 * @param window_title 新标题（可选，nullptr 表示不修改）
 * @param icon_path 新图标路径（可选，nullptr 表示不修改）
 * @return 是否成功修改（失败返回 false）
 */
bool modify_window(int width, int height,
                   const wchar_t *window_title = nullptr,
                   const wchar_t *icon_path = nullptr)
{
    // 获取当前窗口句柄（必须先调用 initgraph）
    HWND hwnd = GetHWnd();
    if (!hwnd)
        return false; // 没有活动窗口

    // 修改标题（如果提供）
    if (window_title)
    {
        SetWindowTextW(hwnd, window_title);
    }

    // 修改图标（如果提供）
    if (icon_path)
    {
        HICON hIcon = (HICON)LoadImageW(
            nullptr, icon_path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
        if (hIcon)
        {
            SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        }
    }

    // 计算居中位置
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    int left = (screen_width - width) / 2;
    int top = (screen_height - height) / 2;

    // 调整窗口大小和位置（考虑边框）
    RECT rect = {0, 0, width, height};
    AdjustWindowRect(&rect, GetWindowLong(hwnd, GWL_STYLE), FALSE);
    MoveWindow(
        hwnd,
        left,
        top,
        rect.right - rect.left, // 实际窗口宽度
        rect.bottom - rect.top, // 实际窗口高度
        TRUE);

    return true;
}

// 函数：绘制居中背景图片
void draw_centered_background(const char *image_path)
{
    // 获取窗口的宽度和高度
    int window_width = getwidth();   // 获取窗口的宽度
    int window_height = getheight(); // 获取窗口的高度

    // logger.info("成功获取窗口的宽度和高度");
    // 加载背景图片
    IMAGE bg_image;
    loadimage(&bg_image, image_path);
    if (bg_image.getwidth() == 0 || bg_image.getheight() == 0)
    {
        logger.error("加载背景图片失败:" + std::string(image_path));
        return;
    }
    else
    {
        // logger.info("成功加载背景图片:" + std::string(image_path));
    }
    // 获取图片原始的宽度和高度
    int img_width = bg_image.getwidth();
    int img_height = bg_image.getheight();
    // logger.info("成功获取图片原始的宽度和高度:" + std::to_string(img_width) + std::to_string(img_height));
    // 计算缩放比例，使图片的高度等于窗口的高度
    float scale = (float)window_height / img_height;

    // 计算缩放后的宽度
    int new_width = img_width * scale;
    int new_height = window_height; // 高度填满窗口

    // 计算图片的左上角坐标，使其水平居中
    int x_offset = (window_width - new_width) / 2; // 使图片水平居中
    int y_offset = 0;                              // 由于高度已经填满，所以 y_offset 为 0

    // 绘制图像（指定目标位置、目标宽高、源图像、源图像裁剪区域）
    putimage(x_offset, y_offset, new_width, new_height, &bg_image, 0, 0, SRCCOPY);
    // logger.info("成功绘制背景图片:" + std::string(image_path));
}

// 等比例缩放并绘制图片
// L"example.png" 是图片的路径，100, 100 是绘制图片的位置，300, 300 是最大宽高
//    resizeImage(L"example.png", 100, 100, 300, 300);
void resizeImage(const char *filePath, int x, int y, int maxWidth, int maxHeight)
{
    // 加载图片
    IMAGE img;
    loadimage(&img, filePath); // 参数应该是 (IMAGE *, const wchar_t *)，确保路径是宽字符格式

    if (img.getwidth() == 0 || img.getheight() == 0)
    {
        logger.error("加载图片失败" + std::string(filePath));
        return;
    }
    else
    {
        // logger.info("成功加载图片" + std::string(filePath));
    }
    // 获取原始图片宽高
    int imgWidth = img.getwidth();
    int imgHeight = img.getheight();
    // logger.info("成功获取图片的宽度和高度:" + std::to_string(imgWidth) + std::to_string(imgHeight));
    // 计算等比例缩放系数
    double scaleW = (double)maxWidth / imgWidth;
    double scaleH = (double)maxHeight / imgHeight;
    double scale = min(scaleW, scaleH); // 取较小的缩放比例，保持图片比例

    // 计算缩放后的宽高
    int newWidth = imgWidth * scale;
    int newHeight = imgHeight * scale;

    // 将图片绘制到指定位置，保持等比例缩放
    putimage(x, y, newWidth, newHeight, &img, 0, 0, SRCCOPY);
    // logger.info("成功绘制图片:" + std::string(filePath));
}

// 绘制消息窗口
void drawMessageBox(const std::string &boxTitle, const std::string &content,
                    std::function<void()> yesCallback = nullptr, bool showNoButton = false, std::function<void()> noCallback = nullptr)
{
    currentState = MESSAGE_ALERT;
    // PlaySound(TEXT("packs\\nontification.wav"), NULL, SND_FILENAME | SND_ASYNC);
    if(setting_nontification_sound){
        AudioManager::play("packs/nontification.wav", "nontification", false, 1000); // 播放提示音
    }
    // 清理画布
    ImageLoader::clearAll();

    // 绘制消息窗口
    ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/24/y2hJAVpcuMXvzHs.png", 231, 90);

    // 自定义文本
    set_custom_font(22, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
    settextcolor(BLACK);
    int boxTitle_x = 231 + (337 - textwidth(boxTitle.c_str())) / 2;
    outtextxy(boxTitle_x, 115, boxTitle.c_str());

    // 绘制 Yes 按钮
    ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/24/plKNYsk1o8gLAc9.png", 440, 240, true, yesCallback); // 点击事件

    // 绘制 No 按钮
    if (showNoButton)
    {
        ImageLoader::drawImageFromURL("https://cdn.sa.net/2025/03/24/GyfBxODdhR8IY1C.png", 320, 240, true, noCallback); // 点击事件
    }

    // 绘制内容
    set_custom_font(18, L"./packs/fusion-pixel/fusion-pixel-12px-monospaced-zh_hans.ttf", L"Fusion Pixel 12px monospaced zh_hans");
    drawTextWithWrap(251, 152, content, 300);
}

bool g_isWindowClosed = false;
// 自定义窗口消息处理函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CLOSE) // 捕获窗口关闭事件
    {
        g_isWindowClosed = true;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void transparentimage3(IMAGE* dstimg, int x, int y, IMAGE* srcimg) //新版png
{
    HDC dstDC = GetImageHDC(dstimg);
    HDC srcDC = GetImageHDC(srcimg);
    int w = srcimg->getwidth();
    int h = srcimg->getheight();
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}

#endif // WINDOW_FUNCTIONS_H
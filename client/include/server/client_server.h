#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

#include <iostream>
#include <winsock2.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <vector>
#include <random>
#include <codecvt>
#include <locale>
#include <fstream>
#include <logger.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <cstdint>
#include <graphics.h> // EasyX 图形库
#include <conio.h>    // _getch() 函数

// 自定义头文件
#include <json.h>
#include <imageLoader.h>
#include <game_config.h>
#include <ball.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#define SERVER_HOST "api-billiards.lhl.one"
#define SERVER_PORT 443

#define HEARTBEAT_INTERVAL 10000  // 10秒心跳间隔
#define HEARTBEAT_TIMEOUT 90000   // 90秒超时阈值
#define MAX_RECONNECT_ATTEMPTS 5  // 最大重连次数
#define RECONNECT_BASE_DELAY 5000 // 重连基准延迟(毫秒)

using namespace std;

extern void handleMessage(const std::string &message);

// // 创建游戏实例
// PoolGame::Game game;

class WebSocketClient
{
private:
    // 网络连接核心组件
    SOCKET sock = INVALID_SOCKET;
    SSL *ssl = nullptr;
    SSL_CTX *ctx = nullptr;
    atomic<bool> connectionActive{false};

    // 消息处理系统
    queue<string> msgQueue;
    mutex queueMutex;
    condition_variable queueCV;

    // 网络监控模块
    thread heartbeatThread;
    thread messageProcessorThread;
    chrono::steady_clock::time_point lastPongTime;

    // 重连策略参数
    atomic<int> reconnectAttempts{0};
    mutex sslMutex;

    // WebSocket协议解析
    struct WebSocketFrame
    {
        uint8_t opcode;
        vector<uint8_t> payload;
    };

public:
    string lastUserID;

    WebSocketClient()
    {
        lastPongTime = chrono::steady_clock::now();
    }

    bool connectServer(const string &user_id)
    {
        // 设置接收超时
        DWORD timeout = 5000; // 5 秒超时
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

        lastUserID = user_id;
        if (connectionActive.load())
        {
            logger.warning("已有活跃连接，拒绝重复创建");
            return false;
        }

        // 初始化网络栈
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            logger.error("Winsock初始化失败");
            return false;
        }

        // 创建TCP套接字
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET)
        {
            logger.error("创建socket失败");
            WSACleanup();
            return false;
        }

        // DNS解析
        hostent *host = gethostbyname(SERVER_HOST);
        if (!host)
        {
            logger.error("解析服务器IP失败");
            closesocket(sock);
            WSACleanup();
            return false;
        }

        // 建立TCP连接
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(SERVER_PORT);
        serverAddr.sin_addr.s_addr = *((unsigned long *)host->h_addr);

        if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            logger.error("连接服务器失败");
            closesocket(sock);
            WSACleanup();
            return false;
        }
        logger.info("TCP连接建立成功");

        // SSL/TLS握手
        init_openssl();
        ctx = create_context();
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);

        if (SSL_connect(ssl) <= 0)
        {
            unsigned long sslError = ERR_get_error();
            logger.error("SSL握手失败: " + string(ERR_error_string(sslError, nullptr)));
            while (sslError != 0)
            {
                sslError = ERR_get_error();
                logger.error("详细SSL错误: " + string(ERR_error_string(sslError, nullptr)));
            }
            cleanup();
            return false;
        }
        logger.info("SSL连接建立成功");

        // WebSocket握手
        if (!performHandshake(user_id))
        {
            cleanup();
            return false;
        }

        connectionActive.store(true);
        startHeartbeat();
        startMessageProcessor();
        return true;
    }

    /* 📌消息处理在这里，把这个搬到client_bridge里面，这样好修改好开发好维护 */
    void outputMessage(const std::string &message)
    {

        handleMessage(message);
    }

    void handleTextMessage(const vector<uint8_t> &frame)
    {
        string message = parseWebSocketFrame(frame);
        if (!message.empty() && is_valid_utf8(message))
        {
            {
                lock_guard<mutex> lock(queueMutex);
                outputMessage(message);
            }
            logger.info("[WS 消息]: " + message);
            cerr << "[WS 消息]: " << message << endl;
        }
    }

    void processWebSocketFrame(const vector<uint8_t> &frame)
    {
        uint8_t opcode = frame[0] & 0x0F;
        logger.info("收到帧，opcode: " + to_string(opcode));

        if (opcode == 0x09) // Ping 控制帧
        {
            logger.info("收到 Ping 帧，稍后发送 Pong 帧");

            std::vector<uint8_t> pingPayload;
            if (frame.size() > 2)
            {
                pingPayload.assign(frame.begin() + 2, frame.end());
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 等 50ms 再发 Pong
            sendPongFrame(pingPayload);
            return;
        }
        else if (opcode == 0x0A) // Pong 控制帧
        {
            logger.info("收到 Pong 帧");
            // 处理 Pong 帧
            handlePongFrame(frame);
            return;
        }
        else if (opcode == 0x08) // 关闭帧
        {
            logger.info("收到关闭帧");
            handleCloseFrame(frame);
            return;
        }

        // 其他帧处理
        WebSocketFrame parsed = parseFrameStructure(frame);
        switch (parsed.opcode)
        {
        case 0x01: // 文本帧
            logger.info("收到文本帧");
            handleTextMessage(parsed.payload);
            break;
            // 可添加其他类型帧处理
        default:
            logger.warning("收到未知类型帧，opcode: " + to_string(parsed.opcode));
            break;
        }
    }

    void closeConnection()
    {
        connectionActive.store(false);
        vector<uint8_t> closeFrame = {0x88, 0x00};
        {
            lock_guard<mutex> lock(sslMutex);
            if (SSL_write(ssl, closeFrame.data(), closeFrame.size()) <= 0)
            {
                logger.error("关闭帧发送失败");
            }
            else
            {
                logger.info("关闭帧发送成功");
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));

        if (heartbeatThread.joinable())
            heartbeatThread.join();
        if (messageProcessorThread.joinable())
            messageProcessorThread.join();

        lock_guard<mutex> lock(sslMutex);
        if (ssl)
        {
            SSL_shutdown(ssl);
            SSL_free(ssl);
            ssl = nullptr;
        }
        if (ctx)
        {
            SSL_CTX_free(ctx);
            ctx = nullptr;
        }
        if (sock != INVALID_SOCKET)
        {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }
        WSACleanup();
        logger.info("连接关闭，资源清理完成");
    }

    void receiveMessages()
    {

        vector<uint8_t> frameBuffer(16384);
        fd_set read_fds;
        struct timeval timeout;

        while (connectionActive.load())
        {
            FD_ZERO(&read_fds);
            FD_SET(sock, &read_fds);

            timeout.tv_sec = 10; // 超时时间 10 秒
            timeout.tv_usec = 0;

            int selectResult = select(sock + 1, &read_fds, NULL, NULL, &timeout);
            if (selectResult == 0)
            {
                logger.warning("SSL_read 超时，无数据可读");
                continue; // 继续监听
            }
            else if (selectResult < 0)
            {
                logger.error("select() 失败");
                break;
            }

            int bytesReceived = 0;
            {
                lock_guard<mutex> lock(sslMutex);
                bytesReceived = SSL_read(ssl, frameBuffer.data(), frameBuffer.size());
            }

            if (bytesReceived <= 0)
            {
                int sslError = SSL_get_error(ssl, bytesReceived);
                if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE)
                {
                    logger.warning("SSL_read 需要等待数据");
                    this_thread::sleep_for(chrono::milliseconds(500)); // 等待 500ms 再试
                    continue;
                }
                logger.error("连接异常，SSL错误码: " + to_string(sslError));
                break;
            }

            logger.info("接收到数据，字节数: " + to_string(bytesReceived));
            vector<uint8_t> receivedData(frameBuffer.begin(), frameBuffer.begin() + bytesReceived);
            processWebSocketFrame(receivedData);
        }

        if (connectionActive.load())
        {
            attemptReconnect();
        }
    }

    vector<uint8_t> buildWebSocketFrame(const string &data, bool isBinary = false)
    {
        vector<uint8_t> frame;
        frame.push_back(0x80 | (isBinary ? 0x02 : 0x01)); // FIN+Opcode

        if (data.size() <= 125)
        {
            frame.push_back(0x80 | static_cast<uint8_t>(data.size()));
        }
        else if (data.size() <= 65535)
        {
            frame.push_back(0x80 | 126);
            frame.push_back(static_cast<uint8_t>((data.size() >> 8) & 0xFF));
            frame.push_back(static_cast<uint8_t>(data.size() & 0xFF));
        }
        else
        {
            frame.push_back(0x80 | 127);
            for (int i = 7; i >= 0; --i)
            {
                frame.push_back(static_cast<uint8_t>((data.size() >> (i * 8)) & 0xFF));
            }
        }

        auto maskKey = generateMaskKey();
        frame.insert(frame.end(), maskKey.begin(), maskKey.end());

        for (size_t i = 0; i < data.size(); ++i)
        {
            frame.push_back(data[i] ^ maskKey[i % 4]);
        }
        return frame;
    }

    // void sendMessage(const string &message)
    // {
    //     auto frame = buildWebSocketFrame(message);
    //     lock_guard<mutex> lock(sslMutex);
    //     if (SSL_write(ssl, frame.data(), frame.size()) <= 0)
    //     {
    //         logger.error("消息发送失败");
    //     }
    // }
    // 增强SSL写入处理
    void sendMessage(const string &message)
    {
        int retryCount = 0;
        auto frame = buildWebSocketFrame(message);

        while (retryCount < 3)
        {
            {
                lock_guard<mutex> lock(sslMutex);
                int result = SSL_write(ssl, frame.data(), frame.size());

                if (result > 0)
                    return;

                int sslError = SSL_get_error(ssl, result);
                if (sslError == SSL_ERROR_WANT_WRITE || sslError == SSL_ERROR_WANT_READ)
                {
                    this_thread::sleep_for(chrono::milliseconds(50));
                    retryCount++;
                    continue;
                }

                logger.error("消息发送失败，SSL错误码: " + to_string(sslError));
            }

            if (retryCount >= 2)
            {
                handleError(SSL_ERROR_SYSCALL);
                attemptReconnect();
            }
            break;
        }
    }

    void handleError(int errorCode)
    {
        switch (errorCode)
        {
        case 1002:
            logger.error("协议违反：检查消息分帧和掩码处理");
            attemptReconnect();
            break;
        }
    }

    void cleanup()
    {
        connectionActive.store(false);
        vector<uint8_t> closeFrame = {0x88, 0x00};
        {
            lock_guard<mutex> lock(sslMutex);
            SSL_write(ssl, closeFrame.data(), closeFrame.size());
        }
        this_thread::sleep_for(chrono::milliseconds(100));

        if (heartbeatThread.joinable())
            heartbeatThread.join();
        if (messageProcessorThread.joinable())
            messageProcessorThread.join();

        lock_guard<mutex> lock(sslMutex);
        if (ssl)
        {
            SSL_shutdown(ssl);
            SSL_free(ssl);
            ssl = nullptr;
        }
        if (ctx)
        {
            SSL_CTX_free(ctx);
            ctx = nullptr;
        }
        if (sock != INVALID_SOCKET)
        {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }
        WSACleanup();
        logger.info("资源清理完成");
    }

    ~WebSocketClient()
    {
        cleanup();
    }

private:
    void init_openssl()
    {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();
        logger.info("OpenSSL初始化完成");
    }

    SSL_CTX *create_context()
    {
        const SSL_METHOD *method = TLS_client_method();
        SSL_CTX *ctx = SSL_CTX_new(method);
        if (!ctx)
        {
            logger.error("无法创建SSL上下文");
            exit(EXIT_FAILURE);
        }
        // 强制使用TLS 1.2及以上版本
        SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
        return ctx;
    }

    // Base64编码函数
    string base64_encode(const uint8_t *data, size_t len)
    {
        static const char base64_chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";
        string result;
        int val = 0, valb = -6;
        for (size_t i = 0; i < len; ++i)
        {
            val = (val << 8) + data[i];
            valb += 8;
            while (valb >= 0)
            {
                result.push_back(base64_chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6)
            result.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (result.size() % 4)
            result.push_back('=');
        return result;
    }

    // SHA-1哈希函数
    vector<uint8_t> sha1(const string &data)
    {
        vector<uint8_t> hash(SHA_DIGEST_LENGTH);
        SHA1(reinterpret_cast<const uint8_t *>(data.c_str()), data.size(), hash.data());
        return hash;
    }

    // 执行WebSocket握手协议
    bool performHandshake(const string &user_id)
    {
        // 生成随机的Sec-WebSocket-Key
        auto generateWebSocketKey = [this]() -> string
        {
            array<uint8_t, 16> key;
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(0, 255);
            for (auto &byte : key)
            {
                byte = static_cast<uint8_t>(dis(gen));
            }
            return base64_encode(key.data(), key.size());
        };

        string secWebSocketKey = generateWebSocketKey();
        const string handshake =
            "GET /server HTTP/1.1\r\n"
            "Host: " SERVER_HOST "\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Origin: https://" SERVER_HOST "\r\n" // 新增 Origin 头
            "Sec-WebSocket-Key: " +
            secWebSocketKey + "\r\n"
                              "Sec-WebSocket-Version: 13\r\n\r\n";

        {
            lock_guard<mutex> lock(sslMutex);
            if (SSL_write(ssl, handshake.c_str(), handshake.length()) <= 0)
            {
                logger.error("握手请求发送失败");
                return false;
            }
        }

        // 读取握手响应
        char buffer[4096] = {0};
        int bytesReceived = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (bytesReceived <= 0)
        {
            logger.error("握手响应读取失败");
            return false;
        }
        buffer[bytesReceived] = '\0';
        logger.info("握手响应: " + string(buffer));

        // 检查是否包含101状态码
        if (string(buffer).find("101") == string::npos)
        {
            logger.error("握手响应状态码不正确");
            return false;
        }

        // 验证 Sec-WebSocket-Accept 头
        string expectedAccept = base64_encode(sha1(secWebSocketKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11").data(), SHA_DIGEST_LENGTH);
        if (string(buffer).find("Sec-WebSocket-Accept: " + expectedAccept) == string::npos)
        {
            logger.error("Sec-WebSocket-Accept 验证失败");
            return false;
        }

        // 发送用户身份帧
        auto userIdFrame = buildWebSocketFrame(user_id);
        {
            lock_guard<mutex> lock(sslMutex);
            if (SSL_write(ssl, userIdFrame.data(), userIdFrame.size()) <= 0)
            {
                logger.error("用户ID发送失败");
                return false;
            }
        }
        return true;
    }

    // 心跳管理模块
    void startHeartbeat()
    {
        heartbeatThread = thread([this]
                                 {
            while(connectionActive.load()) {
                this_thread::sleep_for(chrono::milliseconds(HEARTBEAT_INTERVAL));
                
                // 发送Ping帧
                {
                    lock_guard<mutex> lock(sslMutex);
                    if(!sendPingFrame()) {
                        logger.error("心跳发送失败");
                        attemptReconnect();
                        return;
                    }else{
                        logger.info("发送Ping帧成功");
                    }
                }

                // 检查超时
                auto now = chrono::steady_clock::now();
                if(chrono::duration_cast<chrono::milliseconds>(now - lastPongTime).count() > HEARTBEAT_TIMEOUT) 
                {
                    logger.error("心跳超时，触发重连");
                    attemptReconnect();
                    return;
                }
            } });
    }

    // 用于构建 WebSocket 文本帧

    /* 📌 构造 WebSocket 文本帧 */
    vector<uint8_t> buildTextFrame(const string &message)
    {
        vector<uint8_t> frame;
        frame.push_back(0x81); // FIN=1, OPCODE=1 (文本)

        size_t msgLen = message.size();
        if (msgLen < 126)
        {
            frame.push_back(static_cast<uint8_t>(msgLen));
        }
        else if (msgLen < 65536)
        { // 126 ~ 65535
            frame.push_back(126);
            frame.push_back((msgLen >> 8) & 0xFF); // 高位
            frame.push_back(msgLen & 0xFF);        // 低位
        }
        else
        { // 长度 >= 65536
            frame.push_back(127);
            for (int i = 7; i >= 0; i--)
            {
                frame.push_back((msgLen >> (i * 8)) & 0xFF);
            }
        }

        // 加入消息内容
        frame.insert(frame.end(), message.begin(), message.end());
        return frame;
    }

    // ** WebSocket 处理结构**
    /* 📌 发送 WebSocket 消息 */
    bool sendWebSocketMessage(SSL *ssl, const string &message)
    {
        vector<uint8_t> frame = buildTextFrame(message);
        int result = SSL_write(ssl, frame.data(), frame.size());
        return (result > 0);
    }
    /* 📌 解析 WebSocket 响应数据 */
    string parseWebSocketMessage(const vector<uint8_t> &buffer)
    {
        if (buffer.size() < 2)
            return "";

        uint8_t opcode = buffer[0] & 0x0F; // 低 4 位是 OPCODE
        if (opcode == 0x8)
        {
            cout << "[🔴 服务器要求关闭连接]" << endl;
            return "";
        }

        size_t payloadLength = buffer[1] & 0x7F; // 低 7 位是数据长度
        size_t offset = 2;                       // 数据起始位置
        if (payloadLength == 126)
        {
            payloadLength = (buffer[2] << 8) | buffer[3];
            offset += 2;
        }
        else if (payloadLength == 127)
        {
            cout << "[❌ 错误] 数据太长!" << endl;
            return "";
        }

        if (buffer.size() < offset + payloadLength)
            return "";

        return string(buffer.begin() + offset, buffer.begin() + offset + payloadLength);
    }

    /* 📌 接收服务器消息 */
    string receiveWebSocketMessage(SSL *ssl)
    {
        vector<uint8_t> buffer(1024); // 1KB 缓冲区
        int bytesRead = SSL_read(ssl, buffer.data(), buffer.size());
        if (bytesRead > 0)
        {
            buffer.resize(bytesRead);
            return parseWebSocketMessage(buffer);
        }
        return "";
    }

    // 判断回复是否是"pong"
    bool isWebSocketPongFrame(const vector<char> &frame, int length)
    {
        if (length < 2)
            return false; // WebSocket 帧至少要有 2 字节

        uint8_t opcode = static_cast<uint8_t>(frame[0]) & 0x0F; // 取低 4 位，获取 Opcode
        if (opcode != 0x01)                                     // 确保是文本帧 (Opcode 0x01)
            return false;

        // 获取负载数据
        uint8_t payloadLen = static_cast<uint8_t>(frame[1]) & 0x7F; // 取低 7 位，获取 Payload 长度
        if (payloadLen + 2 > length)                                // 确保数据足够
            return false;

        // 解析文本数据
        string payload(frame.begin() + 2, frame.begin() + 2 + payloadLen);
        return payload == "pong"; // 服务器返回的消息是否是 "pong"
    }

    // 构造 WebSocket Ping 帧
    vector<uint8_t> buildPingFrame()
    {
        vector<uint8_t> frame;
        // FIN=1, Opcode=0x9 (Ping), Mask=1
        frame.push_back(0x89);

        // 生成4字节随机掩码
        auto maskKey = generateMaskKey();
        uint8_t maskBit = 0x80; // 设置MASK位

        // 构造Payload长度（示例使用4字节负载）
        uint8_t payloadLen = 4;
        frame.push_back(maskBit | payloadLen);

        // 添加掩码键
        frame.insert(frame.end(), maskKey.begin(), maskKey.end());

        // 生成带掩码的负载数据
        vector<uint8_t> payload = {0x01, 0x02, 0x03, 0x04}; // 示例心跳数据
        for (size_t i = 0; i < payload.size(); ++i)
        {
            payload[i] ^= maskKey[i % 4];
        }
        frame.insert(frame.end(), payload.begin(), payload.end());

        return frame;
    }

    // 发送 Ping 帧
    bool sendPingFrame()
    {
        vector<uint8_t> pingFrame = buildPingFrame();
        int result = SSL_write(ssl, pingFrame.data(), pingFrame.size());
        if (result > 0)
        {
            logger.info("[✅ ping 已发送]");
            return true;
        }
        else
        {
            int sslError = SSL_get_error(ssl, result);
            logger.error("[❌ ping 发送失败] SSL错误码: " + to_string(sslError));
            return false;
        }
    }

    // 发送 Pong 帧
    void sendPongFrame(const std::vector<uint8_t> &pingPayload)
    {
        if (!connectionActive)
        {
            logger.error("[ERROR] 连接未建立，无法发送 Pong");
            return;
        }

        std::vector<uint8_t> frame;
        frame.push_back(0x80 | 0x0A); // FIN=1, OPCODE=0xA (Pong)

        size_t payloadLen = pingPayload.size();

        if (payloadLen <= 125)
        {
            frame.push_back(0x80 | payloadLen); // Mask=1, 长度=payloadLen
        }
        else if (payloadLen <= 65535)
        {
            frame.push_back(0x80 | 126);
            frame.push_back((payloadLen >> 8) & 0xFF);
            frame.push_back(payloadLen & 0xFF);
        }
        else
        {
            frame.push_back(0x80 | 127);
            for (int i = 7; i >= 0; --i)
            {
                frame.push_back((payloadLen >> (i * 8)) & 0xFF);
            }
        }

        // 生成随机 4 字节掩码
        uint8_t mask[4];
        std::random_device rd;
        for (int i = 0; i < 4; ++i)
            mask[i] = rd() % 256;
        frame.insert(frame.end(), mask, mask + 4);

        // 应用掩码
        std::vector<uint8_t> maskedPayload = pingPayload;
        for (size_t i = 0; i < pingPayload.size(); ++i)
        {
            maskedPayload[i] ^= mask[i % 4]; // XOR 处理
        }
        frame.insert(frame.end(), maskedPayload.begin(), maskedPayload.end());

        int result = SSL_write(ssl, frame.data(), frame.size());
        if (result <= 0)
        {
            logger.error("[ERROR] 发送 Pong 失败, SSL错误码: " + std::to_string(SSL_get_error(ssl, result)));
        }
        else
        {
            logger.info("[INFO] 发送 Pong 帧成功");
        }
    }

    // 处理 Pong 帧
    void handlePongFrame(const vector<uint8_t> &frame)
    {
        lastPongTime = chrono::steady_clock::now();
        logger.info("收到 Pong 帧");
    }

    WebSocketFrame parseFrameStructure(const vector<uint8_t> &frame)
    {
        WebSocketFrame result;
        if (frame.size() < 2)
            return result;

        result.opcode = frame[0] & 0x0F;
        uint8_t payloadLenByte = frame[1] & 0x7F;
        size_t payloadStart = 2;

        // 处理扩展长度
        if (payloadLenByte == 126)
        {
            payloadStart += 2;
        }
        else if (payloadLenByte == 127)
        {
            payloadStart += 8;
        }

        // 处理掩码
        if ((frame[1] & 0x80) != 0)
        {
            payloadStart += 4;
            array<uint8_t, 4> maskKey = {
                frame[payloadStart - 4], frame[payloadStart - 3],
                frame[payloadStart - 2], frame[payloadStart - 1]};

            for (size_t i = 0; i < frame.size() - payloadStart; ++i)
            {
                result.payload.push_back(frame[payloadStart + i] ^ maskKey[i % 4]);
            }
        }
        else
        {
            result.payload.assign(frame.begin() + payloadStart, frame.end());
        }

        return result;
    }

    // 处理关闭帧
    void handleCloseFrame(const vector<uint8_t> &frame)
    {
        uint16_t statusCode = 0;
        if (frame.size() >= 4)
        {
            statusCode = (frame[2] << 8) | frame[3];
        }
        logger.info("收到关闭帧，状态码: " + to_string(statusCode));

        if (statusCode == 4000)
        { // 自定义强制下线代码
            lock_guard<mutex> lock(queueMutex);
            msgQueue.push("{\"type\":\"force_logout\"}");
            queueCV.notify_one();
        }

        connectionActive.store(false);

        // 发送关闭帧响应
        vector<uint8_t> closeFrame = {0x88, 0x00};
        lock_guard<mutex> lock(sslMutex);
        int result = SSL_write(ssl, closeFrame.data(), closeFrame.size());
        if (result <= 0)
        {
            logger.error("关闭帧响应发送失败: " + to_string(SSL_get_error(ssl, result)));
        }
        else
        {
            logger.info("关闭帧响应发送成功");
        }
    }

    void startMessageProcessor()
    {
        messageProcessorThread = thread([this]
                                        {
            unique_lock<mutex> lock(queueMutex);
            while(connectionActive.load()) {
                queueCV.wait(lock, [this]{ 
                    return !msgQueue.empty() || !connectionActive.load(); 
                });

                while(!msgQueue.empty()) {
                    string msg = msgQueue.front();
                    msgQueue.pop();
                    
                    if(msg.find("\"type\":\"force_logout\"") != string::npos) {
                        cout << "[系统] 检测到重复登录，服务端强制下线" << endl;
                        // getch();
                        exit(EXIT_FAILURE);
                    }
                    else {
                        cout << "服务端消息 >> " << msg << endl;
                    }
                    
                }
                
            } });
    }

    void processSystemMessage(const string &msg)
    {
        if (msg.find("\"type\":\"force_logout\"") != string::npos)
        {
            logger.error("服务端强制下线");
            cleanup();
            exit(EXIT_FAILURE);
        }
    }

    

    void attemptReconnect()
    {
        cleanup();                  // 释放资源
        reconnectAttempts.store(0); // 重置计数

        while (reconnectAttempts.load() < MAX_RECONNECT_ATTEMPTS)
        {
            const int delay = RECONNECT_BASE_DELAY * (1 << reconnectAttempts.load()); // 指数退避：5s、10s、20s、...

            logger.info("等待 " + to_string(delay) + "ms 后重试连接 (" + to_string(reconnectAttempts.load() + 1) + "/5)");
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));

            if (connectServer(lastUserID))
            {
                logger.info("重连成功 ✅");
                reconnectAttempts.store(0);
                return;
            }

            reconnectAttempts.fetch_add(1);
        }

        logger.error("超过最大重连次数，放弃重连 ❌");
    }

    string parseWebSocketFrame(const vector<uint8_t> &frame)
    {
        if (frame.size() < 2)
            return "";
        return string(frame.begin(), frame.end());
    }

    void printHexData(const vector<char> &data, int length)
    {
        cout << "Received Data (Hex): ";
        for (int i = 0; i < length; i++)
        {
            printf("%02X ", static_cast<unsigned char>(data[i]));
        }
        cout << endl;
    }

    // 辅助函数
    vector<uint8_t> generateMaskKey()
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 255);
        return {static_cast<uint8_t>(dis(gen)),
                static_cast<uint8_t>(dis(gen)),
                static_cast<uint8_t>(dis(gen)),
                static_cast<uint8_t>(dis(gen))};
    }

    MOUSEMSG parseMouseMsg(const std::string &msgStr)
    {
        MOUSEMSG msg;
        // Add logic to parse msgStr and populate the MOUSEMSG object
        // Example: msg.x = extractX(msgStr); msg.y = extractY(msgStr);
        return msg;
    }

    bool is_valid_utf8(const string &str)
    {
        int bytes_remaining = 0;
        for (auto c : str)
        {
            uint8_t byte = static_cast<uint8_t>(c);
            if (bytes_remaining == 0)
            {
                if (byte <= 0x7F)
                    continue;
                if ((byte & 0xE0) == 0xC0)
                    bytes_remaining = 1;
                else if ((byte & 0xF0) == 0xE0)
                    bytes_remaining = 2;
                else if ((byte & 0xF8) == 0xF0)
                    bytes_remaining = 3;
                else
                    return false;
            }
            else
            {
                if ((byte & 0xC0) != 0x80)
                    return false;
                --bytes_remaining;
            }
        }
        return bytes_remaining == 0;
    }
};

WebSocketClient client;

#endif // CLIENT_SERVER_H

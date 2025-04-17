

# Echo Pool

## 一款用C++编写的在线台球游戏

<!-- PROJECT SHIELDS -->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

<!-- PROJECT LOGO -->
<br />

  <h3 align="center">Echo Pool</h3>
  <p align="center">
    一款用C++编写的在线台球游戏
    <br />
    <br />
    ·
    <a href="https://github.com/lhl77/billiards-online-cpp/issues">报告Bug</a>
    ·
    <a href="https://github.com/lhl77/billiards-online-cpp/issues">提出新特性</a>
  </p>

</p>


 本篇README.md面向开发者

## 目录

- [部署](#部署)
- [使用到的开源库](#使用到的开源库)


### 部署

1. 将`sql/data.sql`导入Mysql数据库
2. 将`php-server`内的文件导入php服务器，然后修改conn.php中的Mysql数据库账号密码
3. 设置Nginx伪静态

```
location / {
    try_files $uri $uri/ /$uri.php?$query_string;
}
```
4. 使用Python启动ws-server，在ws-server/server中`python server.py`
5. 设置Nginx反向代理
```
    location /server {
        proxy_pass http://127.0.0.1:25689/;  # 将请求转发到 WebSocket 服务器
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
        # 传递真实客户端信息[3,8](@ref)
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        # 保持长连接配置
        proxy_connect_timeout 7d;
        proxy_send_timeout 7d;
        proxy_read_timeout 7d;
	    keepalive_timeout 75;
        # TCP优化
        tcp_nodelay on;
        tcp_nopush on;
    }
```
6. 设置客户端，在`client/include/game_config.h`中修改`API_URL`为你的域名，然后编译即可。
7. 在`client`目录下运行`make`编译，编译好的文件在`build`文件夹中

### 使用到的开源库

- [EasyX](#)
- [openssl](https://github.com/openssl/openssl)
- [curl](https://github.com/curl/curl)

由于本项目是学校某课程要求做的，强制限定了需要用EasyX，故只能用了。



<!-- links -->
[your-project-path]:lhl77/billiards-online-cpp
[contributors-shield]: https://img.shields.io/github/contributors/lhl77/billiards-online-cpp.svg?style=flat-square
[contributors-url]: https://github.com/lhl77/billiards-online-cpp/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/lhl77/billiards-online-cpp.svg?style=flat-square
[forks-url]: https://github.com/lhl77/billiards-online-cpp/network/members
[stars-shield]: https://img.shields.io/github/stars/lhl77/billiards-online-cpp.svg?style=flat-square
[stars-url]: https://github.com/lhl77/billiards-online-cpp/stargazers
[issues-shield]: https://img.shields.io/github/issues/lhl77/billiards-online-cpp.svg?style=flat-square
[issues-url]: https://img.shields.io/github/issues/lhl77/billiards-online-cpp.svg
[license-shield]: https://img.shields.io/github/license/lhl77/billiards-online-cpp.svg?style=flat-square
[license-url]: https://github.com/lhl77/billiards-online-cpp/blob/master/LICENSE.txt






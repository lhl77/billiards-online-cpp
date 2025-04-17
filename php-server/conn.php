<?php
// 1.设置连接数据库的信息
$local = "localhost";// 一般都填localhost
$username = "username"; // 数据库用户名，需要自己修改
$password = "password"; // 数据库密码，需要自己修改
$dbname = "dbname"; // 要连接的数据库名，需要自己修改
// 2.连接数据库
$conn = new mysqli($local, $username, $password, $dbname);

// 设置 UTF-8MB4 编码
$conn->set_charset("utf8mb4");

header('Content-Type: application/json; charset=UTF-8');

?>
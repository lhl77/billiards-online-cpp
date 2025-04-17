<?php
require 'conn.php';

if (isset($_GET['username'])) {
    $username = $conn->real_escape_string($_GET['username']);

    // 身份验证
    $user_id = getUserId($username);

    // 查询用户皮肤信息
    $query = "SELECT user_characters FROM user_characters WHERE user_id = $user_id";
    $result = $conn->query($query);

    if ($row = $result->fetch_assoc()) {
        echo json_encode(["user_characters" => $row['user_characters']]);
    } else {
        echo json_encode(["error" => "用户皮肤数据不存在"]);
    }
}

// 获取用户 ID
function getUserId($username) {
    global $conn;
    $query = "SELECT id FROM Users WHERE username = '$username'";
    $result = $conn->query($query);
    
    if ($row = $result->fetch_assoc()) {
        return $row['id'];
    }
    return null;
}
?>

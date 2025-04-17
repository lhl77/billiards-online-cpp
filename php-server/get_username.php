<?php
require 'conn.php';

if (isset($_GET['user_id'])) {
    $user_id = $conn->real_escape_string($_GET['user_id']);

    // 查询用户皮肤信息
    $query = "SELECT username FROM Users WHERE id = $user_id";
    $result = $conn->query($query);

    if ($row = $result->fetch_assoc()) {
        echo json_encode(["username" => $row['username']]);
    } else {
        echo json_encode(["error" => "用户不存在"]);
    }
}


?>

<?php
require 'conn.php';

if (isset($_GET['username'])) {
    $username = $conn->real_escape_string($_GET['username']);

    // 查询用户皮肤信息
    $query = "SELECT id FROM Users WHERE username = '$username'";
    $result = $conn->query($query);

    if ($row = $result->fetch_assoc()) {
        echo json_encode(["user_id" => $row['id']]);
    } else {
        echo json_encode(["error" => "用户不存在"]);
    }
}


?>

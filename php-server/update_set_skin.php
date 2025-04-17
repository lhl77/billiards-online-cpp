<?php
require 'conn.php';

if (isset($_POST['username'], $_POST['password'], $_POST['skin_id'])) {
    $username = $conn->real_escape_string($_POST['username']);
    $password = $conn->real_escape_string($_POST['password']);
    $skin_id = intval($_POST['skin_id']);

    // 身份验证
    $user_id = getUserId($username, $password);
    if ($user_id === null) {
        echo json_encode(["error" => "身份验证失败"]);
        return;
    }

    $query = "UPDATE user_characters SET user_character_set = $skin_id WHERE user_id = '$user_id'";
    if ($conn->query($query) === TRUE) {
        echo json_encode(["success" => true]);
    } else {
        echo json_encode(["error" => $conn->error]);
    }
}

// 获取用户 ID（身份验证）
function getUserId($username, $password) {
    global $conn;
    $query = "SELECT id FROM Users WHERE username = '$username' AND password = '$password'";
    $result = $conn->query($query);
    
    if ($row = $result->fetch_assoc()) {
        return $row['id'];
    }
    return null;
}
?>

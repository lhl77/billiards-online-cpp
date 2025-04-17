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
    
    $user_characters = getUserCharacters($user_id);
    // 处理现有角色和新皮肤的拼接
    if (empty($user_characters)) {
        $new_characters = $skin_id;
    } else {
        $new_characters = $user_characters . ',' . $skin_id;
    }
    $new_characters = $conn->real_escape_string($new_characters);
    
    // 检查是否存在用户角色记录
    $checkQuery = "SELECT user_id FROM user_characters WHERE user_id = $user_id";
    $result = $conn->query($checkQuery);
    if ($result->num_rows === 0) {
        // 不存在记录，执行INSERT
        $query = "INSERT INTO user_characters (user_id, user_characters) VALUES ($user_id, '$new_characters')";
    } else {
        // 存在记录，执行UPDATE
        $query = "UPDATE user_characters SET user_characters = '$new_characters' WHERE user_id = $user_id";
    }
    
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

function getUserCharacters($user_id) {
    global $conn;
    $query = "SELECT user_characters FROM user_characters WHERE user_id = $user_id";
    $result = $conn->query($query);
    
    if ($row = $result->fetch_assoc()) {
        return $row['user_characters'];
    }
    return null;
}
?>
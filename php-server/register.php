<?php
require 'conn.php';

if (isset($_POST['username']) && isset($_POST['password'])) {
    $username = $conn->real_escape_string($_POST['username']);
    $password = $conn->real_escape_string($_POST['password']);

    $query = "INSERT INTO Users (username, password, coins) VALUES ('$username', '$password', 1)";
    
    if ($conn->query($query) === TRUE) {
        $user_id = intval(getUserId($username));
        $queryAddDefaultCharacter = "INSERT INTO user_characters (user_id, user_character_set, user_characters) VALUES ($user_id,1,'1')";
        $conn->query($queryAddDefaultCharacter);
        echo json_encode(["success" => true]);
    } else {
        echo json_encode(["error" => $conn->error]);
    }
    
}
// 获取用户 ID（身份验证）
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
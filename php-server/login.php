<?php
require 'conn.php';

if (isset($_POST['username']) && isset($_POST['password'])) {
    $username = $conn->real_escape_string($_POST['username']);
    $password = $conn->real_escape_string($_POST['password']);

    $query = "SELECT id, username, password FROM Users WHERE username = '$username'";
    $result = $conn->query($query);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        if ($password === $row['password']) {
            echo json_encode(["success" => true, "username" => $row['username']]);
        } else {
            echo json_encode(["success" => false, "error" => "密码错误"]);
        }
    } else {
        echo json_encode(["success" => false, "error" => "用户名不存在"]);
    }
}
?>

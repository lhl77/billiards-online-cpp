<?php
require 'conn.php';

if (isset($_POST['username'], $_POST['password'], $_POST['addcoins'])) {
    $username = $conn->real_escape_string($_POST['username']);
    $password = $conn->real_escape_string($_POST['password']);
    $addcoins = intval($_POST['addcoins']);

    if (!authenticateUser($username, $password)) {
        echo json_encode(["error" => "身份验证失败"]);
        return;
    }

    $query = "UPDATE Users SET coins = coins + $addcoins WHERE username = '$username'";
    if ($conn->query($query) === TRUE) {
        echo json_encode(["success" => true]);
    } else {
        echo json_encode(["error" => $conn->error]);
    }
}

function authenticateUser($username, $password) {
    global $conn;
    $query = "SELECT username FROM Users WHERE username = '$username' AND password = '$password'";
    $result = $conn->query($query);
    return $result->num_rows > 0;
}
?>

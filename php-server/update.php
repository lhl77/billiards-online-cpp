<?php
require 'conn.php';

function executeUpdate($username, $password, $query) {
    global $conn;

    if (!authenticateUser($username, $password)) {
        echo json_encode(["error" => "身份验证失败"]);
        return;
    }

    if ($conn->query($query) === TRUE) {
        echo json_encode(["success" => true]);
    } else {
        echo json_encode(["error" => $conn->error]);
    }
}

if (isset($_POST['username'], $_POST['password'], $_POST['query'])) {
    executeUpdate($_POST['username'], $_POST['password'], $_POST['query']);
}

function authenticateUser($username, $password) {
    global $conn;
    $username = $conn->real_escape_string($username);
    $password = $conn->real_escape_string($password);
    $query = "SELECT username FROM Users WHERE username = '$username' AND password = '$password'";
    $result = $conn->query($query);
    return $result->num_rows > 0;
}
?>

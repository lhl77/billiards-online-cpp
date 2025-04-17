<?php
require 'conn.php';

function executeQuery($username, $password, $query) {
    global $conn;

    if (!authenticateUser($username, $password)) {
        echo json_encode(["error" => "身份验证失败"]);
        return;
    }

    $result = $conn->query($query);
    if ($result) {
        $data = [];
        while ($row = $result->fetch_assoc()) {
            $data[] = $row;
        }
        echo json_encode($data);
    } else {
        echo json_encode(["error" => $conn->error]);
    }
}

if (isset($_POST['username'], $_POST['password'], $_POST['query'])) {
    executeQuery($_POST['username'], $_POST['password'], $_POST['query']);
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

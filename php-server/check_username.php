<?php
require 'conn.php';

if (isset($_GET['username'])) {
    $username = $conn->real_escape_string($_GET['username']);

    $query = "SELECT username FROM Users WHERE username = '$username'";
    $result = $conn->query($query);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        echo json_encode(["success" => true, "username" => $username]);
    } else {
        echo json_encode(["success" => false, "error" => "用户名不存在"]);
    }
}
?>

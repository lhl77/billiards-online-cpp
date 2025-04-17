<?php
require 'conn.php';

if (isset($_GET['username'])) {
    $username = $conn->real_escape_string($_GET['username']);


    $query = "SELECT coins FROM Users WHERE username = '$username'";
    $result = $conn->query($query);

    if ($row = $result->fetch_assoc()) {
        echo json_encode(["coins" => $row['coins']]);
    } else {
        echo json_encode(["error" => "用户不存在"]);
    }
}


?>

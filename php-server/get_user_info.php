<?php
require 'conn.php';

if (isset($_GET['username'])) {
    $username = $conn->real_escape_string($_GET['username']);

    $query = "SELECT coins,win_count,id,total_count FROM Users WHERE username = '$username'";
    $result = $conn->query($query);

    if ($row = $result->fetch_assoc()) {
        echo json_encode(["user_id"=>$row['id'],"coins" => $row['coins'],"win_count"=>$row['win_count'],"total_count"=>$row['total_count'],"coins"=>$row['coins']]);
    } else {
        echo json_encode(["error" => "用户不存在"]);
    }
}

?>

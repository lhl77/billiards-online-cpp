<?php
require 'conn.php';

// 查询characters表的记录数
$query = "SELECT COUNT(*) AS nums FROM characters";
$result = $conn->query($query);

if ($result === false) {
    // 查询失败，返回错误信息
    echo json_encode(["error" => "查询失败: " . $conn->error]);
} else {
    // 查询成功，获取结果
    $row = $result->fetch_assoc();
    if ($row) {
        // 返回记录数
        echo json_encode(["nums" => $row['nums']]);
    } else {
        // 表为空
        echo json_encode(["error" => "表为空"]);
    }
}
?>
<?php
require 'conn.php';

if (isset($_GET['id'])) {
    $id = $conn->real_escape_string($_GET['id']);

    // 查询用户皮肤信息
    $query = "SELECT * FROM characters WHERE id = $id";
    $result = $conn->query($query);

    if ($row = $result->fetch_assoc()) {
        echo json_encode(["id" => $row['id'],"name"=>$row['name'],"description"=>$row['description'],"price"=>$row['price'],"url_big"=>$row['url_big'],"url_small"=>$row['url_small'],"url_bg"=>$row['url_bg']]);
    } else {
        echo json_encode(["error" => "用户皮肤数据不存在"]);
    }
}else{
    $query = "SELECT * FROM characters";
    $result = $conn->query($query);
    $arr = [];
    if ($result->num_rows > 0) {
        while($row = $result->fetch_assoc()) {
		    array_push($arr,$row);
        }
    }
    echo json_encode($arr);
}



?>

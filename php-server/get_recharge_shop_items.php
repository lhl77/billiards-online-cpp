<?php
// 1.连接数据库
require_once("conn.php");

// 2.判断是否连接成功
if ($conn->connect_error) {
    die("连接失败: " . $conn->connect_error);
} 
// 3.编写sql语句
$sql = "SELECT * FROM recharge_shop";
// 4.执行sql语句
$result = $conn->query($sql);
// 5.创建一个空数组，方便等会往其中添加获取到的数据库数据
$arr = [];
// 6.判断sql语句是否执行成功
if ($result->num_rows > 0) {
    // 循环输出数据
    while($row = $result->fetch_assoc()) {
        // var_dump($row);
		// 获取name
		// $name = $row['name'];
		// var_dump($name);
		// 7.将获取到的数据添加到数组中
		array_push($arr,$row);
    }
}
// 8.输出转化为json格式的数组
echo json_encode($arr);
// 9.关闭连接
$conn->close();
?>



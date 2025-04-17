<?php
require 'conn.php';

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $username = $_POST["username"] ?? "";
    $user_id = $_POST["user_id"] ?? "";
    $item_id = $_POST["item_id"] ?? "";
    $item_price = $_POST["item_price"] ?? "";

    if (!$username || !$user_id || !$item_id || !$item_price) {
        echo json_encode(["success" => false, "error" => "缺少参数"]);
        exit;
    }

    // **获取当前金币**
    $query = "SELECT coins FROM Users WHERE username = ?";
    $stmt = $conn->prepare($query);
    $stmt->bind_param("s", $username);
    $stmt->execute();
    $stmt->bind_result($current_coins);
    if (!$stmt->fetch()) {
        echo json_encode(["success" => false, "error" => "用户不存在"]);
        exit;
    }
    $stmt->close();

    // **检查金币是否足够**
    if ($current_coins < $item_price) {
        echo json_encode(["success" => false, "error" => "金币不足"]);
        exit;
    }

    // **开启事务**
    $conn->begin_transaction();

    try {
        // **扣除金币**
        $new_coins = $current_coins - $item_price;
        $query = "UPDATE Users SET coins = ? WHERE username = ?";
        $stmt = $conn->prepare($query);
        $stmt->bind_param("is", $new_coins, $username);
        $stmt->execute();
        $stmt->close();

        // **添加皮肤（修正 SQL 语句）**
        $query = "UPDATE user_characters SET user_characters = CONCAT_WS(',', user_characters, ?) WHERE user_id = ?";
        $stmt = $conn->prepare($query);
        $stmt->bind_param("ii", $item_id, $user_id);
        $stmt->execute();
        $stmt->close();

        // **提交事务**
        $conn->commit();

        echo json_encode(["success" => true, "new_coins" => $new_coins]);
    } catch (Exception $e) {
        // **回滚事务**
        $conn->rollback();
        echo json_encode(["success" => false, "error" => "购买失败"]);
    }
}
?>

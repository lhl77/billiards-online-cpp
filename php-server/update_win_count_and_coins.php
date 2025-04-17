<?php
require 'conn.php';

if (isset($_POST['user_id'],$_POST['isWin'])) {
    $user_id = intval($_POST['user_id']);
    $isWin = intval($_POST['isWin']);
    
    if ($isWin == 1){
        $query = "UPDATE Users SET coins = coins + 2 , win_count = win_count + 1, total_count = total_count + 1 WHERE id = '$user_id'";
    }else{
        $query = "UPDATE Users SET total_count = total_count + 1 WHERE id = '$user_id'";
    }

    if ($conn->query($query) === TRUE) {
        echo json_encode(["success" => true]);
    } else {
        echo json_encode(["error" => $conn->error]);
    }
}

?>

<?php
header("Content-Type: text/plain");

echo "支付宝API接口，用于刘瀚霖的台球游戏\n";
echo "v1.0 by 刘瀚霖\n\n";

echo "生成支付宝付款二维码 - /generate_qrcode - POST\n";
echo "参数(body)：totalAmount, subject（PHP 自动生成订单号）\n";
echo "返回：{\"success\": true, \"orderNo\": \"ORDER123\", \"qr_code\": \"https://...\"}\n\n";

echo "查询订单支付状态 - /api/query_order_status? - POST\n";
echo "参数(body)：orderNo\n";
echo "返回：{\"success\": true, \"status\": \"WAIT_BUYER_PAY\"}\n";
?>

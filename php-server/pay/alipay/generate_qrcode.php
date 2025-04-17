<?php
require 'alipay_config.php';

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $totalAmount = $_POST["totalAmount"] ?? ""; 
    $subject = $_POST["subject"] ?? "商品";

    if (!$totalAmount) {
        echo json_encode(["success" => false, "error" => "金额不能为空"]);
        exit;
    }
    
    $orderNo = generateOrderNo(); // 生成唯一订单号

    $bizContent = json_encode([
        "out_trade_no" => $orderNo,
        "total_amount" => $totalAmount,
        "subject" => $subject,
        "product_code" => "FACE_TO_FACE_PAYMENT"
    ]);

    $params = [
        "app_id" => APP_ID,
        "method" => "alipay.trade.precreate",
        "charset" => "utf-8",
        "sign_type" => "RSA2",
        "timestamp" => date("Y-m-d H:i:s"),
        "version" => "1.0",
        "biz_content" => $bizContent
    ];

    $params["sign"] = signParams($params);
    $url = ALIPAY_GATEWAY . "?" . http_build_query($params);
    
    $response = file_get_contents($url);
    $result = json_decode($response, true);

    if (isset($result["alipay_trade_precreate_response"]["code"]) && $result["alipay_trade_precreate_response"]["code"] === "10000") {
        echo json_encode([
            "success" => true,
            "orderNo" => $orderNo,
            "qr_code" => $result["alipay_trade_precreate_response"]["qr_code"]
        ]);
    } else {
        // echo json_encode(["success" => false, "error" => "支付宝 API 调用失败"]);
        // 直接输出支付宝返回的结果
        echo json_encode($result, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE);
    }
}
?>
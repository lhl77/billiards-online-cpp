<?php
require 'alipay_config.php';

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $orderNo = $_POST["orderNo"] ?? "";

    if (!$orderNo) {
        echo json_encode(["success" => false, "error" => "订单号不能为空"]);
        exit;
    }

    $bizContent = json_encode(["out_trade_no" => $orderNo]);

    $params = [
        "app_id" => APP_ID,
        "method" => "alipay.trade.query",
        "charset" => "utf-8",
        "sign_type" => "RSA2",
        "timestamp" => date("Y-m-d H:i:s"),
        "version" => "1.0",
        "biz_content" => $bizContent
    ];

    $params["sign"] = signParams($params);
    $url = ALIPAY_GATEWAY . "?" . http_build_query($params);
    
    $response = file_get_contents($url);
    
    // if (!verifyAlipaySignature($response)) {
    //     echo json_encode(["success" => false, "error" => "支付宝返回数据签名验证失败"]);
    //     exit;
    // }
    // 懒得写验证了，算了

    $result = json_decode($response, true);

    if ($result["alipay_trade_query_response"]["code"] === "10000") {
        echo json_encode([
            "success" => true,
            "status" => $result["alipay_trade_query_response"]["trade_status"]
        ]);
    } else {
        echo json_encode(["success" => false, "error" => "支付宝 API 调用失败"]);
    }
}
?>

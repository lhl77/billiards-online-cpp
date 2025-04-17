<?php
define("APP_ID", "xxxxxxxxxxx"); // 替换为你的支付宝 APP_ID
define("ALIPAY_GATEWAY", "https://openapi.alipay.com/gateway.do");
define("PRIVATE_KEY_PATH", "rsa_private_key.pem");  // RSA 私钥路径
define("PUBLIC_KEY_PATH", "alipay_public_key.pem"); // 支付宝公钥路径

// **生成订单号**
function generateOrderNo() {
    return "ORDER" . date("YmdHis") . rand(1000, 9999);
}

// **RSA 签名**
function signParams($params) {
    ksort($params); // 参数按键名排序
    $signData = urldecode(http_build_query($params));

    $privateKey = file_get_contents(PRIVATE_KEY_PATH);
    openssl_sign($signData, $signature, $privateKey, OPENSSL_ALGO_SHA256);
    
    return base64_encode($signature);
}

// **验证支付宝返回的签名**
function verifyAlipaySignature($response) {
    $responseData = json_decode($response, true);
    if (!$responseData) {
        error_log("支付宝响应解析失败: " . $response);
        return false;
    }

    // 获取 sign 和 sign_type
    $sign = $responseData['sign'] ?? '';
    $signType = $responseData['sign_type'] ?? 'RSA2';
    unset($responseData['sign'], $responseData['sign_type']);

    // 参数按字母排序
    ksort($responseData);
    $dataToVerify = urldecode(http_build_query($responseData));

    // 支付宝公钥读取校验
    $publicKey = file_get_contents(PUBLIC_KEY_PATH);
    if (!$publicKey) {
        error_log("支付宝公钥读取失败: " . PUBLIC_KEY_PATH);
        return false;
    }

    // 验证签名
    $publicKeyResource = openssl_get_publickey($publicKey);
    if (!$publicKeyResource) {
        error_log("支付宝公钥格式错误");
        return false;
    }

    $result = openssl_verify(
        $dataToVerify, 
        base64_decode($sign), 
        $publicKeyResource, 
        $signType === 'RSA2' ? OPENSSL_ALGO_SHA256 : OPENSSL_ALGO_SHA1
    );
    openssl_free_key($publicKeyResource);

    return $result === 1;
}
?>

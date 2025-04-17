<?php
header("Content-Type: text/plain");

echo "本API接口，用于刘瀚霖的台球游戏 - 响带台球\n";
echo "v1.0 by 刘瀚霖\n\n";

echo "为指定username添加coins - /add_coins - POST\n";
echo "参数(body)：username, password, addcoins\n";
echo "返回：{\"success\": true}\n\n";

echo "查询是否含有用户名 - /check_username - POST\n";
echo "参数(body)：username\n";
echo "返回：{\"success\": true}\n\n";

echo "查询角色信息 - /get_character_details - GET\n";
echo "参数(body)：id(可选)\n";
echo "有id返回：{\"id\": \"1\",\"name\": \"小白\",\"description\": \"...\",\"price\": \"10\",\"url_big\": \"https://...\",\"url_small\": \"https://...\",\"url_bg\": \"https://...\"}\n";
echo "无id返回全部角色数组\n\n";

echo "...正在编写中\n\n";
echo "支付接口API: /pay/alipay";

?>

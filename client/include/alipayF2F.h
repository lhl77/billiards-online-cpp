#ifndef ALIPAYF2F_H
#define ALIPAYF2F_H

#include <optional>
#include <chrono>
#include <string>

using namespace std;

// 自定义头文件
#include <game_config.h>
#include <httplib.h> // HTTP 请求相关函数
#include <json.h>    // JSON 解析器类

// **支付宝 F2F 支付类**
class AlipayF2F
{
public:
    string generateQRcode(string totalAmount,string subject){
        string url_generateQRcode = string(API_URL) + "/pay/alipay/generate_qrcode";
        string data_generateQRcode = "totalAmount=" + totalAmount + "&subject=" + subject;
        string result_generateQRcode = sendPostRequest(url_generateQRcode, data_generateQRcode);
        return result_generateQRcode;
    }
    string queryOrderStatus(string order_id){
        string url_queryOrderStatus = string(API_URL) + "/pay/alipay/query_order_status";
        string data_queryOrderStatus = "orderNo=" + order_id;
        string result_queryOrderStatus = sendPostRequest(url_queryOrderStatus, data_queryOrderStatus);
        if(jsonParser.extractJsonValue(result_queryOrderStatus,"error") != ""){
            // cerr << jsonParser.extractJsonValue(result_queryOrderStatus,"error") << endl;
            return "error";
        }else if(jsonParser.extractJsonValue(result_queryOrderStatus,"error") == ""){
            // cerr << jsonParser.extractJsonValue(result_queryOrderStatus,"status") << endl;
            return jsonParser.extractJsonValue(result_queryOrderStatus,"status");
        }
        return "";
    }
};
AlipayF2F alipay;



#endif // ALIPAYF2F_H
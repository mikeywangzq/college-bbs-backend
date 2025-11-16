#pragma once

#include <drogon/HttpResponse.h>
#include <json/json.h>

using namespace drogon;

/**
 * 响应工具类
 * 用于生成统一格式的JSON响应
 */
class ResponseUtil {
public:
    // 错误码定义
    enum ErrorCode {
        SUCCESS = 0,            // 成功
        PARAM_ERROR = 1001,     // 参数错误
        USER_EXISTS = 1002,     // 用户名已存在
        USER_NOT_FOUND = 1003,  // 用户不存在
        WRONG_PASSWORD = 1004,  // 密码错误
        TOKEN_INVALID = 1005,   // Token无效或过期
        NO_PERMISSION = 1006,   // 无权限操作
        POST_NOT_FOUND = 1007,  // 帖子不存在
        REPLY_NOT_FOUND = 1008, // 回复不存在
        DB_ERROR = 1009,        // 数据库错误
        SERVER_ERROR = 1010     // 服务器内部错误
    };

    /**
     * 成功响应
     * @param data 响应数据（可选）
     * @param msg 响应消息
     * @return HttpResponse
     */
    static HttpResponsePtr success(const Json::Value& data = Json::Value::null,
                                   const std::string& msg = "success");

    /**
     * 失败响应
     * @param code 错误码
     * @param msg 错误消息
     * @return HttpResponse
     */
    static HttpResponsePtr error(int code, const std::string& msg);

    /**
     * 获取错误消息
     * @param code 错误码
     * @return 错误消息
     */
    static std::string getErrorMessage(int code);
};

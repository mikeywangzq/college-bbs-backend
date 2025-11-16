#include "ResponseUtil.h"

HttpResponsePtr ResponseUtil::success(const Json::Value& data, const std::string& msg) {
    Json::Value response;
    response["code"] = SUCCESS;
    response["msg"] = msg;
    response["data"] = data;

    auto resp = HttpResponse::newHttpJsonResponse(response);
    return resp;
}

HttpResponsePtr ResponseUtil::error(int code, const std::string& msg) {
    Json::Value response;
    response["code"] = code;
    response["msg"] = msg.empty() ? getErrorMessage(code) : msg;
    response["data"] = Json::Value::null;

    auto resp = HttpResponse::newHttpJsonResponse(response);
    return resp;
}

std::string ResponseUtil::getErrorMessage(int code) {
    switch (code) {
        case SUCCESS:
            return "success";
        case PARAM_ERROR:
            return "参数错误";
        case USER_EXISTS:
            return "用户名已存在";
        case USER_NOT_FOUND:
            return "用户不存在";
        case WRONG_PASSWORD:
            return "密码错误";
        case TOKEN_INVALID:
            return "Token无效或过期";
        case NO_PERMISSION:
            return "无权限操作";
        case POST_NOT_FOUND:
            return "帖子不存在";
        case REPLY_NOT_FOUND:
            return "回复不存在";
        case DB_ERROR:
            return "数据库错误";
        case SERVER_ERROR:
            return "服务器内部错误";
        default:
            return "未知错误";
    }
}

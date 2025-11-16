#include "AuthFilter.h"
#include "../utils/JwtUtil.h"
#include "../utils/ResponseUtil.h"

void AuthFilter::doFilter(const HttpRequestPtr& req,
                         FilterCallback&& fcb,
                         FilterChainCallback&& fccb) {
    // 从Header中获取Token
    std::string authHeader = req->getHeader("Authorization");

    if (authHeader.empty()) {
        // 没有提供Token
        auto resp = ResponseUtil::error(ResponseUtil::TOKEN_INVALID, "未提供Token");
        fcb(resp);
        return;
    }

    // Token格式: Bearer <token>
    std::string token;
    if (authHeader.substr(0, 7) == "Bearer ") {
        token = authHeader.substr(7);
    } else {
        token = authHeader;
    }

    // 验证Token
    int user_id;
    std::string username;

    if (!JwtUtil::verifyToken(token, user_id, username)) {
        // Token无效或过期
        auto resp = ResponseUtil::error(ResponseUtil::TOKEN_INVALID, "Token无效或过期");
        fcb(resp);
        return;
    }

    // Token验证成功，将用户信息存储到request的attributes中
    req->attributes()->insert("user_id", user_id);
    req->attributes()->insert("username", username);

    // 继续处理请求
    fccb();
}

#pragma once

#include <drogon/HttpFilter.h>

using namespace drogon;

/**
 * 认证过滤器
 * 用于验证JWT Token，保护需要登录的接口
 */
class AuthFilter : public HttpFilter<AuthFilter> {
public:
    AuthFilter() {}

    void doFilter(const HttpRequestPtr& req,
                 FilterCallback&& fcb,
                 FilterChainCallback&& fccb) override;
};

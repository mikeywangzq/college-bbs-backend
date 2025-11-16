/**
 * @file AuthFilter.h
 * @brief JWT认证过滤器头文件
 * @details 提供基于JWT Token的身份认证功能
 * @author College BBS Team
 * @date 2025-11-16
 * @version 1.0
 */

#pragma once

#include <drogon/HttpFilter.h>

using namespace drogon;

/**
 * @class AuthFilter
 * @brief JWT认证过滤器类
 * @details 用于保护需要登录才能访问的接口
 *
 * 工作流程：
 * 1. 从请求头中提取Authorization字段
 * 2. 验证Bearer Token格式
 * 3. 使用JwtUtil验证Token的有效性
 * 4. 提取user_id和username并存储到请求attributes中
 * 5. 通过验证则继续执行后续Controller，失败则返回401错误
 *
 * @note 使用方法：
 *       在Controller的路由注册中添加"AuthFilter"：
 *       ADD_METHOD_TO(Controller::method, "/path", Post, "AuthFilter");
 *
 * @note Token格式：
 *       Authorization: Bearer {JWT_TOKEN}
 *
 * @note 验证失败时返回：
 *       {"code": 1005, "msg": "Token无效或过期", "data": null}
 */
class AuthFilter : public HttpFilter<AuthFilter> {
public:
    /**
     * @brief 默认构造函数
     */
    AuthFilter() {}

    /**
     * @brief 过滤器核心处理方法
     * @param req HTTP请求对象
     * @param fcb 过滤失败时的回调（返回错误响应）
     * @param fccb 过滤成功时的回调（继续执行链）
     *
     * @details 验证流程：
     *          1. 检查Authorization请求头是否存在
     *          2. 验证格式：必须以"Bearer "开头（注意空格）
     *          3. 提取JWT Token
     *          4. 调用JwtUtil::verifyToken验证
     *          5. 将user_id和username存储到req->attributes()
     *          6. 调用fccb()继续执行，或调用fcb()返回错误
     *
     * @note 存储的attributes：
     *       - user_id (int): 当前登录用户的ID
     *       - username (string): 当前登录用户的用户名
     *
     * @note Controller中获取用户信息：
     *       int user_id = req->attributes()->get<int>("user_id");
     *       string username = req->attributes()->get<string>("username");
     */
    void doFilter(const HttpRequestPtr& req,
                 FilterCallback&& fcb,
                 FilterChainCallback&& fccb) override;
};

/**
 * @file UserController.h
 * @brief 用户控制器头文件
 * @details 提供用户注册、登录、信息查询等核心功能
 * @author College BBS Team
 * @date 2025-11-16
 * @version 1.0
 */

#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
namespace v1 {

/**
 * @class UserController
 * @brief 用户控制器类
 * @details 处理用户相关的HTTP请求，包括：
 *          - 用户注册（POST /api/user/register）
 *          - 用户登录（POST /api/user/login）
 *          - 获取用户信息（GET /api/user/info，需要JWT认证）
 *
 * @note 所有接口返回统一的JSON格式：
 *       成功: {"code": 0, "msg": "success", "data": {...}}
 *       失败: {"code": xxxx, "msg": "错误信息", "data": null}
 */
class UserController : public drogon::HttpController<UserController> {
public:
    METHOD_LIST_BEGIN
    // 用户注册接口 - 无需认证
    // 请求体: {"username": "...", "password": "...", "email": "..."}
    ADD_METHOD_TO(UserController::register_, "/api/user/register", Post);

    // 用户登录接口 - 无需认证
    // 请求体: {"username": "...", "password": "..."}
    // 返回JWT Token用于后续认证
    ADD_METHOD_TO(UserController::login, "/api/user/login", Post);

    // 获取用户信息接口 - 需要JWT认证
    // 查询参数: user_id (可选，不填则返回当前登录用户信息)
    ADD_METHOD_TO(UserController::getUserInfo, "/api/user/info", Get, "AuthFilter");
    METHOD_LIST_END

    /**
     * @brief 用户注册
     * @param req HTTP请求对象
     * @param callback 异步回调函数
     *
     * @details 注册流程：
     *          1. 验证请求参数（用户名、密码、邮箱格式）
     *          2. 检查用户名是否已存在
     *          3. 使用SHA256+Salt加密密码
     *          4. 插入用户记录到数据库
     *          5. 返回用户ID
     *
     * @note 参数验证规则：
     *       - 用户名：3-20字符，字母数字下划线
     *       - 密码：6-20字符
     *       - 邮箱：符合邮箱格式规范
     */
    void register_(const HttpRequestPtr& req,
                  std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief 用户登录
     * @param req HTTP请求对象
     * @param callback 异步回调函数
     *
     * @details 登录流程：
     *          1. 验证请求参数
     *          2. 查询用户记录
     *          3. 验证密码（SHA256+Salt）
     *          4. 生成JWT Token（有效期24小时）
     *          5. 返回用户信息和Token
     *
     * @return 成功返回：user_id, username, token
     */
    void login(const HttpRequestPtr& req,
              std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief 获取用户信息
     * @param req HTTP请求对象（包含JWT Token）
     * @param callback 异步回调函数
     *
     * @details 查询用户的详细信息，包括：
     *          - 基本信息：user_id, username, email, avatar_url
     *          - 统计信息：post_count, reply_count
     *          - 注册时间：created_at
     *
     * @note 需要在请求头中携带JWT Token：
     *       Authorization: Bearer {token}
     */
    void getUserInfo(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace v1
} // namespace api

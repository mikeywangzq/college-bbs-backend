#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
namespace v1 {

/**
 * 用户控制器
 * 处理用户注册、登录、获取信息等操作
 */
class UserController : public drogon::HttpController<UserController> {
public:
    METHOD_LIST_BEGIN
    // 用户注册 POST /api/user/register
    ADD_METHOD_TO(UserController::register_, "/api/user/register", Post);

    // 用户登录 POST /api/user/login
    ADD_METHOD_TO(UserController::login, "/api/user/login", Post);

    // 获取用户信息 GET /api/user/info (需要认证)
    ADD_METHOD_TO(UserController::getUserInfo, "/api/user/info", Get, "AuthFilter");
    METHOD_LIST_END

    /**
     * 用户注册
     */
    void register_(const HttpRequestPtr& req,
                  std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * 用户登录
     */
    void login(const HttpRequestPtr& req,
              std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * 获取用户信息
     */
    void getUserInfo(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace v1
} // namespace api

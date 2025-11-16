#include "UserController.h"
#include "../utils/ResponseUtil.h"
#include "../utils/JwtUtil.h"
#include "../utils/PasswordUtil.h"
#include <drogon/orm/DbClient.h>
#include <regex>

using namespace api::v1;
using namespace drogon::orm;

void UserController::register_(const HttpRequestPtr& req,
                               std::function<void(const HttpResponsePtr&)>&& callback) {
    // 解析JSON请求体
    auto json = req->getJsonObject();
    if (!json) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "请求体格式错误"));
        return;
    }

    // 获取参数
    std::string username = (*json).get("username", "").asString();
    std::string password = (*json).get("password", "").asString();
    std::string email = (*json).get("email", "").asString();

    // 参数验证
    if (username.empty() || password.empty() || email.empty()) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "用户名、密码和邮箱不能为空"));
        return;
    }

    // 验证用户名格式：只能包含字母、数字、下划线，长度3-50
    std::regex username_regex("^[a-zA-Z0-9_]{3,50}$");
    if (!std::regex_match(username, username_regex)) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "用户名只能包含字母、数字、下划线，长度3-50"));
        return;
    }

    // 验证密码长度
    if (password.length() < 6 || password.length() > 20) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "密码长度必须在6-20之间"));
        return;
    }

    // 验证邮箱格式
    std::regex email_regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!std::regex_match(email, email_regex)) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "邮箱格式不正确"));
        return;
    }

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    // 检查用户名是否已存在
    auto sql_check = "SELECT id FROM users WHERE username = ? LIMIT 1";

    dbClient->execSqlAsync(
        sql_check,
        [callback, username, password, email, dbClient](const Result& r) {
            if (r.size() > 0) {
                // 用户名已存在
                callback(ResponseUtil::error(ResponseUtil::USER_EXISTS, "用户名已存在"));
                return;
            }

            // 密码加密
            std::string password_hash = PasswordUtil::hashPassword(password);

            // 插入用户数据
            auto sql_insert = "INSERT INTO users (username, password_hash, email) VALUES (?, ?, ?)";

            dbClient->execSqlAsync(
                sql_insert,
                [callback](const Result& r) {
                    // 获取插入的用户ID
                    auto insert_id = r.insertId();

                    Json::Value data;
                    data["user_id"] = static_cast<int>(insert_id);

                    callback(ResponseUtil::success(data, "注册成功"));
                },
                [callback](const DrogonDbException& e) {
                    LOG_ERROR << "Database error: " << e.base().what();
                    callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                },
                username, password_hash, email
            );
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        username
    );
}

void UserController::login(const HttpRequestPtr& req,
                           std::function<void(const HttpResponsePtr&)>&& callback) {
    // 解析JSON请求体
    auto json = req->getJsonObject();
    if (!json) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "请求体格式错误"));
        return;
    }

    // 获取参数
    std::string username = (*json).get("username", "").asString();
    std::string password = (*json).get("password", "").asString();

    // 参数验证
    if (username.empty() || password.empty()) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "用户名和密码不能为空"));
        return;
    }

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    // 查询用户
    auto sql = "SELECT id, username, password_hash FROM users WHERE username = ? LIMIT 1";

    dbClient->execSqlAsync(
        sql,
        [callback, password](const Result& r) {
            if (r.size() == 0) {
                // 用户不存在
                callback(ResponseUtil::error(ResponseUtil::USER_NOT_FOUND, "用户不存在"));
                return;
            }

            auto row = r[0];
            int user_id = row["id"].as<int>();
            std::string db_username = row["username"].as<std::string>();
            std::string password_hash = row["password_hash"].as<std::string>();

            // 验证密码
            if (!PasswordUtil::verifyPassword(password, password_hash)) {
                callback(ResponseUtil::error(ResponseUtil::WRONG_PASSWORD, "密码错误"));
                return;
            }

            // 生成JWT Token
            std::string token = JwtUtil::generateToken(user_id, db_username);

            Json::Value data;
            data["user_id"] = user_id;
            data["username"] = db_username;
            data["token"] = token;

            callback(ResponseUtil::success(data, "登录成功"));
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        username
    );
}

void UserController::getUserInfo(const HttpRequestPtr& req,
                                 std::function<void(const HttpResponsePtr&)>&& callback) {
    // 从request attributes中获取用户信息（由AuthFilter设置）
    auto user_id = req->attributes()->get<int>("user_id");

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    // 查询用户信息和统计数据
    auto sql = R"(
        SELECT
            u.id,
            u.username,
            u.email,
            u.avatar_url,
            u.created_at,
            (SELECT COUNT(*) FROM posts WHERE user_id = u.id) as post_count,
            (SELECT COUNT(*) FROM replies WHERE user_id = u.id) as reply_count
        FROM users u
        WHERE u.id = ?
        LIMIT 1
    )";

    dbClient->execSqlAsync(
        sql,
        [callback](const Result& r) {
            if (r.size() == 0) {
                callback(ResponseUtil::error(ResponseUtil::USER_NOT_FOUND, "用户不存在"));
                return;
            }

            auto row = r[0];

            Json::Value data;
            data["user_id"] = row["id"].as<int>();
            data["username"] = row["username"].as<std::string>();
            data["email"] = row["email"].as<std::string>();
            data["avatar_url"] = row["avatar_url"].as<std::string>();
            data["post_count"] = row["post_count"].as<int>();
            data["reply_count"] = row["reply_count"].as<int>();

            // 格式化时间
            auto created_at = row["created_at"].as<std::string>();
            data["created_at"] = created_at;

            callback(ResponseUtil::success(data));
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        user_id
    );
}

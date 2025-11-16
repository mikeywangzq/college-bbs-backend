#include "PostController.h"
#include "../utils/ResponseUtil.h"
#include <drogon/orm/DbClient.h>

using namespace api::v1;
using namespace drogon::orm;

void PostController::create(const HttpRequestPtr& req,
                            std::function<void(const HttpResponsePtr&)>&& callback) {
    // 从request attributes中获取用户ID
    auto user_id = req->attributes()->get<int>("user_id");

    // 解析JSON请求体
    auto json = req->getJsonObject();
    if (!json) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "请求体格式错误"));
        return;
    }

    // 获取参数
    std::string title = (*json).get("title", "").asString();
    std::string content = (*json).get("content", "").asString();

    // 参数验证
    if (title.empty() || content.empty()) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "标题和内容不能为空"));
        return;
    }

    // 验证标题长度（5-100字）
    if (title.length() < 5 || title.length() > 200) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "标题长度必须在5-100字之间"));
        return;
    }

    // 验证内容长度（10-10000字）
    if (content.length() < 10 || content.length() > 10000) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "内容长度必须在10-10000字之间"));
        return;
    }

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    // 插入帖子
    auto sql = "INSERT INTO posts (user_id, title, content) VALUES (?, ?, ?)";

    dbClient->execSqlAsync(
        sql,
        [callback](const Result& r) {
            auto insert_id = r.insertId();

            Json::Value data;
            data["post_id"] = static_cast<int>(insert_id);

            callback(ResponseUtil::success(data, "发帖成功"));
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        user_id, title, content
    );
}

void PostController::getList(const HttpRequestPtr& req,
                             std::function<void(const HttpResponsePtr&)>&& callback) {
    // 获取分页参数
    auto params = req->getParameters();
    int page = 1;
    int size = 20;

    if (params.find("page") != params.end()) {
        try {
            page = std::stoi(params.at("page"));
        } catch (...) {
            page = 1;
        }
    }

    if (params.find("size") != params.end()) {
        try {
            size = std::stoi(params.at("size"));
        } catch (...) {
            size = 20;
        }
    }

    // 限制分页参数
    if (page < 1) page = 1;
    if (size < 1) size = 1;
    if (size > 100) size = 100;

    int offset = (page - 1) * size;

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    // 先查询总数
    auto sql_count = "SELECT COUNT(*) as total FROM posts";

    dbClient->execSqlAsync(
        sql_count,
        [callback, page, size, offset, dbClient](const Result& r) {
            int total = r[0]["total"].as<int>();

            // 查询帖子列表
            auto sql_list = R"(
                SELECT
                    p.id,
                    p.title,
                    p.view_count,
                    p.like_count,
                    p.reply_count,
                    p.created_at,
                    u.id as author_id,
                    u.username as author
                FROM posts p
                JOIN users u ON p.user_id = u.id
                ORDER BY p.created_at DESC
                LIMIT ? OFFSET ?
            )";

            dbClient->execSqlAsync(
                sql_list,
                [callback, total, page, size](const Result& r) {
                    Json::Value posts(Json::arrayValue);

                    for (const auto& row : r) {
                        Json::Value post;
                        post["id"] = row["id"].as<int>();
                        post["title"] = row["title"].as<std::string>();
                        post["author"] = row["author"].as<std::string>();
                        post["author_id"] = row["author_id"].as<int>();
                        post["view_count"] = row["view_count"].as<int>();
                        post["reply_count"] = row["reply_count"].as<int>();
                        post["like_count"] = row["like_count"].as<int>();
                        post["created_at"] = row["created_at"].as<std::string>();

                        posts.append(post);
                    }

                    Json::Value data;
                    data["posts"] = posts;
                    data["total"] = total;
                    data["page"] = page;
                    data["size"] = size;

                    callback(ResponseUtil::success(data));
                },
                [callback](const DrogonDbException& e) {
                    LOG_ERROR << "Database error: " << e.base().what();
                    callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                },
                size, offset
            );
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        }
    );
}

void PostController::getDetail(const HttpRequestPtr& req,
                               std::function<void(const HttpResponsePtr&)>&& callback) {
    // 获取帖子ID
    auto params = req->getParameters();
    if (params.find("id") == params.end()) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "缺少帖子ID"));
        return;
    }

    int post_id;
    try {
        post_id = std::stoi(params.at("id"));
    } catch (...) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "帖子ID格式错误"));
        return;
    }

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    // 浏览次数+1
    auto sql_view = "UPDATE posts SET view_count = view_count + 1 WHERE id = ?";
    dbClient->execSqlAsync(
        sql_view,
        [](const Result& r) {
            // 浏览次数更新成功
        },
        [](const DrogonDbException& e) {
            LOG_ERROR << "Update view count error: " << e.base().what();
        },
        post_id
    );

    // 查询帖子信息
    auto sql_post = R"(
        SELECT
            p.id,
            p.title,
            p.content,
            p.view_count,
            p.like_count,
            p.reply_count,
            p.created_at,
            u.id as author_id,
            u.username as author
        FROM posts p
        JOIN users u ON p.user_id = u.id
        WHERE p.id = ?
        LIMIT 1
    )";

    dbClient->execSqlAsync(
        sql_post,
        [callback, post_id, dbClient](const Result& r) {
            if (r.size() == 0) {
                callback(ResponseUtil::error(ResponseUtil::POST_NOT_FOUND, "帖子不存在"));
                return;
            }

            auto row = r[0];

            Json::Value post;
            post["id"] = row["id"].as<int>();
            post["title"] = row["title"].as<std::string>();
            post["content"] = row["content"].as<std::string>();
            post["author"] = row["author"].as<std::string>();
            post["author_id"] = row["author_id"].as<int>();
            post["view_count"] = row["view_count"].as<int>() + 1; // +1 因为刚才更新了
            post["like_count"] = row["like_count"].as<int>();
            post["reply_count"] = row["reply_count"].as<int>();
            post["created_at"] = row["created_at"].as<std::string>();

            // 查询回复列表
            auto sql_replies = R"(
                SELECT
                    r.id,
                    r.content,
                    r.created_at,
                    u.id as author_id,
                    u.username as author
                FROM replies r
                JOIN users u ON r.user_id = u.id
                WHERE r.post_id = ?
                ORDER BY r.created_at ASC
            )";

            dbClient->execSqlAsync(
                sql_replies,
                [callback, post](const Result& r) {
                    Json::Value replies(Json::arrayValue);

                    for (const auto& row : r) {
                        Json::Value reply;
                        reply["id"] = row["id"].as<int>();
                        reply["content"] = row["content"].as<std::string>();
                        reply["author"] = row["author"].as<std::string>();
                        reply["author_id"] = row["author_id"].as<int>();
                        reply["created_at"] = row["created_at"].as<std::string>();

                        replies.append(reply);
                    }

                    Json::Value data;
                    data["post"] = post;
                    data["replies"] = replies;

                    callback(ResponseUtil::success(data));
                },
                [callback](const DrogonDbException& e) {
                    LOG_ERROR << "Database error: " << e.base().what();
                    callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                },
                post_id
            );
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        post_id
    );
}

void PostController::deletePost(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    // 从request attributes中获取用户ID
    auto user_id = req->attributes()->get<int>("user_id");

    // 解析JSON请求体
    auto json = req->getJsonObject();
    if (!json) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "请求体格式错误"));
        return;
    }

    // 获取帖子ID
    int post_id = (*json).get("post_id", 0).asInt();
    if (post_id <= 0) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "帖子ID无效"));
        return;
    }

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    // 先查询帖子是否存在，以及是否是当前用户创建的
    auto sql_check = "SELECT user_id FROM posts WHERE id = ? LIMIT 1";

    dbClient->execSqlAsync(
        sql_check,
        [callback, user_id, post_id, dbClient](const Result& r) {
            if (r.size() == 0) {
                callback(ResponseUtil::error(ResponseUtil::POST_NOT_FOUND, "帖子不存在"));
                return;
            }

            int post_user_id = r[0]["user_id"].as<int>();

            // 检查权限
            if (post_user_id != user_id) {
                callback(ResponseUtil::error(ResponseUtil::NO_PERMISSION, "无权限操作"));
                return;
            }

            // 删除帖子（级联删除回复和点赞）
            auto sql_delete = "DELETE FROM posts WHERE id = ?";

            dbClient->execSqlAsync(
                sql_delete,
                [callback](const Result& r) {
                    callback(ResponseUtil::success(Json::Value::null, "删除成功"));
                },
                [callback](const DrogonDbException& e) {
                    LOG_ERROR << "Database error: " << e.base().what();
                    callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                },
                post_id
            );
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        post_id
    );
}

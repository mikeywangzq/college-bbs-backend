#include "LikeController.h"
#include "../utils/ResponseUtil.h"
#include <drogon/orm/DbClient.h>

using namespace api::v1;
using namespace drogon::orm;

void LikeController::toggle(const HttpRequestPtr& req,
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

    // 先检查帖子是否存在
    auto sql_check_post = "SELECT id FROM posts WHERE id = ? LIMIT 1";

    dbClient->execSqlAsync(
        sql_check_post,
        [callback, user_id, post_id, dbClient](const Result& r) {
            if (r.size() == 0) {
                callback(ResponseUtil::error(ResponseUtil::POST_NOT_FOUND, "帖子不存在"));
                return;
            }

            // 检查用户是否已经点赞
            auto sql_check_like = "SELECT id FROM post_likes WHERE post_id = ? AND user_id = ? LIMIT 1";

            dbClient->execSqlAsync(
                sql_check_like,
                [callback, user_id, post_id, dbClient](const Result& r) {
                    if (r.size() > 0) {
                        // 已经点赞，执行取消点赞操作
                        auto sql_delete = "DELETE FROM post_likes WHERE post_id = ? AND user_id = ?";

                        dbClient->execSqlAsync(
                            sql_delete,
                            [callback, post_id, dbClient](const Result& r) {
                                // 更新帖子的点赞数 -1
                                auto sql_update = "UPDATE posts SET like_count = like_count - 1 WHERE id = ?";

                                dbClient->execSqlAsync(
                                    sql_update,
                                    [callback, post_id, dbClient](const Result& r) {
                                        // 查询最新的点赞数
                                        auto sql_count = "SELECT like_count FROM posts WHERE id = ?";

                                        dbClient->execSqlAsync(
                                            sql_count,
                                            [callback](const Result& r) {
                                                int like_count = r[0]["like_count"].as<int>();

                                                Json::Value data;
                                                data["liked"] = false;
                                                data["like_count"] = like_count;

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
                                        LOG_ERROR << "Update like count error: " << e.base().what();
                                        callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                                    },
                                    post_id
                                );
                            },
                            [callback](const DrogonDbException& e) {
                                LOG_ERROR << "Database error: " << e.base().what();
                                callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                            },
                            post_id, user_id
                        );
                    } else {
                        // 未点赞，执行点赞操作
                        auto sql_insert = "INSERT INTO post_likes (post_id, user_id) VALUES (?, ?)";

                        dbClient->execSqlAsync(
                            sql_insert,
                            [callback, post_id, dbClient](const Result& r) {
                                // 更新帖子的点赞数 +1
                                auto sql_update = "UPDATE posts SET like_count = like_count + 1 WHERE id = ?";

                                dbClient->execSqlAsync(
                                    sql_update,
                                    [callback, post_id, dbClient](const Result& r) {
                                        // 查询最新的点赞数
                                        auto sql_count = "SELECT like_count FROM posts WHERE id = ?";

                                        dbClient->execSqlAsync(
                                            sql_count,
                                            [callback](const Result& r) {
                                                int like_count = r[0]["like_count"].as<int>();

                                                Json::Value data;
                                                data["liked"] = true;
                                                data["like_count"] = like_count;

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
                                        LOG_ERROR << "Update like count error: " << e.base().what();
                                        callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                                    },
                                    post_id
                                );
                            },
                            [callback](const DrogonDbException& e) {
                                LOG_ERROR << "Database error: " << e.base().what();
                                callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                            },
                            post_id, user_id
                        );
                    }
                },
                [callback](const DrogonDbException& e) {
                    LOG_ERROR << "Database error: " << e.base().what();
                    callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                },
                post_id, user_id
            );
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        post_id
    );
}

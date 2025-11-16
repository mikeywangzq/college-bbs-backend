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
                    bool already_liked = (r.size() > 0);

                    // 使用事务保证数据一致性
                    auto transPtr = dbClient->newTransaction();

                    if (already_liked) {
                        // 已经点赞，执行取消点赞操作
                        auto sql_delete = "DELETE FROM post_likes WHERE post_id = ? AND user_id = ?";

                        transPtr->execSqlAsync(
                            sql_delete,
                            [callback, post_id, transPtr](const Result& r) {
                                // 检查是否删除成功
                                if (r.affectedRows() == 0) {
                                    transPtr->rollback();
                                    callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "取消点赞失败"));
                                    return;
                                }

                                // 更新帖子的点赞数 -1
                                auto sql_update = "UPDATE posts SET like_count = like_count - 1 WHERE id = ? AND like_count > 0";

                                transPtr->execSqlAsync(
                                    sql_update,
                                    [callback, post_id, transPtr](const Result& r) {
                                        // 查询最新的点赞数
                                        auto sql_count = "SELECT like_count FROM posts WHERE id = ?";

                                        transPtr->execSqlAsync(
                                            sql_count,
                                            [callback, transPtr](const Result& r) {
                                                int like_count = r[0]["like_count"].as<int>();

                                                // 提交事务
                                                transPtr->commit([callback, like_count]() {
                                                    Json::Value data;
                                                    data["liked"] = false;
                                                    data["like_count"] = like_count;

                                                    callback(ResponseUtil::success(data));
                                                });
                                            },
                                            [callback, transPtr](const DrogonDbException& e) {
                                                LOG_ERROR << "Query like count error: " << e.base().what();
                                                transPtr->rollback();
                                                callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                                            },
                                            post_id
                                        );
                                    },
                                    [callback, transPtr](const DrogonDbException& e) {
                                        LOG_ERROR << "Update like count error: " << e.base().what();
                                        transPtr->rollback();
                                        callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                                    },
                                    post_id
                                );
                            },
                            [callback, transPtr](const DrogonDbException& e) {
                                LOG_ERROR << "Delete like error: " << e.base().what();
                                transPtr->rollback();
                                callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                            },
                            post_id, user_id
                        );
                    } else {
                        // 未点赞，执行点赞操作
                        // 使用INSERT IGNORE避免并发时UNIQUE约束冲突
                        auto sql_insert = "INSERT IGNORE INTO post_likes (post_id, user_id) VALUES (?, ?)";

                        transPtr->execSqlAsync(
                            sql_insert,
                            [callback, post_id, transPtr](const Result& r) {
                                // 检查是否真正插入了数据
                                if (r.affectedRows() == 0) {
                                    // 数据已存在（并发情况），直接返回成功
                                    transPtr->rollback();

                                    // 查询当前点赞数返回给用户
                                    auto dbClient = drogon::app().getDbClient();
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
                                            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                                        },
                                        post_id
                                    );
                                    return;
                                }

                                // 成功插入，更新帖子的点赞数 +1
                                auto sql_update = "UPDATE posts SET like_count = like_count + 1 WHERE id = ?";

                                transPtr->execSqlAsync(
                                    sql_update,
                                    [callback, post_id, transPtr](const Result& r) {
                                        // 查询最新的点赞数
                                        auto sql_count = "SELECT like_count FROM posts WHERE id = ?";

                                        transPtr->execSqlAsync(
                                            sql_count,
                                            [callback, transPtr](const Result& r) {
                                                int like_count = r[0]["like_count"].as<int>();

                                                // 提交事务
                                                transPtr->commit([callback, like_count]() {
                                                    Json::Value data;
                                                    data["liked"] = true;
                                                    data["like_count"] = like_count;

                                                    callback(ResponseUtil::success(data));
                                                });
                                            },
                                            [callback, transPtr](const DrogonDbException& e) {
                                                LOG_ERROR << "Query like count error: " << e.base().what();
                                                transPtr->rollback();
                                                callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                                            },
                                            post_id
                                        );
                                    },
                                    [callback, transPtr](const DrogonDbException& e) {
                                        LOG_ERROR << "Update like count error: " << e.base().what();
                                        transPtr->rollback();
                                        callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                                    },
                                    post_id
                                );
                            },
                            [callback, transPtr](const DrogonDbException& e) {
                                LOG_ERROR << "Insert like error: " << e.base().what();
                                transPtr->rollback();
                                callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                            },
                            post_id, user_id
                        );
                    }
                },
                [callback](const DrogonDbException& e) {
                    LOG_ERROR << "Check like error: " << e.base().what();
                    callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                },
                post_id, user_id
            );
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Check post error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        post_id
    );
}

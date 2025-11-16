#include "ReplyController.h"
#include "../utils/ResponseUtil.h"
#include <drogon/orm/DbClient.h>

using namespace api::v1;
using namespace drogon::orm;

void ReplyController::create(const HttpRequestPtr& req,
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
    int post_id = (*json).get("post_id", 0).asInt();
    std::string content = (*json).get("content", "").asString();

    // 参数验证
    if (post_id <= 0) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "帖子ID无效"));
        return;
    }

    if (content.empty()) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "回复内容不能为空"));
        return;
    }

    // 验证内容长度（1-1000字）
    if (content.length() < 1 || content.length() > 1000) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "回复内容长度必须在1-1000字之间"));
        return;
    }

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    // 先检查帖子是否存在
    auto sql_check = "SELECT id FROM posts WHERE id = ? LIMIT 1";

    dbClient->execSqlAsync(
        sql_check,
        [callback, user_id, post_id, content, dbClient](const Result& r) {
            if (r.size() == 0) {
                callback(ResponseUtil::error(ResponseUtil::POST_NOT_FOUND, "帖子不存在"));
                return;
            }

            // 使用事务保证数据一致性
            auto transPtr = dbClient->newTransaction();

            // 插入回复
            auto sql_insert = "INSERT INTO replies (post_id, user_id, content) VALUES (?, ?, ?)";

            transPtr->execSqlAsync(
                sql_insert,
                [callback, post_id, transPtr](const Result& r) {
                    auto insert_id = r.insertId();

                    // 更新帖子的回复数 +1
                    auto sql_update = "UPDATE posts SET reply_count = reply_count + 1 WHERE id = ?";

                    transPtr->execSqlAsync(
                        sql_update,
                        [callback, insert_id, transPtr](const Result& r) {
                            // 提交事务
                            transPtr->commit([callback, insert_id]() {
                                Json::Value data;
                                data["reply_id"] = static_cast<int>(insert_id);

                                callback(ResponseUtil::success(data, "回复成功"));
                            });
                        },
                        [callback, transPtr](const DrogonDbException& e) {
                            LOG_ERROR << "Update reply count error: " << e.base().what();
                            // 回滚事务
                            transPtr->rollback();
                            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                        },
                        post_id
                    );
                },
                [callback, transPtr](const DrogonDbException& e) {
                    LOG_ERROR << "Database error: " << e.base().what();
                    // 回滚事务
                    transPtr->rollback();
                    callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                },
                post_id, user_id, content
            );
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        post_id
    );
}

void ReplyController::deleteReply(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback) {
    // 从request attributes中获取用户ID
    auto user_id = req->attributes()->get<int>("user_id");

    // 解析JSON请求体
    auto json = req->getJsonObject();
    if (!json) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "请求体格式错误"));
        return;
    }

    // 获取回复ID
    int reply_id = (*json).get("reply_id", 0).asInt();
    if (reply_id <= 0) {
        callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "回复ID无效"));
        return;
    }

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    // 先查询回复是否存在，以及是否是当前用户创建的
    auto sql_check = "SELECT user_id, post_id FROM replies WHERE id = ? LIMIT 1";

    dbClient->execSqlAsync(
        sql_check,
        [callback, user_id, reply_id, dbClient](const Result& r) {
            if (r.size() == 0) {
                callback(ResponseUtil::error(ResponseUtil::REPLY_NOT_FOUND, "回复不存在"));
                return;
            }

            int reply_user_id = r[0]["user_id"].as<int>();
            int post_id = r[0]["post_id"].as<int>();

            // 检查权限
            if (reply_user_id != user_id) {
                callback(ResponseUtil::error(ResponseUtil::NO_PERMISSION, "无权限操作"));
                return;
            }

            // 使用事务保证数据一致性
            auto transPtr = dbClient->newTransaction();

            // 删除回复
            auto sql_delete = "DELETE FROM replies WHERE id = ?";

            transPtr->execSqlAsync(
                sql_delete,
                [callback, post_id, transPtr](const Result& r) {
                    // 更新帖子的回复数 -1
                    auto sql_update = "UPDATE posts SET reply_count = reply_count - 1 WHERE id = ?";

                    transPtr->execSqlAsync(
                        sql_update,
                        [callback, transPtr](const Result& r) {
                            // 提交事务
                            transPtr->commit([callback]() {
                                callback(ResponseUtil::success(Json::Value::null, "删除成功"));
                            });
                        },
                        [callback, transPtr](const DrogonDbException& e) {
                            LOG_ERROR << "Update reply count error: " << e.base().what();
                            // 回滚事务
                            transPtr->rollback();
                            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                        },
                        post_id
                    );
                },
                [callback, transPtr](const DrogonDbException& e) {
                    LOG_ERROR << "Database error: " << e.base().what();
                    // 回滚事务
                    transPtr->rollback();
                    callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
                },
                reply_id
            );
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误"));
        },
        reply_id
    );
}

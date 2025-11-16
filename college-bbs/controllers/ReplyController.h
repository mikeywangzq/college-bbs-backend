/**
 * @file ReplyController.h
 * @brief 回复控制器头文件
 * @details 提供帖子回复的创建和删除功能
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
 * @class ReplyController
 * @brief 回复控制器类
 * @details 处理回复相关的HTTP请求，包括：
 *          - 发布回复（POST /api/reply/create，需要认证）
 *          - 删除回复（DELETE /api/reply/delete，需要认证且仅限本人）
 *
 * @note 数据一致性保证：
 *       - 使用数据库事务保证reply插入和reply_count更新的原子性
 *       - 删除回复时同步更新帖子的reply_count
 */
class ReplyController : public drogon::HttpController<ReplyController> {
public:
    METHOD_LIST_BEGIN
    // 创建回复接口 - 需要JWT认证
    // 请求体: {"post_id": 帖子ID, "content": "回复内容"}
    ADD_METHOD_TO(ReplyController::create, "/api/reply/create", Post, "AuthFilter");

    // 删除回复接口 - 需要JWT认证且仅限回复作者
    // 请求体: {"reply_id": 回复ID}
    ADD_METHOD_TO(ReplyController::deleteReply, "/api/reply/delete", Delete, "AuthFilter");
    METHOD_LIST_END

    /**
     * @brief 创建回复
     * @param req HTTP请求对象
     * @param callback 异步回调函数
     *
     * @details 回复流程：
     *          1. 验证post_id和content参数
     *          2. 检查帖子是否存在
     *          3. 使用事务执行：
     *             a. INSERT回复记录
     *             b. UPDATE帖子的reply_count +1
     *          4. 提交事务或回滚（任一操作失败）
     *
     * @note 参数验证规则：
     *       - post_id: 必须 > 0且帖子存在
     *       - content: 1-1000字符
     *
     * @note 事务保证：
     *       确保reply插入和reply_count更新要么全成功要么全失败
     *       避免数据不一致问题
     */
    void create(const HttpRequestPtr& req,
               std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief 删除回复
     * @param req HTTP请求对象（包含JWT Token）
     * @param callback 异步回调函数
     *
     * @details 删除流程：
     *          1. 验证reply_id参数
     *          2. 查询回复是否存在并获取user_id和post_id
     *          3. 检查权限（仅回复作者可删除）
     *          4. 使用事务执行：
     *             a. DELETE回复记录
     *             b. UPDATE帖子的reply_count -1
     *          5. 提交事务或回滚
     *
     * @note 权限控制：
     *       只有回复作者可以删除自己的回复
     *       其他用户会收到"无权限操作"错误
     *
     * @note 事务保证：
     *       确保reply删除和reply_count更新的原子性
     */
    void deleteReply(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace v1
} // namespace api

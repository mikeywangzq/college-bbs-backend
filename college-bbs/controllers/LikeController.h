/**
 * @file LikeController.h
 * @brief 点赞控制器头文件
 * @details 提供帖子的点赞/取消点赞切换功能
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
 * @class LikeController
 * @brief 点赞控制器类
 * @details 处理点赞相关的HTTP请求：
 *          - 点赞/取消点赞切换（POST /api/like/toggle，需要认证）
 *
 * @note 并发安全保证：
 *       - 使用INSERT IGNORE处理并发点赞请求
 *       - 使用数据库事务保证like插入和like_count更新的原子性
 *       - 通过affectedRows()判断是否真正插入/删除
 */
class LikeController : public drogon::HttpController<LikeController> {
public:
    METHOD_LIST_BEGIN
    // 点赞/取消点赞切换接口 - 需要JWT认证
    // 请求体: {"post_id": 帖子ID}
    // 行为: 如果未点赞则点赞，如果已点赞则取消
    ADD_METHOD_TO(LikeController::toggle, "/api/like/toggle", Post, "AuthFilter");
    METHOD_LIST_END

    /**
     * @brief 点赞/取消点赞切换
     * @param req HTTP请求对象
     * @param callback 异步回调函数
     *
     * @details 切换流程：
     *          1. 验证post_id参数
     *          2. 检查帖子是否存在
     *          3. 查询用户是否已点赞该帖子
     *          4. 使用事务执行：
     *             a. 如果已点赞：DELETE点赞记录 + UPDATE like_count -1
     *             b. 如果未点赞：INSERT IGNORE点赞记录 + UPDATE like_count +1
     *          5. 提交事务或回滚
     *          6. 返回最新的点赞状态和like_count
     *
     * @return 返回格式：
     *         {
     *           "liked": true/false,  // 当前用户是否已点赞
     *           "like_count": 数字    // 帖子的点赞总数
     *         }
     *
     * @note 并发处理：
     *       - 使用INSERT IGNORE避免并发点赞产生UNIQUE约束错误
     *       - 通过affectedRows()判断INSERT是否真正执行
     *       - 如果并发导致已存在，则回滚事务并返回当前状态
     *
     * @note 事务保证：
     *       确保点赞记录的增删和like_count的更新保持一致
     */
    void toggle(const HttpRequestPtr& req,
               std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace v1
} // namespace api

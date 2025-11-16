/**
 * @file PostController.h
 * @brief 帖子控制器头文件
 * @details 提供帖子的CRUD（创建、读取、删除）功能
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
 * @class PostController
 * @brief 帖子控制器类
 * @details 处理帖子相关的HTTP请求，包括：
 *          - 发布帖子（POST /api/post/create，需要认证）
 *          - 获取帖子列表（GET /api/post/list，支持分页）
 *          - 获取帖子详情（GET /api/post/detail，自动增加浏览次数）
 *          - 删除帖子（DELETE /api/post/delete，需要认证且仅限本人）
 *
 * @note 特殊处理：
 *       - getDetail会自动增加view_count（使用事务避免竞态条件）
 *       - getList支持分页（page, size参数）
 *       - 删除帖子会级联删除相关回复和点赞
 */
class PostController : public drogon::HttpController<PostController> {
public:
    METHOD_LIST_BEGIN
    // 创建帖子接口 - 需要JWT认证
    // 请求体: {"title": "标题", "content": "内容"}
    ADD_METHOD_TO(PostController::create, "/api/post/create", Post, "AuthFilter");

    // 获取帖子列表接口 - 无需认证
    // 查询参数: page (页码，默认1), size (每页数量，默认20，最大100)
    ADD_METHOD_TO(PostController::getList, "/api/post/list", Get);

    // 获取帖子详情接口 - 无需认证
    // 查询参数: id (帖子ID)
    // 注意: 每次访问会自动将view_count +1
    ADD_METHOD_TO(PostController::getDetail, "/api/post/detail", Get);

    // 删除帖子接口 - 需要JWT认证且仅限帖子作者
    // 请求体: {"post_id": 帖子ID}
    ADD_METHOD_TO(PostController::deletePost, "/api/post/delete", Delete, "AuthFilter");
    METHOD_LIST_END

    /**
     * @brief 创建帖子
     * @param req HTTP请求对象
     * @param callback 异步回调函数
     *
     * @details 发布流程：
     *          1. 验证标题和内容（长度、格式）
     *          2. 获取当前登录用户ID（从JWT Token）
     *          3. 插入帖子记录到数据库
     *          4. 返回帖子ID
     *
     * @note 参数验证规则：
     *       - title: 1-100字符
     *       - content: 5-200字符
     */
    void create(const HttpRequestPtr& req,
               std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief 获取帖子列表（分页）
     * @param req HTTP请求对象
     * @param callback 异步回调函数
     *
     * @details 查询流程：
     *          1. 解析分页参数（page, size）
     *          2. 查询总帖子数
     *          3. 查询当前页帖子（JOIN users表获取作者信息）
     *          4. 返回分页数据和帖子列表
     *
     * @return 返回格式：
     *         {
     *           "total": 总数,
     *           "page": 当前页,
     *           "size": 每页数量,
     *           "list": [帖子数组]
     *         }
     */
    void getList(const HttpRequestPtr& req,
                std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief 获取帖子详情
     * @param req HTTP请求对象
     * @param callback 异步回调函数
     *
     * @details 查询流程：
     *          1. 验证post_id参数
     *          2. 使用事务先UPDATE view_count +1
     *          3. 再SELECT完整帖子信息（避免竞态条件）
     *          4. 查询所有回复（JOIN users表）
     *          5. 返回帖子+回复列表
     *
     * @note 并发安全：
     *       使用顺序执行（UPDATE完成后才SELECT）
     *       确保返回的view_count包含本次增量
     */
    void getDetail(const HttpRequestPtr& req,
                  std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief 删除帖子
     * @param req HTTP请求对象（包含JWT Token）
     * @param callback 异步回调函数
     *
     * @details 删除流程：
     *          1. 验证post_id参数
     *          2. 查询帖子是否存在
     *          3. 检查权限（仅作者可删除）
     *          4. 删除帖子（数据库会级联删除回复和点赞）
     *
     * @note 权限控制：
     *       只有帖子作者可以删除自己的帖子
     *       其他用户会收到"无权限操作"错误
     */
    void deletePost(const HttpRequestPtr& req,
                   std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace v1
} // namespace api

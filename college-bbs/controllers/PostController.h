#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

namespace api {
namespace v1 {

/**
 * 帖子控制器
 * 处理帖子的创建、查看、删除等操作
 */
class PostController : public drogon::HttpController<PostController> {
public:
    METHOD_LIST_BEGIN
    // 创建帖子 POST /api/post/create (需要认证)
    ADD_METHOD_TO(PostController::create, "/api/post/create", Post, "AuthFilter");

    // 获取帖子列表 GET /api/post/list
    ADD_METHOD_TO(PostController::getList, "/api/post/list", Get);

    // 获取帖子详情 GET /api/post/detail
    ADD_METHOD_TO(PostController::getDetail, "/api/post/detail", Get);

    // 删除帖子 DELETE /api/post/delete (需要认证)
    ADD_METHOD_TO(PostController::deletePost, "/api/post/delete", Delete, "AuthFilter");
    METHOD_LIST_END

    /**
     * 创建帖子
     */
    void create(const HttpRequestPtr& req,
               std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * 获取帖子列表
     */
    void getList(const HttpRequestPtr& req,
                std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * 获取帖子详情
     */
    void getDetail(const HttpRequestPtr& req,
                  std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * 删除帖子
     */
    void deletePost(const HttpRequestPtr& req,
                   std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace v1
} // namespace api

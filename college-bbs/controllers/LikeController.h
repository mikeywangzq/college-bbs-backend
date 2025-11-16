#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

namespace api {
namespace v1 {

/**
 * 点赞控制器
 * 处理帖子的点赞和取消点赞
 */
class LikeController : public drogon::HttpController<LikeController> {
public:
    METHOD_LIST_BEGIN
    // 点赞/取消点赞 POST /api/like/toggle (需要认证)
    ADD_METHOD_TO(LikeController::toggle, "/api/like/toggle", Post, "AuthFilter");
    METHOD_LIST_END

    /**
     * 点赞/取消点赞（toggle）
     */
    void toggle(const HttpRequestPtr& req,
               std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace v1
} // namespace api

#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
namespace v1 {

/**
 * 回复控制器
 * 处理帖子回复的创建和删除
 */
class ReplyController : public drogon::HttpController<ReplyController> {
public:
    METHOD_LIST_BEGIN
    // 创建回复 POST /api/reply/create (需要认证)
    ADD_METHOD_TO(ReplyController::create, "/api/reply/create", Post, "AuthFilter");

    // 删除回复 DELETE /api/reply/delete (需要认证)
    ADD_METHOD_TO(ReplyController::deleteReply, "/api/reply/delete", Delete, "AuthFilter");
    METHOD_LIST_END

    /**
     * 创建回复
     */
    void create(const HttpRequestPtr& req,
               std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * 删除回复
     */
    void deleteReply(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace v1
} // namespace api

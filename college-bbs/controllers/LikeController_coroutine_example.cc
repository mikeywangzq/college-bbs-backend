/**
 * LikeController - C++20协程重构示例
 *
 * 这个文件展示了如何使用C++20协程重构原有的回调式代码
 *
 * 优点：
 * 1. 代码可读性大幅提升（从嵌套回调变为顺序执行）
 * 2. 错误处理更加集中和清晰
 * 3. 更符合同步代码的思维模式
 * 4. 减少了Lambda捕获的复杂性
 *
 * 注意：
 * - 需要C++20支持
 * - 需要在CMakeLists.txt中设置 set(CMAKE_CXX_STANDARD 20)
 * - 此文件仅供参考，不会被编译
 */

#include "LikeController.h"
#include "../utils/ResponseUtil.h"
#include "../utils/ErrorLogger.h"
#include <drogon/orm/DbClient.h>

using namespace api::v1;
using namespace drogon::orm;

/**
 * 协程版本的点赞切换
 *
 * 相比原版本（200+行嵌套回调），协程版本仅需约60行
 * 代码结构清晰，易于理解和维护
 */
drogon::Task<HttpResponsePtr> LikeController::toggleCoroutine(HttpRequestPtr req) {
    // 从request attributes中获取用户ID
    auto user_id = req->attributes()->get<int>("user_id");

    // 解析JSON请求体
    auto json = req->getJsonObject();
    if (!json) {
        co_return ResponseUtil::error(ResponseUtil::PARAM_ERROR, "请求体格式错误");
    }

    // 获取帖子ID
    int post_id = (*json).get("post_id", 0).asInt();
    if (post_id <= 0) {
        co_return ResponseUtil::error(ResponseUtil::PARAM_ERROR, "帖子ID无效");
    }

    // 获取数据库客户端
    auto dbClient = drogon::app().getDbClient();

    try {
        // 1. 检查帖子是否存在
        auto r1 = co_await dbClient->execSqlCoro(
            "SELECT id FROM posts WHERE id = ? LIMIT 1",
            post_id
        );

        if (r1.size() == 0) {
            co_return ResponseUtil::error(ResponseUtil::POST_NOT_FOUND, "帖子不存在");
        }

        // 2. 检查用户是否已经点赞
        auto r2 = co_await dbClient->execSqlCoro(
            "SELECT id FROM post_likes WHERE post_id = ? AND user_id = ? LIMIT 1",
            post_id,
            user_id
        );

        bool already_liked = (r2.size() > 0);

        // 3. 开始事务
        auto transPtr = dbClient->newTransaction();

        if (already_liked) {
            // === 取消点赞 ===

            // 3.1 删除点赞记录
            auto r3 = co_await transPtr->execSqlCoro(
                "DELETE FROM post_likes WHERE post_id = ? AND user_id = ?",
                post_id,
                user_id
            );

            if (r3.affectedRows() == 0) {
                transPtr->rollback();
                co_return ResponseUtil::error(ResponseUtil::SERVER_ERROR, "取消点赞失败（记录不存在）");
            }

            // 3.2 更新点赞数 -1
            co_await transPtr->execSqlCoro(
                "UPDATE posts SET like_count = like_count - 1 WHERE id = ?",
                post_id
            );

        } else {
            // === 添加点赞 ===

            // 3.1 插入点赞记录（使用INSERT IGNORE避免并发冲突）
            auto r3 = co_await transPtr->execSqlCoro(
                "INSERT IGNORE INTO post_likes (post_id, user_id) VALUES (?, ?)",
                post_id,
                user_id
            );

            if (r3.affectedRows() == 0) {
                // 并发情况下已经被插入，回滚事务
                transPtr->rollback();

                // 查询当前状态返回给用户
                auto r4 = co_await dbClient->execSqlCoro(
                    "SELECT like_count FROM posts WHERE id = ?",
                    post_id
                );

                Json::Value data;
                data["liked"] = true;
                data["like_count"] = r4[0]["like_count"].as<int>();

                co_return ResponseUtil::success(data, "点赞成功");
            }

            // 3.2 更新点赞数 +1
            co_await transPtr->execSqlCoro(
                "UPDATE posts SET like_count = like_count + 1 WHERE id = ?",
                post_id
            );
        }

        // 4. 提交事务
        co_await transPtr->commitCoro();

        // 5. 查询最新的点赞数
        auto r5 = co_await dbClient->execSqlCoro(
            "SELECT like_count FROM posts WHERE id = ?",
            post_id
        );

        int like_count = r5[0]["like_count"].as<int>();

        // 6. 返回结果
        Json::Value data;
        data["liked"] = !already_liked;
        data["like_count"] = like_count;

        co_return ResponseUtil::success(data, already_liked ? "取消点赞成功" : "点赞成功");

    } catch (const DrogonDbException& e) {
        // 统一的错误处理
        auto errorId = ErrorLogger::generateErrorId();
        ErrorLogger::logDatabaseError(errorId, "toggle like (coroutine)", e);
        co_return ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误", errorId);
    }
}

/**
 * 协程版本的路由注册
 *
 * 注意：协程方法的签名是 Task<HttpResponsePtr>(HttpRequestPtr)
 * 而不是传统的 void(HttpRequestPtr, callback)
 */
/*
void LikeController::registerRoutes() {
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(LikeController::toggleCoroutine, "/api/like/toggle", Post, "AuthFilter");
    METHOD_LIST_END
}
*/

/**
 * ============================================================================
 * 对比：回调版本 vs 协程版本
 * ============================================================================
 *
 * 回调版本（原LikeController.cc）:
 * - 6层嵌套回调
 * - 每层都需要捕获变量
 * - 错误处理分散在各处
 * - 代码跳跃式阅读（从外到内）
 * - 约200行代码
 *
 * 协程版本（本文件）:
 * - 顺序执行，零嵌套
 * - 变量作用域清晰
 * - 统一的try-catch错误处理
 * - 自上而下阅读
 * - 约120行代码（含注释）
 *
 * ============================================================================
 * 性能对比
 * ============================================================================
 *
 * 性能方面两者几乎相同：
 * - 协程底层也是状态机，编译后与回调类似
 * - Drogon的协程实现是零成本抽象
 * - 内存占用：协程略高（需要保存协程帧），但可忽略不计
 *
 * ============================================================================
 * 何时使用协程
 * ============================================================================
 *
 * 适合使用协程的场景：
 * ✅ 多步骤异步操作（3+层嵌套）
 * ✅ 需要复杂的错误处理逻辑
 * ✅ 业务逻辑复杂，需要频繁修改
 * ✅ 新项目或新模块
 *
 * 可以继续使用回调的场景：
 * ⭕ 单层或双层回调（简单场景）
 * ⭕ 性能极度敏感的热点代码（虽然差异很小）
 * ⭕ 需要兼容C++17环境
 *
 * ============================================================================
 * 迁移建议
 * ============================================================================
 *
 * 1. 渐进式迁移：先迁移最复杂的Controller
 * 2. 保持接口兼容：协程版本可以与回调版本共存
 * 3. 充分测试：确保异常处理覆盖所有边界情况
 * 4. 团队培训：确保团队成员理解C++20协程语法
 *
 * ============================================================================
 * C++20协程基础语法速查
 * ============================================================================
 *
 * 1. co_await - 等待异步操作完成
 *    auto result = co_await asyncOperation();
 *
 * 2. co_return - 返回结果（协程函数必须用co_return而非return）
 *    co_return ResponseUtil::success(data);
 *
 * 3. Task<T> - Drogon的协程返回类型
 *    Task<HttpResponsePtr> myHandler(HttpRequestPtr req)
 *
 * 4. try-catch in coroutine - 异常处理
 *    try {
 *        auto r = co_await dbClient->execSqlCoro(...);
 *    } catch (const DrogonDbException& e) {
 *        // 处理错误
 *    }
 *
 * ============================================================================
 */

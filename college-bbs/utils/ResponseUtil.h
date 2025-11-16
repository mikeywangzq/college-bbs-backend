/**
 * @file ResponseUtil.h
 * @brief 统一响应工具类头文件
 * @details 提供标准化的HTTP JSON响应格式
 * @author College BBS Team
 * @date 2025-11-16
 * @version 1.0
 */

#pragma once

#include <drogon/HttpResponse.h>
#include <json/json.h>

using namespace drogon;

/**
 * @class ResponseUtil
 * @brief 统一响应工具类
 * @details 封装HTTP响应的生成逻辑，确保所有API返回统一格式的JSON
 *
 * 统一响应格式：
 * {
 *   "code": 错误码（0表示成功，非0表示失败）,
 *   "msg": 消息描述,
 *   "data": 业务数据（可选）,
 *   "error_id": 错误ID（仅错误响应，可选）
 * }
 *
 * 使用示例：
 * - 成功: callback(ResponseUtil::success(data, "操作成功"));
 * - 失败: callback(ResponseUtil::error(1001, "参数错误"));
 * - 带错误ID: callback(ResponseUtil::error(1009, "数据库错误", errorId));
 *
 * @note 为什么需要统一响应：
 *       1. 前端可以统一解析响应格式
 *       2. 便于错误处理和日志记录
 *       3. 提供更好的API一致性
 */
class ResponseUtil {
public:
    /**
     * @enum ErrorCode
     * @brief 错误码枚举
     * @details 定义所有可能的业务错误码
     *
     * 错误码分类：
     * - 0: 成功
     * - 1001-1099: 通用错误（参数、权限等）
     * - 2001-2099: 用户相关错误（保留）
     * - 3001-3099: 帖子相关错误（保留）
     */
    enum ErrorCode {
        SUCCESS = 0,            /**< 操作成功 */
        PARAM_ERROR = 1001,     /**< 参数错误（格式、长度、类型等） */
        USER_EXISTS = 1002,     /**< 用户名已存在（注册时） */
        USER_NOT_FOUND = 1003,  /**< 用户不存在（登录、查询时） */
        WRONG_PASSWORD = 1004,  /**< 密码错误（登录时） */
        TOKEN_INVALID = 1005,   /**< Token无效或过期（需要重新登录） */
        NO_PERMISSION = 1006,   /**< 无权限操作（如删除他人的帖子/回复） */
        POST_NOT_FOUND = 1007,  /**< 帖子不存在 */
        REPLY_NOT_FOUND = 1008, /**< 回复不存在 */
        DB_ERROR = 1009,        /**< 数据库错误（通用） */
        SERVER_ERROR = 1010     /**< 服务器内部错误（未预期的异常） */
    };

    /**
     * @brief 生成成功响应
     * @param data 响应数据（可选，默认为null）
     * @param msg 成功消息（可选，默认为"success"）
     * @return HttpResponsePtr 包含JSON响应的HTTP对象
     *
     * @details 响应格式：
     *          {
     *            "code": 0,
     *            "msg": "success",
     *            "data": {...}  // 实际业务数据
     *          }
     *
     * @example
     *       // 返回用户信息
     *       Json::Value userData;
     *       userData["user_id"] = 1;
     *       userData["username"] = "user001";
     *       callback(ResponseUtil::success(userData, "查询成功"));
     *
     * @example
     *       // 仅返回成功消息
     *       callback(ResponseUtil::success(Json::Value::null, "删除成功"));
     */
    static HttpResponsePtr success(const Json::Value& data = Json::Value::null,
                                   const std::string& msg = "success");

    /**
     * @brief 生成失败响应
     * @param code 错误码（使用ErrorCode枚举）
     * @param msg 错误消息（如为空则使用默认消息）
     * @return HttpResponsePtr 包含JSON响应的HTTP对象
     *
     * @details 响应格式：
     *          {
     *            "code": 1001,
     *            "msg": "参数错误",
     *            "data": null
     *          }
     *
     * @example
     *       // 使用预定义错误码
     *       callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR, "用户名格式不正确"));
     *
     * @example
     *       // 使用默认错误消息
     *       callback(ResponseUtil::error(ResponseUtil::DB_ERROR, ""));
     */
    static HttpResponsePtr error(int code, const std::string& msg);

    /**
     * @brief 生成带错误ID的失败响应
     * @param code 错误码
     * @param msg 错误消息
     * @param errorId 错误ID（用于追踪和日志关联）
     * @return HttpResponsePtr 包含JSON响应的HTTP对象
     *
     * @details 响应格式：
     *          {
     *            "code": 1009,
     *            "msg": "数据库错误",
     *            "error_id": "ERR-1700000000-A3F2",  // 唯一错误ID
     *            "data": null
     *          }
     *
     * @note 错误ID的作用：
     *       1. 用户可以提供error_id给客服
     *       2. 技术人员通过error_id在日志中定位问题
     *       3. 隐藏敏感的错误细节，提升安全性
     *
     * @example
     *       auto errorId = ErrorLogger::generateErrorId();
     *       ErrorLogger::logDatabaseError(errorId, "insert user", e);
     *       callback(ResponseUtil::error(ResponseUtil::DB_ERROR, "数据库错误", errorId));
     */
    static HttpResponsePtr error(int code, const std::string& msg, const std::string& errorId);

    /**
     * @brief 获取错误码对应的默认错误消息
     * @param code 错误码
     * @return 错误消息字符串
     *
     * @details 提供每个错误码的中文说明
     *          如果错误码未定义，返回"未知错误"
     *
     * @example
     *       string msg = ResponseUtil::getErrorMessage(ResponseUtil::USER_NOT_FOUND);
     *       // 返回: "用户不存在"
     */
    static std::string getErrorMessage(int code);
};

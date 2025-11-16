#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>

/**
 * 错误日志工具类
 *
 * 用于安全地记录错误信息，避免泄露敏感数据
 *
 * 设计原则：
 * 1. 生成唯一的错误ID，方便追踪和排查
 * 2. 详细的错误信息只记录到日志文件（ERROR级别）
 * 3. 返回给用户的只包含通用错误消息和错误ID
 * 4. 在开发环境可以选择包含详细信息，生产环境则隐藏
 */
class ErrorLogger {
public:
    /**
     * 生成唯一的错误ID
     * 格式: ERR-{timestamp}-{random}
     * 示例: ERR-1700000000-A3F2
     */
    static std::string generateErrorId();

    /**
     * 记录数据库错误
     *
     * @param errorId 错误ID
     * @param operation 操作描述（如"create post", "delete reply"）
     * @param exception 数据库异常对象
     * @param includeDetails 是否在日志中包含详细信息（默认false，生产环境应为false）
     */
    static void logDatabaseError(
        const std::string& errorId,
        const std::string& operation,
        const drogon::orm::DrogonDbException& exception,
        bool includeDetails = false
    );

    /**
     * 记录一般错误
     *
     * @param errorId 错误ID
     * @param operation 操作描述
     * @param errorMessage 错误消息
     * @param includeDetails 是否在日志中包含详细信息
     */
    static void logError(
        const std::string& errorId,
        const std::string& operation,
        const std::string& errorMessage,
        bool includeDetails = false
    );

    /**
     * 检查是否应该包含详细错误信息
     * 根据环境变量 LOG_LEVEL 判断
     * DEBUG/TRACE -> true
     * INFO/WARN/ERROR -> false
     */
    static bool shouldIncludeDetails();

private:
    static std::mt19937 rng_;
    static std::uniform_int_distribution<int> dist_;

    /**
     * 生成4位随机十六进制字符串
     */
    static std::string generateRandomHex();
};

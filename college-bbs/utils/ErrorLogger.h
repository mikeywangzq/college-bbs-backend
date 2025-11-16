/**
 * @file ErrorLogger.h
 * @brief 安全错误日志工具类头文件
 * @details 提供安全的错误日志记录功能，防止敏感信息泄露
 * @author College BBS Team
 * @date 2025-11-16
 * @version 1.0
 */

#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>

/**
 * @class ErrorLogger
 * @brief 安全错误日志工具类
 * @details 用于安全地记录错误信息，避免泄露敏感数据（如SQL语句、数据库结构）
 *
 * 设计原则：
 * 1. 生成唯一的错误ID，方便追踪和排查
 * 2. 详细的错误信息只记录到日志文件（ERROR级别）
 * 3. 返回给用户的只包含通用错误消息和错误ID
 * 4. 开发环境显示详细信息，生产环境自动隐藏
 *
 * 错误ID格式：
 * ERR-{Unix时间戳}-{4位十六进制随机数}
 * 示例: ERR-1700000000-A3F2
 *
 * 使用流程：
 * 1. 生成错误ID: auto errorId = ErrorLogger::generateErrorId();
 * 2. 记录日志: ErrorLogger::logDatabaseError(errorId, "operation", exception);
 * 3. 返回给用户: ResponseUtil::error(code, "数据库错误", errorId);
 *
 * @note 日志级别控制：
 *       - DEBUG/TRACE: 显示详细错误信息（开发环境）
 *       - INFO/WARN/ERROR: 隐藏敏感信息（生产环境）
 *       在config.json中配置：{"log_level": "INFO"}
 */
class ErrorLogger {
public:
    /**
     * @brief 生成唯一的错误ID
     * @return 错误ID字符串
     *
     * @details 格式: ERR-{timestamp}-{random}
     *          - timestamp: Unix时间戳（秒）
     *          - random: 4位十六进制随机数（0000-FFFF）
     *
     * @example
     *       auto errorId = ErrorLogger::generateErrorId();
     *       // 返回: "ERR-1700000000-A3F2"
     *
     * @note 唯一性保证：
     *       - 同一秒内最多65536个不同ID
     *       - 碰撞概率：1/65536（极低）
     *       - 如需更强保证，可扩展随机数位数
     */
    static std::string generateErrorId();

    /**
     * @brief 记录数据库错误
     * @param errorId 错误ID（由generateErrorId()生成）
     * @param operation 操作描述（如"create post", "delete reply"）
     * @param exception 数据库异常对象
     * @param includeDetails 是否强制包含详细信息（默认false，自动根据日志级别判断）
     *
     * @details 记录行为：
     *          开发环境（DEBUG/TRACE）：
     *          [ERR-xxx] Database error during {operation} - Details: {exception.what()}
     *
     *          生产环境（INFO/WARN/ERROR）：
     *          [ERR-xxx] Database error during {operation} (Use error ID for tracking)
     *
     * @example
     *       try {
     *           dbClient->execSqlAsync(...);
     *       } catch (const DrogonDbException& e) {
     *           auto errorId = ErrorLogger::generateErrorId();
     *           ErrorLogger::logDatabaseError(errorId, "insert user", e);
     *           callback(ResponseUtil::error(DB_ERROR, "数据库错误", errorId));
     *       }
     *
     * @note 安全性：
     *       exception.what()可能包含SQL语句、表名、字段名等敏感信息
     *       生产环境自动隐藏这些信息
     */
    static void logDatabaseError(
        const std::string& errorId,
        const std::string& operation,
        const drogon::orm::DrogonDbException& exception,
        bool includeDetails = false
    );

    /**
     * @brief 记录一般错误
     * @param errorId 错误ID
     * @param operation 操作描述
     * @param errorMessage 错误消息
     * @param includeDetails 是否强制包含详细信息（默认false）
     *
     * @details 用于记录非数据库相关的错误
     *
     * @example
     *       auto errorId = ErrorLogger::generateErrorId();
     *       ErrorLogger::logError(errorId, "parse JSON", "Invalid JSON format");
     *       callback(ResponseUtil::error(PARAM_ERROR, "请求格式错误", errorId));
     */
    static void logError(
        const std::string& errorId,
        const std::string& operation,
        const std::string& errorMessage,
        bool includeDetails = false
    );

    /**
     * @brief 检查是否应该包含详细错误信息
     * @return true=包含详细信息，false=隐藏详细信息
     *
     * @details 根据Drogon日志级别判断：
     *          - kDebug (4) 或 kTrace (5): 返回true（显示详情）
     *          - kInfo (3)、kWarn (2)、kError (1)、kFatal (0): 返回false（隐藏详情）
     *
     * @note 配置方式：
     *       在config.json中设置：
     *       开发环境: {"log_level": "DEBUG"}
     *       生产环境: {"log_level": "INFO"}
     */
    static bool shouldIncludeDetails();

private:
    /**
     * @brief 随机数生成器（Mersenne Twister）
     * @note 使用steady_clock初始化种子
     */
    static std::mt19937 rng_;

    /**
     * @brief 均匀分布（0-65535）
     * @note 用于生成4位十六进制随机数
     */
    static std::uniform_int_distribution<int> dist_;

    /**
     * @brief 生成4位随机十六进制字符串
     * @return 4位大写十六进制字符串（0000-FFFF）
     *
     * @details 使用mt19937生成0-65535的随机数
     *          转换为4位十六进制（补零）
     */
    static std::string generateRandomHex();
};

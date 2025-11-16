#include "ErrorLogger.h"

// 初始化静态成员
std::mt19937 ErrorLogger::rng_(std::chrono::steady_clock::now().time_since_epoch().count());
std::uniform_int_distribution<int> ErrorLogger::dist_(0, 65535);

std::string ErrorLogger::generateErrorId() {
    // 获取当前时间戳（秒）
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();

    // 生成随机十六进制后缀
    std::string randomHex = generateRandomHex();

    // 组合成错误ID: ERR-{timestamp}-{random}
    std::ostringstream oss;
    oss << "ERR-" << timestamp << "-" << randomHex;

    return oss.str();
}

std::string ErrorLogger::generateRandomHex() {
    int randomNum = dist_(rng_);
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << randomNum;
    return oss.str();
}

void ErrorLogger::logDatabaseError(
    const std::string& errorId,
    const std::string& operation,
    const drogon::orm::DrogonDbException& exception,
    bool includeDetails
) {
    if (includeDetails || shouldIncludeDetails()) {
        // 开发环境：记录详细信息
        LOG_ERROR << "[" << errorId << "] Database error during " << operation
                  << " - Details: " << exception.base().what();
    } else {
        // 生产环境：只记录操作和错误ID，不记录详细信息
        LOG_ERROR << "[" << errorId << "] Database error during " << operation
                  << " (Use error ID for tracking)";
    }
}

void ErrorLogger::logError(
    const std::string& errorId,
    const std::string& operation,
    const std::string& errorMessage,
    bool includeDetails
) {
    if (includeDetails || shouldIncludeDetails()) {
        // 开发环境：记录详细信息
        LOG_ERROR << "[" << errorId << "] Error during " << operation
                  << " - Details: " << errorMessage;
    } else {
        // 生产环境：只记录操作和错误ID
        LOG_ERROR << "[" << errorId << "] Error during " << operation
                  << " (Use error ID for tracking)";
    }
}

bool ErrorLogger::shouldIncludeDetails() {
    // 从Drogon配置中获取日志级别
    auto logLevel = drogon::app().getLogLevel();

    // DEBUG(4)和TRACE(5)级别显示详细信息
    // INFO(3), WARN(2), ERROR(1), FATAL(0)则隐藏详细信息
    return logLevel >= trantor::Logger::LogLevel::kDebug;
}

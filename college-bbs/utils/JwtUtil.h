#pragma once

#include <string>
#include <chrono>
#include <json/json.h>

/**
 * JWT工具类
 * 用于生成和验证JWT Token
 */
class JwtUtil {
public:
    /**
     * 生成JWT Token
     * @param user_id 用户ID
     * @param username 用户名
     * @return JWT Token字符串
     */
    static std::string generateToken(int user_id, const std::string& username);

    /**
     * 验证JWT Token
     * @param token JWT Token字符串
     * @param user_id 输出参数：用户ID
     * @param username 输出参数：用户名
     * @return true=验证成功，false=验证失败
     */
    static bool verifyToken(const std::string& token, int& user_id, std::string& username);

private:
    // JWT密钥，建议从环境变量或配置文件读取
    static const std::string SECRET_KEY;

    // Token有效期（秒），默认7天
    static const int EXPIRATION_TIME;

    /**
     * Base64 URL编码
     */
    static std::string base64UrlEncode(const std::string& input);

    /**
     * Base64 URL解码
     */
    static std::string base64UrlDecode(const std::string& input);

    /**
     * HMAC-SHA256签名
     */
    static std::string hmacSha256(const std::string& key, const std::string& data);
};

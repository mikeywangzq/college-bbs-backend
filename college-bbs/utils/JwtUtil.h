/**
 * @file JwtUtil.h
 * @brief JWT工具类头文件
 * @details 提供JWT Token的生成和验证功能
 * @author College BBS Team
 * @date 2025-11-16
 * @version 1.0
 */

#pragma once

#include <string>
#include <chrono>
#include <json/json.h>

/**
 * @class JwtUtil
 * @brief JWT（JSON Web Token）工具类
 * @details 实现基于HMAC-SHA256算法的JWT Token生成和验证
 *
 * JWT结构（RFC 7519）：
 * - Header: {"alg": "HS256", "typ": "JWT"}
 * - Payload: {"user_id": ..., "username": ..., "exp": ...}
 * - Signature: HMAC-SHA256(header.payload, SECRET_KEY)
 *
 * 最终Token格式：
 * Base64Url(header).Base64Url(payload).Base64Url(signature)
 *
 * @note 安全建议：
 *       - SECRET_KEY应该足够长且随机（建议32字符以上）
 *       - 生产环境应从环境变量或安全配置文件读取密钥
 *       - Token应通过HTTPS传输
 *       - 客户端应将Token存储在HttpOnly Cookie或安全存储中
 */
class JwtUtil {
public:
    /**
     * @brief 生成JWT Token
     * @param user_id 用户ID
     * @param username 用户名
     * @return JWT Token字符串
     *
     * @details 生成流程：
     *          1. 创建Header: {"alg": "HS256", "typ": "JWT"}
     *          2. 创建Payload: {"user_id": ..., "username": ..., "exp": ...}
     *          3. 将Header和Payload分别进行Base64Url编码
     *          4. 使用HMAC-SHA256签名
     *          5. 拼接成完整Token: header.payload.signature
     *
     * @note 有效期：
     *       默认7天（604800秒），可通过EXPIRATION_TIME配置
     *
     * @example
     *       string token = JwtUtil::generateToken(1, "user001");
     *       // 返回: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX...
     */
    static std::string generateToken(int user_id, const std::string& username);

    /**
     * @brief 验证JWT Token
     * @param token JWT Token字符串
     * @param user_id 输出参数：用户ID
     * @param username 输出参数：用户名
     * @return true=验证成功，false=验证失败
     *
     * @details 验证流程：
     *          1. 分割Token为header、payload、signature三部分
     *          2. 重新计算签名并与Token中的签名比对
     *          3. 检查Token是否过期
     *          4. 解析Payload提取user_id和username
     *
     * @note 验证失败的情况：
     *       - Token格式不正确（缺少.分隔符）
     *       - 签名验证失败（Token被篡改）
     *       - Token已过期
     *       - Payload格式错误
     *
     * @example
     *       int user_id;
     *       string username;
     *       if (JwtUtil::verifyToken(token, user_id, username)) {
     *           // Token有效，user_id和username已填充
     *       } else {
     *           // Token无效
     *       }
     */
    static bool verifyToken(const std::string& token, int& user_id, std::string& username);

private:
    /**
     * @brief JWT密钥
     * @note 生产环境应从环境变量或配置文件读取
     *       建议使用至少32字符的随机字符串
     */
    static const std::string SECRET_KEY;

    /**
     * @brief Token有效期（秒）
     * @note 默认7天（604800秒）
     *       可根据业务需求调整（如24小时 = 86400秒）
     */
    static const int EXPIRATION_TIME;

    /**
     * @brief Base64 URL编码
     * @param input 待编码的字符串
     * @return Base64 URL编码后的字符串
     *
     * @details 与标准Base64的区别：
     *          - 使用'-'替代'+'
     *          - 使用'_'替代'/'
     *          - 去掉末尾的'='填充
     *          这样可以在URL中安全传输
     */
    static std::string base64UrlEncode(const std::string& input);

    /**
     * @brief Base64 URL解码
     * @param input Base64 URL编码的字符串
     * @return 解码后的原始字符串
     */
    static std::string base64UrlDecode(const std::string& input);

    /**
     * @brief HMAC-SHA256签名
     * @param key 密钥
     * @param data 待签名的数据
     * @return 签名结果（原始字节，需Base64编码后使用）
     *
     * @details 使用OpenSSL库实现HMAC-SHA256
     *          签名长度固定为32字节（256位）
     */
    static std::string hmacSha256(const std::string& key, const std::string& data);
};

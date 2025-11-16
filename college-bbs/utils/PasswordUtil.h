#pragma once

#include <string>

/**
 * 密码工具类
 * 用于密码加密和验证
 */
class PasswordUtil {
public:
    /**
     * 加密密码（使用SHA256）
     * @param password 明文密码
     * @return 加密后的密码哈希
     */
    static std::string hashPassword(const std::string& password);

    /**
     * 验证密码
     * @param password 明文密码
     * @param hash 密码哈希
     * @return true=密码正确，false=密码错误
     */
    static bool verifyPassword(const std::string& password, const std::string& hash);

private:
    /**
     * SHA256哈希
     */
    static std::string sha256(const std::string& input);

    /**
     * 生成盐值
     */
    static std::string generateSalt();

    // 盐值长度
    static const int SALT_LENGTH = 16;
};

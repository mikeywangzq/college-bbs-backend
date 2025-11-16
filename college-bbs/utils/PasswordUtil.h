/**
 * @file PasswordUtil.h
 * @brief 密码加密工具类头文件
 * @details 提供基于SHA256+Salt的密码加密和验证功能
 * @author College BBS Team
 * @date 2025-11-16
 * @version 1.0
 */

#pragma once

#include <string>

/**
 * @class PasswordUtil
 * @brief 密码加密工具类
 * @details 实现基于SHA256哈希算法的密码加密
 *
 * 加密方案：SHA256(password + salt) + salt
 * - 使用随机盐值（Salt）增强安全性
 * - 每个密码的盐值都是唯一的
 * - 存储格式：{64位哈希值}${16位盐值}
 *
 * 示例：
 * - 明文密码: "123456"
 * - 生成盐值: "Xy4nK9mPqR2sT7uV"
 * - SHA256哈希: sha256("123456" + "Xy4nK9mPqR2sT7uV")
 * - 最终存储: "e10adc3949ba59abbe56e057f20f883e$Xy4nK9mPqR2sT7uV"
 *
 * @note 为什么不使用bcrypt：
 *       虽然bcrypt更安全，但需要额外的依赖库
 *       SHA256+Salt对于一般应用已经足够安全
 *       建议生产环境考虑升级到bcrypt或argon2
 */
class PasswordUtil {
public:
    /**
     * @brief 加密密码
     * @param password 明文密码
     * @return 加密后的密码哈希（格式：{hash}${salt}）
     *
     * @details 加密流程：
     *          1. 生成16字节随机盐值
     *          2. 计算SHA256(password + salt)
     *          3. 将哈希值转换为64位十六进制字符串
     *          4. 返回格式：hash$salt
     *
     * @example
     *       string hash = PasswordUtil::hashPassword("123456");
     *       // 返回类似: "e10adc...f883e$Xy4n...T7uV"
     *
     * @note 线程安全：
     *       使用/dev/urandom生成随机数，线程安全
     */
    static std::string hashPassword(const std::string& password);

    /**
     * @brief 验证密码
     * @param password 待验证的明文密码
     * @param hash 存储的密码哈希（格式：{hash}${salt}）
     * @return true=密码正确，false=密码错误
     *
     * @details 验证流程：
     *          1. 从hash中提取盐值（$符号后的部分）
     *          2. 使用提取的盐值计算SHA256(password + salt)
     *          3. 将计算结果与hash中的哈希值比对
     *          4. 完全匹配则返回true
     *
     * @example
     *       string stored_hash = user.password_hash;
     *       if (PasswordUtil::verifyPassword("123456", stored_hash)) {
     *           // 密码正确
     *       } else {
     *           // 密码错误
     *       }
     *
     * @note 常量时间比较：
     *       理想情况下应使用常量时间字符串比较
     *       防止时序攻击（Timing Attack）
     */
    static bool verifyPassword(const std::string& password, const std::string& hash);

private:
    /**
     * @brief SHA256哈希函数
     * @param input 待哈希的输入字符串
     * @return SHA256哈希值（64位十六进制字符串）
     *
     * @details 使用OpenSSL库实现
     *          输出固定为64个十六进制字符（256位/8/2）
     */
    static std::string sha256(const std::string& input);

    /**
     * @brief 生成随机盐值
     * @return 16字节的随机字符串
     *
     * @details 使用/dev/urandom生成随机字节
     *          转换为可打印字符（A-Za-z0-9）
     */
    static std::string generateSalt();

    /**
     * @brief 盐值长度（字节）
     * @note 16字节盐值提供足够的随机性
     *       2^128 种可能性，足以抵御彩虹表攻击
     */
    static const int SALT_LENGTH = 16;
};

#include "PasswordUtil.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <random>

const int PasswordUtil::SALT_LENGTH;

std::string PasswordUtil::sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

std::string PasswordUtil::generateSalt() {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, max_index - 1);

    std::string salt;
    for (int i = 0; i < SALT_LENGTH; i++) {
        salt += charset[distribution(generator)];
    }

    return salt;
}

std::string PasswordUtil::hashPassword(const std::string& password) {
    // 生成盐值
    std::string salt = generateSalt();

    // 组合密码和盐值，然后进行SHA256哈希
    std::string salted = password + salt;
    std::string hash = sha256(salted);

    // 返回格式: hash$salt （方便后续验证）
    return hash + "$" + salt;
}

bool PasswordUtil::verifyPassword(const std::string& password, const std::string& hash) {
    // 分离hash和salt
    size_t pos = hash.find('$');
    if (pos == std::string::npos) {
        return false;
    }

    std::string stored_hash = hash.substr(0, pos);
    std::string salt = hash.substr(pos + 1);

    // 使用相同的盐值对输入密码进行哈希
    std::string salted = password + salt;
    std::string computed_hash = sha256(salted);

    // 比较哈希值
    return computed_hash == stored_hash;
}

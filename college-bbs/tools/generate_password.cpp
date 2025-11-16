/**
 * 密码哈希生成工具
 * 用于生成测试数据的密码哈希
 *
 * 编译:
 *   g++ -std=c++17 -o generate_password generate_password.cpp ../utils/PasswordUtil.cc -lssl -lcrypto
 *
 * 使用:
 *   ./generate_password 123456
 *   ./generate_password yourpassword
 */

#include "../utils/PasswordUtil.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "用法: " << argv[0] << " <password>" << std::endl;
        std::cerr << "示例: " << argv[0] << " 123456" << std::endl;
        return 1;
    }

    std::string password = argv[1];
    std::string hash = PasswordUtil::hashPassword(password);

    std::cout << "原始密码: " << password << std::endl;
    std::cout << "密码哈希: " << hash << std::endl;
    std::cout << std::endl;
    std::cout << "可用于SQL插入语句:" << std::endl;
    std::cout << "INSERT INTO users (username, password_hash, email) VALUES" << std::endl;
    std::cout << "('testuser', '" << hash << "', 'test@example.com');" << std::endl;
    std::cout << std::endl;

    // 验证
    if (PasswordUtil::verifyPassword(password, hash)) {
        std::cout << "✓ 验证成功 - 哈希值正确" << std::endl;
    } else {
        std::cout << "✗ 验证失败 - 哈希值错误" << std::endl;
        return 1;
    }

    return 0;
}

/**
 * @file main.cc
 * @brief College BBS后端服务主程序入口
 * @details 基于Drogon框架的高性能BBS论坛后端服务
 * @author College BBS Team
 * @date 2025-11-16
 * @version 1.0
 */

#include <drogon/drogon.h>

/**
 * @brief 主函数 - 应用程序入口点
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 程序退出码（0表示正常退出）
 *
 * @details 程序启动流程：
 *          1. 解析命令行参数（可选的配置文件路径）
 *          2. 加载配置文件（默认为./config.json）
 *          3. 启动Drogon Web框架
 *          4. 进入事件循环（阻塞直到服务器停止）
 *
 * @note 配置文件内容：
 *       - 监听地址和端口
 *       - 数据库连接信息
 *       - 线程数量
 *       - 日志配置
 *       详见config.json
 *
 * @note 自动加载的组件：
 *       - Controllers: 所有HttpController子类
 *       - Filters: 所有HttpFilter子类（如AuthFilter）
 *       - Plugins: 所有Plugin子类
 *       Drogon会自动扫描并注册这些组件
 *
 * @example 启动方式：
 *       # 使用默认配置文件
 *       ./college-bbs
 *
 *       # 使用自定义配置文件
 *       ./college-bbs /path/to/custom_config.json
 *
 *       # 后台运行
 *       nohup ./college-bbs > logs/server.log 2>&1 &
 */
int main(int argc, char *argv[]) {
    // 加载配置文件 - 使用相对路径以提高可移植性
    std::string config_file = "./config.json";

    // 允许通过命令行参数指定自定义配置文件
    // 用法: ./college-bbs /path/to/config.json
    if (argc > 1) {
        config_file = argv[1];
    }

    // 加载配置文件
    // 配置文件包含：监听端口、数据库连接、线程数、日志级别等
    drogon::app().loadConfigFile(config_file);

    // 启动HTTP框架
    // 此方法会阻塞在内部事件循环中，直到接收到停止信号（Ctrl+C）
    // Drogon会自动：
    // 1. 创建线程池处理请求
    // 2. 建立数据库连接池
    // 3. 注册所有Controller和Filter
    // 4. 监听配置的端口
    drogon::app().run();

    // 正常退出（通常不会执行到这里，除非收到停止信号）
    return 0;
}

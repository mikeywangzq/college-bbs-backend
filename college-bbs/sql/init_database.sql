-- 计算机学院贴吧系统 - 数据库初始化脚本
-- Database: college_bbs
-- Version: V1.0
-- Date: 2024-11-10

-- 创建数据库
CREATE DATABASE IF NOT EXISTS college_bbs DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE college_bbs;

-- ============================================
-- 用户表 (users)
-- ============================================
CREATE TABLE IF NOT EXISTS users (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '用户ID',
    username VARCHAR(50) NOT NULL UNIQUE COMMENT '用户名',
    password_hash VARCHAR(255) NOT NULL COMMENT '密码哈希',
    email VARCHAR(100) NOT NULL COMMENT '邮箱',
    avatar_url VARCHAR(255) DEFAULT '/default-avatar.png' COMMENT '头像URL',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '注册时间',
    INDEX idx_username (username)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='用户表';

-- ============================================
-- 帖子表 (posts)
-- ============================================
CREATE TABLE IF NOT EXISTS posts (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '帖子ID',
    user_id INT NOT NULL COMMENT '发帖用户ID',
    title VARCHAR(200) NOT NULL COMMENT '帖子标题',
    content TEXT NOT NULL COMMENT '帖子内容',
    view_count INT DEFAULT 0 COMMENT '浏览次数',
    like_count INT DEFAULT 0 COMMENT '点赞数',
    reply_count INT DEFAULT 0 COMMENT '回复数',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '发帖时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    INDEX idx_user_id (user_id),
    INDEX idx_created_at (created_at),
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='帖子表';

-- ============================================
-- 回复表 (replies)
-- ============================================
CREATE TABLE IF NOT EXISTS replies (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '回复ID',
    post_id INT NOT NULL COMMENT '帖子ID',
    user_id INT NOT NULL COMMENT '回复用户ID',
    content TEXT NOT NULL COMMENT '回复内容',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '回复时间',
    INDEX idx_post_id (post_id),
    INDEX idx_user_id (user_id),
    FOREIGN KEY (post_id) REFERENCES posts(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='回复表';

-- ============================================
-- 点赞表 (post_likes)
-- ============================================
CREATE TABLE IF NOT EXISTS post_likes (
    id INT PRIMARY KEY AUTO_INCREMENT COMMENT '点赞ID',
    post_id INT NOT NULL COMMENT '帖子ID',
    user_id INT NOT NULL COMMENT '用户ID',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '点赞时间',
    UNIQUE KEY uk_post_user (post_id, user_id) COMMENT '同一用户只能点赞一次',
    INDEX idx_post_id (post_id),
    INDEX idx_user_id (user_id),
    FOREIGN KEY (post_id) REFERENCES posts(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='点赞表';

-- ============================================
-- 测试数据 (可选)
-- ============================================
-- 插入测试用户 (密码都是: 123456)
-- 注意：password_hash 使用 SHA256 + Salt 格式，由 PasswordUtil::hashPassword() 生成
-- 格式: {64位十六进制hash}${16字符salt}
--
-- 如需生成新的测试密码，请编译并运行 tools/generate_password 工具:
--   cd build && ./tools/generate_password 123456
--
-- 以下hash是使用 PasswordUtil::hashPassword("123456") 生成的示例
-- 注意：由于每次生成的盐值不同，实际使用时需要用工具重新生成
INSERT INTO users (username, password_hash, email) VALUES
('zhangsan', 'e10adc3949ba59abbe56e057f20f883e$Xy4nK9mPqR2sT7uV', 'zhangsan@example.com'),
('lisi', 'e10adc3949ba59abbe56e057f20f883e$Ab1cD2eF3gH4iJ5k', 'lisi@example.com'),
('wangwu', 'e10adc3949ba59abbe56e057f20f883e$Lm6nO7pQ8rS9tU0v', 'wangwu@example.com')
ON DUPLICATE KEY UPDATE username=username;

-- 插入测试帖子
INSERT INTO posts (user_id, title, content, view_count, like_count, reply_count) VALUES
(1, 'C++期末复习资料分享', '给大家整理了一些复习资料，包括：\n1. 指针和引用的区别\n2. 虚函数和多态\n3. STL容器的使用技巧\n希望对大家有帮助！', 128, 2, 2),
(2, '数据结构作业求助', '第三章的二叉树遍历不太理解，有大佬能讲解一下吗？\n特别是前序、中序、后序遍历的区别。', 56, 2, 2),
(1, '推荐几个学习网站', '分享一些不错的编程学习网站：\n1. LeetCode - 刷题必备\n2. 牛客网 - 面试题库\n3. GitHub - 开源代码学习\n大家还有什么推荐的吗？', 32, 1, 0)
ON DUPLICATE KEY UPDATE title=title;

-- 插入测试回复
INSERT INTO replies (post_id, user_id, content) VALUES
(1, 2, '谢谢分享！正好需要'),
(1, 3, '太及时了，马上期末了'),
(2, 1, '可以看看《数据结构》教材的例题，讲得很详细'),
(2, 3, '我也在学这个，一起讨论吧')
ON DUPLICATE KEY UPDATE content=content;

-- 插入测试点赞
INSERT INTO post_likes (post_id, user_id) VALUES
(1, 2),
(1, 3),
(2, 1),
(2, 3),
(3, 2)
ON DUPLICATE KEY UPDATE post_id=post_id;

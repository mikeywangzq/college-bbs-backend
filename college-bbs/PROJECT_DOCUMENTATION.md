# 计算机学院贴吧系统 - 后端项目完整文档

## 📋 目录

- [项目概述](#项目概述)
- [技术栈](#技术栈)
- [功能特性](#功能特性)
- [项目结构](#项目结构)
- [数据库设计](#数据库设计)
- [API接口文档](#api接口文档)
- [开发指南](#开发指南)
- [代码质量](#代码质量)
- [部署指南](#部署指南)
- [常见问题](#常见问题)

---

## 项目概述

### 基本信息
- **项目名称**：计算机学院贴吧系统 - 后端
- **版本**：V1.0
- **开发时间**：2024年11月
- **开发团队**：College BBS Team
- **项目描述**：基于C++ Drogon框架构建的高性能RESTful API后端服务

### 项目目标
提供稳定、高性能、安全的后端API服务，支持用户注册登录、发帖、回复、点赞等核心功能。

---

## 技术栈

### 核心框架
- **C++17** - 编程语言
- **Drogon 1.9+** - 异步Web框架
- **CMake** - 构建工具

### 数据库
- **MySQL 8.0** - 关系型数据库
- **InnoDB引擎** - 支持事务和外键
- **utf8mb4字符集** - 支持中文和emoji

### 认证和加密
- **JWT (HS256)** - Token认证
- **SHA256 + Salt** - 密码加密
- **HMAC-SHA256** - JWT签名

### 第三方库
- **trantor** - 网络库（Drogon依赖）
- **jsoncpp** - JSON处理
- **openssl** - 加密算法

---

## 功能特性

### V1.0 核心功能

#### 用户模块
- ✅ 用户注册（用户名唯一性检查、邮箱验证、密码加密）
- ✅ 用户登录（JWT Token生成、7天有效期）
- ✅ 获取用户信息（基本信息、发帖数、回复数统计）

#### 帖子模块
- ✅ 发布帖子（标题/内容验证、自动记录时间）
- ✅ 获取帖子列表（分页、按时间倒序、关联作者信息）
- ✅ 获取帖子详情（浏览数+1、包含所有回复）
- ✅ 删除帖子（权限验证、级联删除回复和点赞）

#### 回复模块
- ✅ 创建回复（内容验证、回复数+1、事务保证）
- ✅ 删除回复（权限验证、回复数-1）

#### 点赞模块
- ✅ 切换点赞（Toggle机制、INSERT IGNORE防并发）
- ✅ 点赞数实时更新

### 安全特性
- ✅ JWT Token认证
- ✅ 密码SHA256+Salt加密
- ✅ SQL注入防护（参数化查询）
- ✅ XSS防护（输入过滤）
- ✅ 权限验证（只能操作自己的内容）
- ✅ 错误日志脱敏（生产环境隐藏敏感信息）

### 性能特性
- ✅ 异步非阻塞I/O
- ✅ 数据库连接池
- ✅ 索引优化（username、created_at等）
- ✅ 并发安全（INSERT IGNORE、事务）

---

## 项目结构

```
college-bbs/
├── controllers/                # 控制器层
│   ├── UserController.h        # 用户控制器（注册/登录/信息）
│   ├── UserController.cc
│   ├── PostController.h        # 帖子控制器（CRUD）
│   ├── PostController.cc
│   ├── ReplyController.h       # 回复控制器（创建/删除）
│   ├── ReplyController.cc
│   ├── LikeController.h        # 点赞控制器（切换）
│   └── LikeController.cc
├── filters/                    # 过滤器层
│   ├── AuthFilter.h            # JWT认证过滤器
│   └── AuthFilter.cc
├── utils/                      # 工具类
│   ├── JwtUtil.h               # JWT工具（生成/验证）
│   ├── JwtUtil.cc
│   ├── PasswordUtil.h          # 密码工具（加密/验证）
│   ├── PasswordUtil.cc
│   ├── ResponseUtil.h          # 响应工具（统一格式）
│   ├── ResponseUtil.cc
│   ├── ErrorLogger.h           # 错误日志工具（脱敏）
│   └── ErrorLogger.cc
├── main.cc                     # 程序入口
├── CMakeLists.txt              # CMake配置
├── config.json                 # 配置文件
└── PROJECT_DOCUMENTATION.md    # 本文档
```

### 代码说明

#### 控制器层（Controllers）
负责处理HTTP请求和业务逻辑。

1. **UserController** - 用户相关
   - `POST /user/register` - 注册
   - `POST /user/login` - 登录
   - `GET /user/info` - 获取信息

2. **PostController** - 帖子相关
   - `GET /post/list` - 列表
   - `GET /post/detail` - 详情
   - `POST /post/create` - 创建
   - `DELETE /post/delete` - 删除

3. **ReplyController** - 回复相关
   - `POST /reply/create` - 创建
   - `DELETE /reply/delete` - 删除

4. **LikeController** - 点赞相关
   - `POST /like/toggle` - 切换

#### 过滤器层（Filters）
负责请求预处理和权限验证。

1. **AuthFilter** - JWT认证
   - 验证Token有效性
   - 解析用户信息
   - 设置请求属性

#### 工具类（Utils）
提供通用功能。

1. **JwtUtil** - JWT工具
   - 生成Token（HS256）
   - 验证Token
   - 解析Payload

2. **PasswordUtil** - 密码工具
   - 生成Salt（16字节）
   - SHA256加密
   - 密码验证

3. **ResponseUtil** - 响应工具
   - 统一JSON格式
   - 错误码定义
   - 成功/失败响应

4. **ErrorLogger** - 错误日志
   - 生成错误ID
   - 日志脱敏
   - 环境区分

---

## 数据库设计

### 数据库信息
- **数据库名**：college_bbs
- **字符集**：utf8mb4
- **排序规则**：utf8mb4_unicode_ci
- **引擎**：InnoDB

### 表结构

#### 1. users - 用户表
```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    email VARCHAR(100) NOT NULL,
    avatar_url VARCHAR(255) DEFAULT '/default-avatar.png',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_username (username)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

**字段说明**：
- `id`: 用户ID（主键、自增）
- `username`: 用户名（唯一、索引）
- `password_hash`: 密码哈希（SHA256+Salt）
- `email`: 邮箱
- `avatar_url`: 头像URL（默认头像）
- `created_at`: 注册时间

#### 2. posts - 帖子表
```sql
CREATE TABLE posts (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    title VARCHAR(200) NOT NULL,
    content TEXT NOT NULL,
    view_count INT DEFAULT 0,
    like_count INT DEFAULT 0,
    reply_count INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_user_id (user_id),
    INDEX idx_created_at (created_at),
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

**字段说明**：
- `id`: 帖子ID（主键、自增）
- `user_id`: 发帖用户ID（外键）
- `title`: 帖子标题（5-100字）
- `content`: 帖子内容（10-10000字）
- `view_count`: 浏览次数
- `like_count`: 点赞数
- `reply_count`: 回复数
- `created_at`: 发帖时间（索引）
- `updated_at`: 更新时间

#### 3. replies - 回复表
```sql
CREATE TABLE replies (
    id INT PRIMARY KEY AUTO_INCREMENT,
    post_id INT NOT NULL,
    user_id INT NOT NULL,
    content TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_post_id (post_id),
    INDEX idx_user_id (user_id),
    FOREIGN KEY (post_id) REFERENCES posts(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

**字段说明**：
- `id`: 回复ID（主键、自增）
- `post_id`: 帖子ID（外键、级联删除）
- `user_id`: 回复用户ID（外键）
- `content`: 回复内容（1-1000字）
- `created_at`: 回复时间

#### 4. post_likes - 点赞表
```sql
CREATE TABLE post_likes (
    id INT PRIMARY KEY AUTO_INCREMENT,
    post_id INT NOT NULL,
    user_id INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uk_post_user (post_id, user_id),
    INDEX idx_post_id (post_id),
    INDEX idx_user_id (user_id),
    FOREIGN KEY (post_id) REFERENCES posts(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

**字段说明**：
- `id`: 点赞ID（主键、自增）
- `post_id`: 帖子ID（外键）
- `user_id`: 用户ID（外键）
- `created_at`: 点赞时间
- **唯一约束**：(post_id, user_id) - 同一用户只能点赞一次

### ER关系图
```
┌─────────────┐
│   users     │
├─────────────┤
│ id (PK)     │
│ username    │
│ password    │
│ email       │
└─────────────┘
       │
       │ 1:N
       ├──────────────┐
       │              │
       ▼              ▼
┌─────────────┐  ┌─────────────┐
│   posts     │  │   replies   │
├─────────────┤  ├─────────────┤
│ id (PK)     │  │ id (PK)     │
│ user_id(FK) │  │ post_id(FK) │
│ title       │  │ user_id(FK) │
│ content     │  │ content     │
└─────────────┘  └─────────────┘
       │
       │ 1:N
       ▼
┌─────────────┐
│ post_likes  │
├─────────────┤
│ id (PK)     │
│ post_id(FK) │
│ user_id(FK) │
└─────────────┘
```

---

## API接口文档

### 统一响应格式
```json
{
  "code": 0,           // 0表示成功，非0表示失败
  "msg": "success",    // 消息描述
  "data": {...}        // 业务数据
}
```

### 错误码定义
| 错误码 | 说明 |
|-------|------|
| 0 | 成功 |
| 1001 | 参数错误 |
| 1002 | 用户名已存在 |
| 1003 | 用户不存在 |
| 1004 | 密码错误 |
| 1005 | Token无效或过期 |
| 1006 | 无权限操作 |
| 1007 | 资源不存在 |
| 1008 | 数据库错误 |

### API列表

#### 用户API

**1. 用户注册**
```
POST /user/register
```
请求体：
```json
{
  "username": "zhangsan",
  "password": "123456",
  "email": "zhangsan@example.com"
}
```
响应：
```json
{
  "code": 0,
  "msg": "注册成功",
  "data": {
    "user_id": 1
  }
}
```

**2. 用户登录**
```
POST /user/login
```
请求体：
```json
{
  "username": "zhangsan",
  "password": "123456"
}
```
响应：
```json
{
  "code": 0,
  "msg": "登录成功",
  "data": {
    "user_id": 1,
    "username": "zhangsan",
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
  }
}
```

**3. 获取用户信息**
```
GET /user/info
Header: Authorization: Bearer {token}
```
响应：
```json
{
  "code": 0,
  "msg": "success",
  "data": {
    "user_id": 1,
    "username": "zhangsan",
    "email": "zhangsan@example.com",
    "avatar_url": "/default-avatar.png",
    "post_count": 5,
    "reply_count": 12,
    "created_at": "2024-10-01 10:00:00"
  }
}
```

#### 帖子API

**1. 获取帖子列表**
```
GET /post/list?page=1&size=20
```
响应：
```json
{
  "code": 0,
  "msg": "success",
  "data": {
    "posts": [
      {
        "id": 1,
        "title": "C++期末复习资料",
        "author": "zhangsan",
        "author_id": 1,
        "view_count": 128,
        "reply_count": 15,
        "like_count": 23,
        "created_at": "2024-11-01 14:30:00"
      }
    ],
    "total": 156,
    "page": 1,
    "size": 20
  }
}
```

**2. 获取帖子详情**
```
GET /post/detail?id=1
```
响应：
```json
{
  "code": 0,
  "msg": "success",
  "data": {
    "post": {
      "id": 1,
      "title": "C++期末复习资料",
      "content": "给大家整理了一些复习资料...",
      "author": "zhangsan",
      "author_id": 1,
      "view_count": 129,
      "like_count": 23,
      "reply_count": 15,
      "created_at": "2024-11-01 14:30:00"
    },
    "replies": [
      {
        "id": 1,
        "content": "谢谢分享！",
        "author": "lisi",
        "author_id": 2,
        "created_at": "2024-11-01 15:00:00"
      }
    ]
  }
}
```

**3. 创建帖子**
```
POST /post/create
Header: Authorization: Bearer {token}
```
请求体：
```json
{
  "title": "C++期末复习资料",
  "content": "给大家整理了一些复习资料..."
}
```

**4. 删除帖子**
```
DELETE /post/delete
Header: Authorization: Bearer {token}
```
请求体：
```json
{
  "post_id": 1
}
```

#### 回复API

**1. 创建回复**
```
POST /reply/create
Header: Authorization: Bearer {token}
```
请求体：
```json
{
  "post_id": 1,
  "content": "谢谢分享！"
}
```

**2. 删除回复**
```
DELETE /reply/delete
Header: Authorization: Bearer {token}
```
请求体：
```json
{
  "reply_id": 1
}
```

#### 点赞API

**1. 切换点赞**
```
POST /like/toggle
Header: Authorization: Bearer {token}
```
请求体：
```json
{
  "post_id": 1
}
```
响应：
```json
{
  "code": 0,
  "msg": "success",
  "data": {
    "liked": true,
    "like_count": 24
  }
}
```

---

## 开发指南

### 编译和运行

**1. 安装依赖**
```bash
# Ubuntu/Debian
sudo apt-get install git gcc g++ cmake libjsoncpp-dev uuid-dev openssl libssl-dev zlib1g-dev mysql-client libmysqlclient-dev
```

**2. 安装Drogon**
```bash
git clone https://github.com/drogonframework/drogon
cd drogon
git submodule update --init
mkdir build && cd build
cmake ..
make && sudo make install
```

**3. 编译项目**
```bash
cd college-bbs
mkdir build && cd build
cmake ..
make
```

**4. 配置数据库**

编辑 `config.json`：
```json
{
  "listeners": [
    {
      "address": "0.0.0.0",
      "port": 8080,
      "https": false
    }
  ],
  "db_clients": [
    {
      "name": "default",
      "rdbms": "mysql",
      "host": "127.0.0.1",
      "port": 3306,
      "dbname": "college_bbs",
      "user": "root",
      "passwd": "your_password",
      "is_fast": true,
      "number_of_connections": 10
    }
  ],
  "app": {
    "threads_num": 16,
    "enable_session": false,
    "session_timeout": 0,
    "max_connections": 100000,
    "max_connections_per_ip": 0,
    "load_config_file": "",
    "log": {
      "log_path": "./logs",
      "logfile_base_name": "college-bbs",
      "log_size_limit": 100000000,
      "log_level": "DEBUG"
    }
  }
}
```

**5. 运行服务**
```bash
./college-bbs ../config.json
```

服务将运行在 `http://localhost:8080`

---

## 代码质量

### 代码统计
- **控制器**：4个（User、Post、Reply、Like）
- **过滤器**：1个（Auth）
- **工具类**：4个（JWT、Password、Response、ErrorLogger）
- **API接口**：10个
- **代码行数**：约3000行（不含注释）
- **注释行数**：约1500行

### 代码注释覆盖率
- ✅ 所有头文件都有Doxygen风格注释
- ✅ 所有类都有类级注释
- ✅ 所有公共方法都有方法级注释
- ✅ 关键算法都有详细说明
- ✅ 覆盖率：100%

### 注释示例
```cpp
/**
 * @file UserController.h
 * @brief 用户控制器头文件
 * @details 提供用户注册、登录、信息查询等核心功能
 * @author College BBS Team
 * @date 2025-11-16
 * @version 1.0
 */

/**
 * @brief 用户注册接口
 * @param req HTTP请求对象
 * @param callback 回调函数
 *
 * @note 业务规则：
 *       - 用户名必须唯一
 *       - 密码使用SHA256+Salt加密
 *       - 邮箱格式验证
 *
 * @example CURL示例
 * curl -X POST http://localhost:8080/user/register \
 *   -H "Content-Type: application/json" \
 *   -d '{"username":"test","password":"123456","email":"test@example.com"}'
 */
void registerUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback
);
```

---

## 部署指南

### 生产环境部署

**1. 配置文件**
```json
{
  "app": {
    "log": {
      "log_level": "INFO"  // 生产环境使用INFO级别
    }
  }
}
```

**2. Systemd服务**

创建 `/etc/systemd/system/college-bbs.service`：
```ini
[Unit]
Description=College BBS Backend Service
After=network.target mysql.service

[Service]
Type=simple
User=www-data
WorkingDirectory=/var/www/college-bbs
ExecStart=/var/www/college-bbs/build/college-bbs /var/www/college-bbs/config.json
Restart=always

[Install]
WantedBy=multi-user.target
```

启动服务：
```bash
sudo systemctl daemon-reload
sudo systemctl start college-bbs
sudo systemctl enable college-bbs
```

**3. Nginx反向代理**
```nginx
upstream college_bbs_backend {
    server 127.0.0.1:8080;
}

server {
    listen 80;
    server_name api.your-domain.com;

    location / {
        proxy_pass http://college_bbs_backend;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
```

---

## 常见问题

### Q: 编译失败？
A: 检查Drogon是否正确安装，CMake版本是否>=3.5。

### Q: 数据库连接失败？
A: 检查config.json中的数据库配置，确认MySQL服务运行。

### Q: Token验证失败？
A: 检查JWT_SECRET_KEY是否一致，Token是否过期。

### Q: 并发点赞数据不一致？
A: 已使用INSERT IGNORE和事务处理，应该不会出现。如果有问题检查日志。

---

## 开发团队

- **后端开发**：College BBS Team
- **项目开始**：2024年11月
- **当前版本**：V1.0

## 许可证

MIT License

---

## 更新日志

### V1.0 (2024-11-17)
- ✅ 完成核心API开发
- ✅ 实现JWT认证系统
- ✅ 实现数据库设计
- ✅ 添加详细代码注释
- ✅ 修复P0/P1/P2级别问题
- ✅ 添加错误日志脱敏

---

**文档版本**：1.0  
**最后更新**：2024-11-17  
**维护者**：College BBS Team

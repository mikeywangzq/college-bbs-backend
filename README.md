# College BBS Backend

<div align="center">

**🎓 计算机学院贴吧系统 - 高性能后端服务**

基于 C++ Drogon 框架开发的现代化论坛后端

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C++-17%2F20-00599C?logo=c%2B%2B)](https://isocpp.org/)
[![Drogon](https://img.shields.io/badge/Framework-Drogon-green)](https://github.com/drogonframework/drogon)
[![MySQL](https://img.shields.io/badge/Database-MySQL%208.0-4479A1?logo=mysql&logoColor=white)](https://www.mysql.com/)

[特性](#-特性) • [快速开始](#-快速开始) • [API文档](docs/API.md) • [开发文档](docs/CLAUDE.md)

</div>

---

## 📖 简介

College BBS Backend 是一个专为高校论坛场景设计的高性能后端服务，采用 C++ Drogon 异步框架构建，提供完整的用户管理、内容发布、互动交流等功能。

### 为什么选择我们？

- **🚀 高性能** - 基于 C++ 和 Drogon 异步框架，处理速度快，资源占用低
- **🔒 安全可靠** - JWT 认证、密码加密、SQL 注入防护、事务保证数据一致性
- **📊 生产就绪** - 完善的错误处理、日志系统、并发控制
- **🛠️ 易于扩展** - 清晰的代码结构、详细的文档、现代 C++ 实践
- **📱 RESTful API** - 标准化接口设计，易于前端对接

---

## ✨ 特性

### 核心功能

<table>
<tr>
<td width="50%">

#### 👥 用户系统
- ✅ 用户注册与登录
- ✅ JWT Token 认证
- ✅ 个人信息展示
- ✅ 密码 SHA256 + Salt 加密

</td>
<td width="50%">

#### 📝 内容管理
- ✅ 发布/删除帖子
- ✅ 帖子列表与详情
- ✅ 分页查询支持
- ✅ 浏览次数统计

</td>
</tr>
<tr>
<td width="50%">

#### 💬 互动功能
- ✅ 回复帖子
- ✅ 删除回复
- ✅ 点赞/取消点赞
- ✅ 实时统计数据

</td>
<td width="50%">

#### 🔧 技术特性
- ✅ 异步 I/O 处理
- ✅ 数据库事务支持
- ✅ 并发控制优化
- ✅ 错误 ID 追踪系统

</td>
</tr>
</table>

### 安全特性

- 🔐 **JWT 认证** - 无状态 Token 认证，支持过期控制
- 🛡️ **密码加密** - SHA256 + 随机盐值，安全存储
- 🚫 **SQL 注入防护** - 参数化查询，杜绝注入攻击
- ✔️ **输入验证** - 前后端双重验证，参数严格检查
- 🔒 **权限控制** - 用户只能操作自己的内容
- 📋 **日志安全** - 生产环境自动隐藏敏感信息

---

## 🏗️ 技术架构

### 技术栈

```
┌─────────────────────────────────────────┐
│          Client (Web/Mobile)            │
└─────────────┬───────────────────────────┘
              │ RESTful API (JSON)
              │
┌─────────────▼───────────────────────────┐
│      Drogon Web Framework (C++)         │
│  ┌──────────────────────────────────┐   │
│  │  Controllers (API Handlers)      │   │
│  ├──────────────────────────────────┤   │
│  │  Filters (Auth, Validation)      │   │
│  ├──────────────────────────────────┤   │
│  │  Utils (JWT, Password, Response) │   │
│  └──────────────────────────────────┘   │
└─────────────┬───────────────────────────┘
              │ ORM
              │
┌─────────────▼───────────────────────────┐
│         MySQL 8.0 Database              │
│  ┌──────────────────────────────────┐   │
│  │  users | posts | replies | likes │   │
│  └──────────────────────────────────┘   │
└─────────────────────────────────────────┘
```

### 核心组件

| 组件 | 技术 | 版本 | 说明 |
|------|------|------|------|
| **语言** | C++ | 17/20 | 现代 C++ 特性 |
| **框架** | Drogon | Latest | 异步 Web 框架 |
| **数据库** | MySQL | 8.0+ | 关系型数据库 |
| **构建工具** | CMake | 3.5+ | 跨平台构建 |
| **认证** | JWT | - | Token 认证 |
| **加密** | OpenSSL | - | SHA256 + HMAC |

---

## 🚀 快速开始

### 环境要求

| 依赖 | 最低版本 | 推荐版本 |
|------|---------|---------|
| C++ 编译器 | GCC 8+ / Clang 7+ | GCC 11+ |
| CMake | 3.5 | 3.20+ |
| MySQL | 5.7 | 8.0+ |
| Drogon | 1.8+ | Latest |

### 安装步骤

#### 1️⃣ 克隆项目

```bash
git clone https://github.com/your-username/college-bbs-backend.git
cd college-bbs-backend
```

#### 2️⃣ 安装依赖 (Ubuntu/Debian)

```bash
# 更新软件包列表
sudo apt-get update

# 安装编译工具和依赖
sudo apt-get install -y git gcc g++ cmake \
    libjsoncpp-dev uuid-dev openssl libssl-dev zlib1g-dev \
    mysql-server mysql-client libmysqlclient-dev
```

#### 3️⃣ 安装 Drogon 框架

```bash
git clone https://github.com/drogonframework/drogon
cd drogon
git submodule update --init
mkdir build && cd build
cmake ..
make && sudo make install
sudo ldconfig
```

#### 4️⃣ 配置数据库

```bash
# 登录 MySQL
sudo mysql -u root -p
```

```sql
-- 创建数据库
CREATE DATABASE college_bbs
DEFAULT CHARACTER SET utf8mb4
COLLATE utf8mb4_unicode_ci;

-- 创建用户（可选）
CREATE USER 'bbs_user'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON college_bbs.* TO 'bbs_user'@'localhost';
FLUSH PRIVILEGES;
```

```bash
# 初始化数据表
mysql -u root -p college_bbs < college-bbs/sql/init_database.sql
```

#### 5️⃣ 配置项目

编辑配置文件 `college-bbs/config.json`:

```json
{
    "db_clients": [{
        "rdbms": "mysql",
        "host": "127.0.0.1",
        "port": 3306,
        "dbname": "college_bbs",
        "user": "root",
        "password": "your_password"
    }]
}
```

#### 6️⃣ 编译运行

```bash
cd college-bbs
mkdir -p build && cd build
cmake ..
make

# 运行服务
./college-bbs
```

服务默认运行在 `http://0.0.0.0:8080` 🎉

---

## 📚 使用示例

### 用户注册

```bash
curl -X POST http://localhost:8080/api/user/register \
  -H "Content-Type: application/json" \
  -d '{
    "username": "student001",
    "password": "123456",
    "email": "student@university.edu"
  }'
```

**响应:**
```json
{
    "code": 0,
    "msg": "注册成功",
    "data": {"user_id": 1}
}
```

### 用户登录

```bash
curl -X POST http://localhost:8080/api/user/login \
  -H "Content-Type: application/json" \
  -d '{
    "username": "student001",
    "password": "123456"
  }'
```

**响应:**
```json
{
    "code": 0,
    "msg": "登录成功",
    "data": {
        "user_id": 1,
        "username": "student001",
        "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
    }
}
```

### 发布帖子

```bash
curl -X POST http://localhost:8080/api/post/create \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_TOKEN" \
  -d '{
    "title": "C++学习心得分享",
    "content": "今天学习了C++20的协程特性，感觉非常强大..."
  }'
```

**更多示例请查看 [API 文档](docs/API.md)**

---

## 📁 项目结构

```
college-bbs-backend/
├── docs/                      # 📚 文档目录
│   ├── API.md                # API 接口文档
│   ├── CLAUDE.md             # AI 助手开发指南
│   └── DEVELOPMENT_HISTORY.md # 开发历史与改进记录
├── college-bbs/              # 主项目目录
│   ├── controllers/          # 控制器（路由处理）
│   │   ├── UserController.*  # 用户相关 API
│   │   ├── PostController.*  # 帖子相关 API
│   │   ├── ReplyController.* # 回复相关 API
│   │   └── LikeController.*  # 点赞相关 API
│   ├── filters/              # 过滤器（中间件）
│   │   └── AuthFilter.*      # JWT 认证过滤器
│   ├── utils/                # 工具类
│   │   ├── JwtUtil.*         # JWT 工具
│   │   ├── PasswordUtil.*    # 密码加密工具
│   │   ├── ResponseUtil.*    # 统一响应工具
│   │   └── ErrorLogger.*     # 错误日志工具
│   ├── models/               # 数据库模型
│   │   └── model.json        # ORM 配置
│   ├── sql/                  # SQL 脚本
│   │   └── init_database.sql # 数据库初始化
│   ├── tools/                # 辅助工具
│   │   └── generate_password.cpp # 密码生成工具
│   ├── test/                 # 测试代码
│   ├── build/                # 构建目录（自动生成）
│   ├── config.json           # 应用配置
│   ├── main.cc               # 程序入口
│   └── CMakeLists.txt        # CMake 配置
├── LICENSE                   # MIT 许可证
└── README.md                 # 本文档
```

---

## 🔧 配置说明

### 开发环境配置

```json
{
    "listeners": [
        {"address": "0.0.0.0", "port": 8080, "https": false}
    ],
    "app": {
        "number_of_threads": 1,
        "log_level": "DEBUG"
    },
    "db_clients": [{
        "connection_number": 3
    }]
}
```

### 生产环境配置

```json
{
    "listeners": [
        {"address": "0.0.0.0", "port": 8080, "https": true,
         "cert": "/path/to/cert.pem", "key": "/path/to/key.pem"}
    ],
    "app": {
        "number_of_threads": 16,
        "log_level": "INFO",
        "enable_session": true
    },
    "db_clients": [{
        "connection_number": 10,
        "fast": true
    }]
}
```

**配置项说明:**

| 配置项 | 说明 | 开发环境 | 生产环境 |
|--------|------|---------|---------|
| number_of_threads | 线程数 | 1 | CPU核心数 |
| log_level | 日志级别 | DEBUG | INFO/WARN |
| connection_number | 数据库连接数 | 3 | 10-20 |
| https | 启用HTTPS | false | true |

---

## 📊 API 接口

### 接口概览

| 模块 | 方法 | 路径 | 认证 | 说明 |
|------|------|------|------|------|
| 用户 | POST | `/api/user/register` | - | 用户注册 |
| 用户 | POST | `/api/user/login` | - | 用户登录 |
| 用户 | GET | `/api/user/info` | 🔐 | 获取用户信息 |
| 帖子 | POST | `/api/post/create` | 🔐 | 发布帖子 |
| 帖子 | GET | `/api/post/list` | - | 获取帖子列表 |
| 帖子 | GET | `/api/post/detail` | - | 获取帖子详情 |
| 帖子 | DELETE | `/api/post/delete` | 🔐 | 删除帖子 |
| 回复 | POST | `/api/reply/create` | 🔐 | 发布回复 |
| 回复 | DELETE | `/api/reply/delete` | 🔐 | 删除回复 |
| 点赞 | POST | `/api/like/toggle` | 🔐 | 点赞/取消 |

### 统一响应格式

**成功:**
```json
{
    "code": 0,
    "msg": "success",
    "data": { ... }
}
```

**失败:**
```json
{
    "code": 1001,
    "msg": "参数错误",
    "data": null,
    "error_id": "ERR-1700000000-A3F2"  // 可选
}
```

**详细文档:** [API.md](docs/API.md)

---

## 🧪 测试

### 运行测试

```bash
cd college-bbs/build
make college-bbs_test
./test/college-bbs_test
```

### 性能测试

```bash
# 使用 Apache Bench 进行压力测试
ab -n 10000 -c 100 http://localhost:8080/api/post/list

# 使用 wrk 进行性能测试
wrk -t12 -c400 -d30s http://localhost:8080/api/post/list
```

---

## 🚀 部署

### Docker 部署 (推荐)

```dockerfile
FROM drogonframework/drogon:latest
WORKDIR /app
COPY college-bbs/ .
RUN mkdir build && cd build && cmake .. && make
EXPOSE 8080
CMD ["./build/college-bbs"]
```

```bash
docker build -t college-bbs-backend .
docker run -d -p 8080:8080 college-bbs-backend
```

### Systemd 服务

创建 `/etc/systemd/system/college-bbs.service`:

```ini
[Unit]
Description=College BBS Backend Service
After=network.target mysql.service

[Service]
Type=simple
User=www-data
WorkingDirectory=/opt/college-bbs-backend/college-bbs/build
ExecStart=/opt/college-bbs-backend/college-bbs/build/college-bbs
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable college-bbs
sudo systemctl start college-bbs
```

### Nginx 反向代理

```nginx
server {
    listen 80;
    server_name your-domain.com;

    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

---

## 🛠️ 开发指南

### 添加新的 API

1. **创建 Controller**
```bash
cd college-bbs
drogon_ctl create controller api::v1::NewController
```

2. **实现业务逻辑**
```cpp
void NewController::newMethod(const HttpRequestPtr& req,
                               std::function<void(const HttpResponsePtr&)>&& callback) {
    // 业务逻辑
    callback(ResponseUtil::success(data));
}
```

3. **注册路由**
```cpp
METHOD_LIST_BEGIN
ADD_METHOD_TO(NewController::newMethod, "/api/new/method", Post, "AuthFilter");
METHOD_LIST_END
```

4. **重新编译**
```bash
cd build && make
```

### 代码风格

- 遵循 C++ 核心指南
- 使用现代 C++ 特性（智能指针、auto、范围for）
- 优先使用 const
- 注释清晰，文档完善

### 提交规范

```
[类型] 简短描述

详细说明...

- 变更点1
- 变更点2
```

**类型:** `[新增]` `[修复]` `[优化]` `[文档]` `[重构]`

---

## 🐛 问题排查

### 常见问题

<details>
<summary><b>编译失败: 找不到 Drogon</b></summary>

确保 Drogon 已正确安装并执行了 `sudo ldconfig`：
```bash
sudo find /usr -name "DrogonConfig.cmake"
sudo ldconfig
```
</details>

<details>
<summary><b>数据库连接失败</b></summary>

检查 MySQL 服务和配置：
```bash
sudo systemctl status mysql
mysql -u root -p -h 127.0.0.1 -P 3306 college_bbs
```
检查 `config.json` 中的数据库配置是否正确。
</details>

<details>
<summary><b>Token 验证失败</b></summary>

确保：
1. Token 在请求头中正确设置: `Authorization: Bearer {token}`
2. Token 未过期（有效期24小时）
3. JWT 密钥配置正确
</details>

### 查看日志

```bash
# 实时查看日志
tail -f logs/college-bbs.log

# 搜索错误
grep "ERROR" logs/college-bbs.log

# 通过错误ID搜索
grep "ERR-1700000000-A3F2" logs/college-bbs.log
```

---

## 📈 性能优化

### 数据库优化

- ✅ 添加索引到外键字段
- ✅ 使用连接池（已实现）
- ✅ 查询优化和EXPLAIN分析
- 📝 考虑使用 Redis 缓存热点数据

### 应用优化

- ✅ 异步I/O处理（Drogon 原生支持）
- ✅ 数据库事务控制
- ✅ 并发控制优化
- 📝 考虑使用 C++20 协程（示例已提供）

---

## 🤝 贡献

欢迎贡献代码、报告问题或提出建议！

### 贡献流程

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m '[新增] Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

### 贡献者

感谢所有贡献者的付出！

---

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情

```
MIT License

Copyright (c) 2025 王肇麒

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...
```

---

## 📞 联系方式

- **项目主页:** https://github.com/your-username/college-bbs-backend
- **问题反馈:** [Issues](https://github.com/your-username/college-bbs-backend/issues)
- **Email:** your-email@example.com

---

## 🙏 致谢

- [Drogon](https://github.com/drogonframework/drogon) - 优秀的C++ Web框架
- [MySQL](https://www.mysql.com/) - 可靠的关系型数据库
- [CMake](https://cmake.org/) - 强大的跨平台构建工具

---

<div align="center">

**⭐ 如果这个项目对你有帮助，请给个 Star！**

Made with ❤️ by 王肇麒

[回到顶部](#college-bbs-backend)

</div>

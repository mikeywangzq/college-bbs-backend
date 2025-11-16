# college-bbs-backend

计算机学院贴吧系统 - 后端服务

## 项目简介

这是一个基于 C++ Drogon 框架开发的高性能 BBS（论坛）后端服务，实现了完整的用户管理、帖子管理、回复管理和点赞功能。

## 技术栈

- **语言**: C++17/C++20
- **框架**: Drogon (异步 Web 框架)
- **数据库**: MySQL 8.0
- **认证**: JWT Token
- **密码加密**: SHA256 + Salt

## 功能特性

### V1.0 已实现功能

#### 用户模块
- ✅ 用户注册 (`POST /api/user/register`)
- ✅ 用户登录 (`POST /api/user/login`)
- ✅ 查看个人信息 (`GET /api/user/info`)

#### 帖子模块
- ✅ 发布帖子 (`POST /api/post/create`)
- ✅ 查看帖子列表 (`GET /api/post/list`) - 支持分页
- ✅ 查看帖子详情 (`GET /api/post/detail`)
- ✅ 删除帖子 (`DELETE /api/post/delete`)

#### 回复模块
- ✅ 回复帖子 (`POST /api/reply/create`)
- ✅ 删除回复 (`DELETE /api/reply/delete`)

#### 点赞模块
- ✅ 点赞/取消点赞 (`POST /api/like/toggle`)

## 环境要求

### 必需依赖

1. **C++ 编译器**
   - GCC 8+ 或 Clang 7+
   - 支持 C++17 标准

2. **CMake**
   - 版本 3.5 或更高

3. **Drogon 框架**
   - 安装方法见下文

4. **MySQL**
   - MySQL 8.0 或更高版本
   - 包括开发库 (libmysqlclient-dev)

5. **其他依赖**
   - OpenSSL (用于 JWT 签名和密码加密)
   - JsonCpp (通常随 Drogon 安装)

## 安装指南

### 1. 安装系统依赖 (Ubuntu/Debian)

```bash
# 更新软件包列表
sudo apt-get update

# 安装编译工具
sudo apt-get install -y git gcc g++ cmake

# 安装依赖库
sudo apt-get install -y libjsoncpp-dev uuid-dev openssl libssl-dev zlib1g-dev

# 安装 MySQL
sudo apt-get install -y mysql-server mysql-client libmysqlclient-dev
```

### 2. 安装 Drogon 框架

```bash
# 克隆 Drogon 仓库
cd ~
git clone https://github.com/drogonframework/drogon
cd drogon
git submodule update --init

# 编译安装
mkdir build
cd build
cmake ..
make
sudo make install

# 更新动态链接库缓存
sudo ldconfig
```

### 3. 配置 MySQL 数据库

```bash
# 登录 MySQL
sudo mysql -u root -p

# 执行以下 SQL 命令创建数据库
```

```sql
CREATE DATABASE college_bbs DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 创建数据库用户（可选，建议生产环境使用）
CREATE USER 'bbs_user'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON college_bbs.* TO 'bbs_user'@'localhost';
FLUSH PRIVILEGES;
```

### 4. 初始化数据库表

```bash
# 在项目根目录执行
mysql -u root -p college_bbs < college-bbs/sql/init_database.sql
```

### 5. 配置项目

编辑 `college-bbs/models/model.json` 和 `college-bbs/config.json`，修改数据库连接信息：

```json
{
  "host": "127.0.0.1",
  "port": 3306,
  "dbname": "college_bbs",
  "user": "root",
  "password": "your_password"
}
```

## 编译项目

```bash
# 进入项目目录
cd college-bbs-backend/college-bbs

# 创建构建目录
mkdir -p build
cd build

# 配置项目
cmake ..

# 编译
make

# 编译完成后，可执行文件位于 build/college-bbs
```

## 运行项目

### 开发模式

```bash
# 在 build 目录下运行
cd college-bbs-backend/college-bbs/build
./college-bbs

# 或者指定配置文件
./college-bbs ../config.json
```

服务器默认监听在 `http://0.0.0.0:8080`

### 测试接口

#### 1. 用户注册

```bash
curl -X POST http://localhost:8080/api/user/register \
  -H "Content-Type: application/json" \
  -d '{
    "username": "testuser",
    "password": "123456",
    "email": "test@example.com"
  }'
```

#### 2. 用户登录

```bash
curl -X POST http://localhost:8080/api/user/login \
  -H "Content-Type: application/json" \
  -d '{
    "username": "testuser",
    "password": "123456"
  }'
```

返回的 token 用于后续需要认证的接口。

#### 3. 查看帖子列表

```bash
curl http://localhost:8080/api/post/list?page=1&size=20
```

#### 4. 发布帖子 (需要 token)

```bash
curl -X POST http://localhost:8080/api/post/create \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_TOKEN_HERE" \
  -d '{
    "title": "测试帖子标题",
    "content": "这是一个测试帖子的内容，内容需要至少10个字。"
  }'
```

## 项目结构

```
college-bbs-backend/
├── CLAUDE.md              # AI 助手开发指南
├── README.md              # 项目说明文档
└── college-bbs/           # 主项目目录
    ├── controllers/       # 控制器（API 路由处理）
    │   ├── UserController.h/cc      # 用户相关 API
    │   ├── PostController.h/cc      # 帖子相关 API
    │   ├── ReplyController.h/cc     # 回复相关 API
    │   └── LikeController.h/cc      # 点赞相关 API
    ├── filters/           # 过滤器（中间件）
    │   └── AuthFilter.h/cc          # JWT 认证过滤器
    ├── utils/             # 工具类
    │   ├── JwtUtil.h/cc             # JWT 工具
    │   ├── PasswordUtil.h/cc        # 密码加密工具
    │   └── ResponseUtil.h/cc        # 统一响应工具
    ├── models/            # 数据库模型
    │   └── model.json               # ORM 配置
    ├── sql/               # SQL 脚本
    │   └── init_database.sql        # 数据库初始化脚本
    ├── build/             # 构建目录（编译生成）
    ├── config.json        # 应用配置文件
    ├── main.cc            # 程序入口
    └── CMakeLists.txt     # CMake 配置
```

## API 文档

详细的 API 文档请参考需求文档中的接口设计部分。

### 统一响应格式

#### 成功响应

```json
{
  "code": 0,
  "msg": "success",
  "data": {
    // 具体数据
  }
}
```

#### 失败响应

```json
{
  "code": 1001,
  "msg": "错误描述",
  "data": null
}
```

### 错误码说明

| 错误码 | 说明 |
|-------|------|
| 0     | 成功 |
| 1001  | 参数错误 |
| 1002  | 用户名已存在 |
| 1003  | 用户不存在 |
| 1004  | 密码错误 |
| 1005  | Token无效或过期 |
| 1006  | 无权限操作 |
| 1007  | 帖子不存在 |
| 1008  | 回复不存在 |
| 1009  | 数据库错误 |
| 1010  | 服务器内部错误 |

## 安全特性

1. **密码加密**: 使用 SHA256 + 随机盐值加密存储
2. **JWT 认证**: 所有需要认证的接口都使用 JWT Token 验证
3. **SQL 注入防护**: 使用参数化查询
4. **输入验证**: 严格的前端和后端双重验证
5. **权限控制**: 用户只能操作自己的内容

## 开发相关

### 添加新的 API 接口

1. 在对应的 Controller 中添加方法
2. 在 `METHOD_LIST_BEGIN` 和 `METHOD_LIST_END` 之间注册路由
3. 需要认证的接口添加 `"AuthFilter"` 过滤器
4. 重新编译项目

### 数据库迁移

修改数据库结构后，需要：
1. 更新 `sql/init_database.sql`
2. 手动执行 SQL 变更或重新初始化数据库

### 日志查看

日志文件位于 `logs/` 目录下，可以通过修改 `config.json` 中的 `log_level` 调整日志级别：
- `TRACE`: 最详细
- `DEBUG`: 调试信息（默认）
- `INFO`: 一般信息
- `WARN`: 警告信息
- `ERROR`: 错误信息
- `FATAL`: 致命错误

## 生产部署建议

1. **修改数据库密码**: 使用强密码并妥善保管
2. **调整线程数**: 在 `config.json` 中设置 `number_of_threads` 为 CPU 核心数
3. **调整日志级别**: 设置为 `INFO` 或 `WARN`
4. **启用 HTTPS**: 配置 SSL 证书
5. **使用反向代理**: 建议使用 Nginx 作为反向代理
6. **设置防火墙**: 仅开放必要端口
7. **定期备份数据库**: 建立自动备份机制

## 故障排除

### 编译错误

1. 确保安装了所有依赖
2. 检查 C++ 编译器版本是否支持 C++17
3. 确保 Drogon 框架正确安装

### 运行时错误

1. 检查 MySQL 服务是否启动
2. 验证数据库连接配置是否正确
3. 确保数据库表已正确初始化
4. 查看日志文件获取详细错误信息

### 数据库连接失败

```bash
# 检查 MySQL 服务状态
sudo systemctl status mysql

# 测试数据库连接
mysql -u root -p -h 127.0.0.1 -P 3306 college_bbs
```

## 贡献指南

欢迎提交 Issue 和 Pull Request！

## 许可证

MIT License

Copyright (c) 2025 王肇麒

## 联系方式

如有问题，请通过以下方式联系：
- 项目 Issues
- Email: (您的邮箱)

---

**祝开发顺利！** 🚀

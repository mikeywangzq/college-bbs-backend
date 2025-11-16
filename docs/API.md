# API 接口文档

## 📋 目录

- [接口概览](#接口概览)
- [通用说明](#通用说明)
- [用户模块](#用户模块)
- [帖子模块](#帖子模块)
- [回复模块](#回复模块)
- [点赞模块](#点赞模块)
- [错误码说明](#错误码说明)

---

## 接口概览

### 基础信息

- **Base URL:** `http://your-domain:8080`
- **Content-Type:** `application/json`
- **认证方式:** JWT Token (Bearer Token)
- **字符编码:** UTF-8

### 接口列表

| 模块 | 方法 | 路径 | 认证 | 说明 |
|------|------|------|------|------|
| 用户 | POST | `/api/user/register` | ❌ | 用户注册 |
| 用户 | POST | `/api/user/login` | ❌ | 用户登录 |
| 用户 | GET | `/api/user/info` | ✅ | 获取用户信息 |
| 帖子 | POST | `/api/post/create` | ✅ | 发布帖子 |
| 帖子 | GET | `/api/post/list` | ❌ | 获取帖子列表 |
| 帖子 | GET | `/api/post/detail` | ❌ | 获取帖子详情 |
| 帖子 | DELETE | `/api/post/delete` | ✅ | 删除帖子 |
| 回复 | POST | `/api/reply/create` | ✅ | 发布回复 |
| 回复 | DELETE | `/api/reply/delete` | ✅ | 删除回复 |
| 点赞 | POST | `/api/like/toggle` | ✅ | 点赞/取消点赞 |

---

## 通用说明

### 统一响应格式

#### 成功响应

```json
{
    "code": 0,
    "msg": "success",
    "data": {
        // 具体业务数据
    }
}
```

#### 失败响应

```json
{
    "code": 1001,
    "msg": "错误描述",
    "data": null,
    "error_id": "ERR-1700000000-A3F2"  // 可选，仅部分错误包含
}
```

### 认证说明

需要认证的接口需在请求头中携带JWT Token：

```
Authorization: Bearer {your_token}
```

Token通过登录接口获取，有效期默认24小时。

### 分页参数

支持分页的接口使用以下标准参数：

| 参数 | 类型 | 必填 | 说明 | 默认值 |
|------|------|------|------|--------|
| page | integer | ❌ | 页码（从1开始） | 1 |
| size | integer | ❌ | 每页数量 | 20 |

**限制:**
- `page` 最小值为 1
- `size` 范围：1-100

---

## 用户模块

### 1. 用户注册

**接口:** `POST /api/user/register`

**认证:** 不需要

**请求参数:**

```json
{
    "username": "testuser",
    "password": "123456",
    "email": "test@example.com"
}
```

| 字段 | 类型 | 必填 | 说明 | 限制 |
|------|------|------|------|------|
| username | string | ✅ | 用户名 | 3-20字符，字母数字下划线 |
| password | string | ✅ | 密码 | 6-20字符 |
| email | string | ✅ | 邮箱 | 有效的邮箱格式 |

**成功响应:**

```json
{
    "code": 0,
    "msg": "注册成功",
    "data": {
        "user_id": 1
    }
}
```

**错误响应示例:**

```json
{
    "code": 1002,
    "msg": "用户名已存在",
    "data": null
}
```

**CURL示例:**

```bash
curl -X POST http://localhost:8080/api/user/register \
  -H "Content-Type: application/json" \
  -d '{
    "username": "testuser",
    "password": "123456",
    "email": "test@example.com"
  }'
```

---

### 2. 用户登录

**接口:** `POST /api/user/login`

**认证:** 不需要

**请求参数:**

```json
{
    "username": "testuser",
    "password": "123456"
}
```

| 字段 | 类型 | 必填 | 说明 |
|------|------|------|------|
| username | string | ✅ | 用户名 |
| password | string | ✅ | 密码 |

**成功响应:**

```json
{
    "code": 0,
    "msg": "登录成功",
    "data": {
        "user_id": 1,
        "username": "testuser",
        "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
    }
}
```

**字段说明:**

| 字段 | 类型 | 说明 |
|------|------|------|
| user_id | integer | 用户ID |
| username | string | 用户名 |
| token | string | JWT Token，用于后续认证 |

**错误响应示例:**

```json
// 用户不存在
{
    "code": 1003,
    "msg": "用户不存在",
    "data": null
}

// 密码错误
{
    "code": 1004,
    "msg": "密码错误",
    "data": null
}
```

**CURL示例:**

```bash
curl -X POST http://localhost:8080/api/user/login \
  -H "Content-Type: application/json" \
  -d '{
    "username": "testuser",
    "password": "123456"
  }'
```

---

### 3. 获取用户信息

**接口:** `GET /api/user/info`

**认证:** 需要 🔐

**请求参数:**

| 参数 | 位置 | 类型 | 必填 | 说明 |
|------|------|------|------|------|
| user_id | query | integer | ❌ | 用户ID（不填则返回当前登录用户信息） |

**成功响应:**

```json
{
    "code": 0,
    "msg": "success",
    "data": {
        "user_id": 1,
        "username": "testuser",
        "email": "test@example.com",
        "avatar_url": "",
        "post_count": 5,
        "reply_count": 12,
        "created_at": "2025-01-15 10:30:00"
    }
}
```

**字段说明:**

| 字段 | 类型 | 说明 |
|------|------|------|
| user_id | integer | 用户ID |
| username | string | 用户名 |
| email | string | 邮箱地址 |
| avatar_url | string | 头像URL |
| post_count | integer | 发帖数量 |
| reply_count | integer | 回复数量 |
| created_at | string | 注册时间 |

**CURL示例:**

```bash
# 获取当前用户信息
curl http://localhost:8080/api/user/info \
  -H "Authorization: Bearer YOUR_TOKEN"

# 获取指定用户信息
curl http://localhost:8080/api/user/info?user_id=1 \
  -H "Authorization: Bearer YOUR_TOKEN"
```

---

## 帖子模块

### 1. 发布帖子

**接口:** `POST /api/post/create`

**认证:** 需要 🔐

**请求参数:**

```json
{
    "title": "这是帖子标题",
    "content": "这是帖子内容，内容需要至少10个字符。"
}
```

| 字段 | 类型 | 必填 | 说明 | 限制 |
|------|------|------|------|------|
| title | string | ✅ | 帖子标题 | 1-100字符 |
| content | string | ✅ | 帖子内容 | 5-200字符 |

**成功响应:**

```json
{
    "code": 0,
    "msg": "发布成功",
    "data": {
        "post_id": 1
    }
}
```

**CURL示例:**

```bash
curl -X POST http://localhost:8080/api/post/create \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_TOKEN" \
  -d '{
    "title": "测试帖子标题",
    "content": "这是一个测试帖子的内容，内容需要至少10个字。"
  }'
```

---

### 2. 获取帖子列表

**接口:** `GET /api/post/list`

**认证:** 不需要

**请求参数:**

| 参数 | 类型 | 必填 | 说明 | 默认值 |
|------|------|------|------|--------|
| page | integer | ❌ | 页码 | 1 |
| size | integer | ❌ | 每页数量 | 20 |

**成功响应:**

```json
{
    "code": 0,
    "msg": "success",
    "data": {
        "total": 100,
        "page": 1,
        "size": 20,
        "list": [
            {
                "post_id": 1,
                "title": "帖子标题",
                "content": "帖子内容...",
                "author_id": 1,
                "author_name": "testuser",
                "view_count": 128,
                "like_count": 15,
                "reply_count": 8,
                "created_at": "2025-01-15 10:30:00"
            }
            // ... 更多帖子
        ]
    }
}
```

**字段说明:**

| 字段 | 类型 | 说明 |
|------|------|------|
| total | integer | 总帖子数 |
| page | integer | 当前页码 |
| size | integer | 每页数量 |
| list | array | 帖子列表 |
| list[].post_id | integer | 帖子ID |
| list[].title | string | 帖子标题 |
| list[].content | string | 帖子内容（前200字） |
| list[].author_id | integer | 作者ID |
| list[].author_name | string | 作者用户名 |
| list[].view_count | integer | 浏览次数 |
| list[].like_count | integer | 点赞数 |
| list[].reply_count | integer | 回复数 |
| list[].created_at | string | 创建时间 |

**CURL示例:**

```bash
curl http://localhost:8080/api/post/list?page=1&size=20
```

---

### 3. 获取帖子详情

**接口:** `GET /api/post/detail`

**认证:** 不需要

**说明:** 每次访问会自动增加浏览次数

**请求参数:**

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| id | integer | ✅ | 帖子ID |

**成功响应:**

```json
{
    "code": 0,
    "msg": "success",
    "data": {
        "post_id": 1,
        "title": "帖子标题",
        "content": "完整的帖子内容...",
        "author_id": 1,
        "author_name": "testuser",
        "view_count": 129,
        "like_count": 15,
        "reply_count": 8,
        "created_at": "2025-01-15 10:30:00",
        "replies": [
            {
                "reply_id": 1,
                "content": "回复内容",
                "user_id": 2,
                "username": "user2",
                "created_at": "2025-01-15 11:00:00"
            }
            // ... 更多回复
        ]
    }
}
```

**错误响应:**

```json
{
    "code": 1007,
    "msg": "帖子不存在",
    "data": null
}
```

**CURL示例:**

```bash
curl http://localhost:8080/api/post/detail?id=1
```

---

### 4. 删除帖子

**接口:** `DELETE /api/post/delete`

**认证:** 需要 🔐

**权限:** 只能删除自己发布的帖子

**请求参数:**

```json
{
    "post_id": 1
}
```

| 字段 | 类型 | 必填 | 说明 |
|------|------|------|------|
| post_id | integer | ✅ | 帖子ID |

**成功响应:**

```json
{
    "code": 0,
    "msg": "删除成功",
    "data": null
}
```

**错误响应:**

```json
// 帖子不存在
{
    "code": 1007,
    "msg": "帖子不存在",
    "data": null
}

// 无权限
{
    "code": 1006,
    "msg": "无权限操作",
    "data": null
}
```

**CURL示例:**

```bash
curl -X DELETE http://localhost:8080/api/post/delete \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_TOKEN" \
  -d '{
    "post_id": 1
  }'
```

---

## 回复模块

### 1. 发布回复

**接口:** `POST /api/reply/create`

**认证:** 需要 🔐

**请求参数:**

```json
{
    "post_id": 1,
    "content": "这是回复内容"
}
```

| 字段 | 类型 | 必填 | 说明 | 限制 |
|------|------|------|------|------|
| post_id | integer | ✅ | 帖子ID | 必须存在 |
| content | string | ✅ | 回复内容 | 1-1000字符 |

**成功响应:**

```json
{
    "code": 0,
    "msg": "回复成功",
    "data": {
        "reply_id": 1,
        "post_id": 1,
        "content": "这是回复内容",
        "created_at": "2025-01-15 12:00:00"
    }
}
```

**错误响应:**

```json
{
    "code": 1007,
    "msg": "帖子不存在",
    "data": null
}
```

**CURL示例:**

```bash
curl -X POST http://localhost:8080/api/reply/create \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_TOKEN" \
  -d '{
    "post_id": 1,
    "content": "这是我的回复内容"
  }'
```

---

### 2. 删除回复

**接口:** `DELETE /api/reply/delete`

**认证:** 需要 🔐

**权限:** 只能删除自己的回复

**请求参数:**

```json
{
    "reply_id": 1
}
```

| 字段 | 类型 | 必填 | 说明 |
|------|------|------|------|
| reply_id | integer | ✅ | 回复ID |

**成功响应:**

```json
{
    "code": 0,
    "msg": "删除成功",
    "data": null
}
```

**错误响应:**

```json
// 回复不存在
{
    "code": 1008,
    "msg": "回复不存在",
    "data": null
}

// 无权限
{
    "code": 1006,
    "msg": "无权限操作",
    "data": null
}
```

**CURL示例:**

```bash
curl -X DELETE http://localhost:8080/api/reply/delete \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_TOKEN" \
  -d '{
    "reply_id": 1
  }'
```

---

## 点赞模块

### 点赞/取消点赞

**接口:** `POST /api/like/toggle`

**认证:** 需要 🔐

**说明:** 同一个接口实现点赞和取消点赞的切换

**请求参数:**

```json
{
    "post_id": 1
}
```

| 字段 | 类型 | 必填 | 说明 |
|------|------|------|------|
| post_id | integer | ✅ | 帖子ID |

**成功响应:**

```json
{
    "code": 0,
    "msg": "点赞成功",  // 或 "取消点赞成功"
    "data": {
        "liked": true,      // true表示已点赞，false表示未点赞
        "like_count": 16    // 当前帖子的点赞总数
    }
}
```

**字段说明:**

| 字段 | 类型 | 说明 |
|------|------|------|
| liked | boolean | 当前用户是否已点赞该帖子 |
| like_count | integer | 帖子的点赞总数 |

**错误响应:**

```json
{
    "code": 1007,
    "msg": "帖子不存在",
    "data": null
}
```

**CURL示例:**

```bash
curl -X POST http://localhost:8080/api/like/toggle \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer YOUR_TOKEN" \
  -d '{
    "post_id": 1
  }'
```

**行为说明:**
- 如果用户未点赞，调用后会添加点赞
- 如果用户已点赞，调用后会取消点赞
- 支持并发点击（使用事务保证数据一致性）

---

## 错误码说明

### 错误码列表

| 错误码 | 说明 | HTTP状态码 |
|-------|------|-----------|
| 0 | 成功 | 200 |
| 1001 | 参数错误 | 200 |
| 1002 | 用户名已存在 | 200 |
| 1003 | 用户不存在 | 200 |
| 1004 | 密码错误 | 200 |
| 1005 | Token无效或过期 | 200 |
| 1006 | 无权限操作 | 200 |
| 1007 | 帖子不存在 | 200 |
| 1008 | 回复不存在 | 200 |
| 1009 | 数据库错误 | 200 |
| 1010 | 服务器内部错误 | 200 |

### 错误ID系统

部分错误响应会包含 `error_id` 字段，格式为 `ERR-{timestamp}-{random}`：

```json
{
    "code": 1009,
    "msg": "数据库错误",
    "error_id": "ERR-1700000000-A3F2",
    "data": null
}
```

**用途:**
- 用户可以提供 `error_id` 给技术支持
- 技术人员通过 `error_id` 在日志中快速定位问题
- 保护敏感信息不泄露给用户

**查找日志:**
```bash
grep "ERR-1700000000-A3F2" /var/log/college-bbs/*.log
```

---

## 附录

### 数据类型说明

| 类型 | 说明 | 示例 |
|------|------|------|
| integer | 整数 | 1, 100, -5 |
| string | 字符串 | "hello", "测试" |
| boolean | 布尔值 | true, false |
| array | 数组 | [1, 2, 3] |
| object | 对象 | {"key": "value"} |

### Token获取和使用

1. **获取Token:**
   - 调用登录接口 `/api/user/login`
   - 从响应的 `data.token` 字段获取

2. **使用Token:**
   - 在请求头中添加: `Authorization: Bearer {token}`
   - Token有效期为24小时

3. **Token过期:**
   - 错误码: 1005
   - 需要重新登录获取新Token

### 测试工具推荐

- **Postman** - 可视化API测试工具
- **curl** - 命令行HTTP客户端
- **httpie** - 用户友好的HTTP客户端
- **Insomnia** - REST API客户端

---

**文档版本:** 1.0
**最后更新:** 2025-11-16
**API版本:** v1.0

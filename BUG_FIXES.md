# Bug修复报告

## 修复日期
2025-11-16

## 修复的Bug列表

### 1. 🔴 严重Bug - 控制器头文件包含错误
**位置**: 所有Controller头文件 (UserController.h, PostController.h, ReplyController.h, LikeController.h)

**问题描述**:
```cpp
#include <drogon/HttpSimpleController.h>  // 错误的头文件
```
所有控制器都包含了错误的头文件。控制器继承的是`HttpController`，应该包含`<drogon/HttpController.h>`而不是`<drogon/HttpSimpleController.h>`。

**修复方案**:
```cpp
#include <drogon/HttpController.h>  // 正确的头文件
```

**影响**: 编译错误或未定义行为
**严重程度**: 🔴 严重

---

### 2. 🔴 严重Bug - AuthFilter substr越界访问
**位置**: `filters/AuthFilter.cc` 第20行

**问题描述**:
```cpp
if (authHeader.substr(0, 7) == "Bearer ") {  // 如果authHeader长度<7会出错
```
如果HTTP请求头中的Authorization字段长度小于7个字符，`substr(0, 7)`会导致越界访问或抛出异常。

**修复方案**:
```cpp
if (authHeader.length() >= 7 && authHeader.substr(0, 7) == "Bearer ") {
```

**影响**: 程序崩溃
**严重程度**: 🔴 严重

---

### 3. 🔴 严重Bug - JwtUtil HMAC内存管理问题
**位置**: `utils/JwtUtil.cc` 第135-144行

**问题描述**:
```cpp
std::string JwtUtil::hmacSha256(const std::string& key, const std::string& data) {
    unsigned char* digest;  // 未初始化的指针
    unsigned int digest_len;

    digest = HMAC(..., nullptr, &digest_len);  // 传入nullptr不安全
    return std::string(reinterpret_cast<char*>(digest), digest_len);
}
```
HMAC函数如果第5个参数传入nullptr，行为不明确，可能导致内存问题。

**修复方案**:
```cpp
std::string JwtUtil::hmacSha256(const std::string& key, const std::string& data) {
    unsigned char digest[EVP_MAX_MD_SIZE];  // 提供固定缓冲区
    unsigned int digest_len;

    HMAC(EVP_sha256(),
         key.c_str(), key.length(),
         reinterpret_cast<const unsigned char*>(data.c_str()), data.length(),
         digest, &digest_len);  // 传入缓冲区地址

    return std::string(reinterpret_cast<char*>(digest), digest_len);
}
```

**影响**: 潜在的内存问题和JWT签名错误
**严重程度**: 🔴 严重

---

### 4. 🔴 严重Bug - Base64解码错误处理
**位置**: `utils/JwtUtil.cc` 第75、92行

**问题描述**:
```cpp
for (i = 0; i < 4; i++)
    char_array_4[i] = base64_chars.find(char_array_4[i]);  // find可能返回npos
```
如果`find()`找不到字符，会返回`std::string::npos`（通常是一个非常大的值），导致后续位运算出现错误结果。

**修复方案**:
```cpp
for (i = 0; i < 4; i++) {
    size_t pos = base64_chars.find(char_array_4[i]);
    if (pos == std::string::npos) {
        return "";  // 或者做其他错误处理
    }
    char_array_4[i] = pos;
}
```

**影响**: JWT Token验证失败，用户无法登录
**严重程度**: 🔴 严重

---

### 5. 🟡 中等Bug - PostController错误提示不一致
**位置**: `controllers/PostController.cc` 第30-33行

**问题描述**:
```cpp
// 验证标题长度（5-100字）  ← 注释说5-100
if (title.length() < 5 || title.length() > 200) {  ← 代码是5-200
    callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR,
             "标题长度必须在5-100字之间"));  ← 提示说5-100
```
注释、代码和错误提示三者不一致，容易造成用户混淆。

**修复方案**:
```cpp
// 验证标题长度（5-200字符）
if (title.length() < 5 || title.length() > 200) {
    callback(ResponseUtil::error(ResponseUtil::PARAM_ERROR,
             "标题长度必须在5-200字符之间"));
```

**影响**: 用户体验问题，错误提示误导用户
**严重程度**: 🟡 中等

---

### 6. 🟢 配置问题 - config.json无效配置项
**位置**: `college-bbs/config.json` 第26-31行

**问题描述**:
```json
"simple_controllers_map": [
    {
        "path_pattern": "/api/*",
        "controller": "api::v1::*"
    }
]
```
配置中包含`simple_controllers_map`，但项目中使用的是`HttpController`而不是`SimpleController`，这个配置项无效且会引起混淆。

**修复方案**:
移除整个`simple_controllers_map`配置项。所有路由已经在Controller类中通过`ADD_METHOD_TO`宏手动注册。

**影响**: 配置文件混淆，但不影响功能
**严重程度**: 🟢 轻微

---

## 修复总结

- **修复文件数**: 9个
  - 4个Controller头文件
  - 1个Filter实现文件
  - 1个Util实现文件
  - 1个Controller实现文件
  - 1个配置文件

- **严重Bug**: 4个 🔴
- **中等Bug**: 1个 🟡
- **轻微问题**: 1个 🟢

## 测试建议

修复这些bug后，建议进行以下测试：

1. **编译测试**: 确保项目能够成功编译
2. **JWT测试**: 测试用户注册、登录功能，验证Token生成和验证是否正常
3. **AuthFilter测试**: 测试各种格式的Authorization头（包括短字符串）
4. **API测试**: 测试所有API端点是否能正常访问和响应

## 影响范围

所有修复都是向后兼容的，不会影响现有的API接口和数据库。建议尽快合并这些修复到主分支。

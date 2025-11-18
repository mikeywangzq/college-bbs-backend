# 计算机学院贴吧系统 - 前端项目完整文档

## 📋 目录

- [项目概述](#项目概述)
- [技术栈](#技术栈)
- [功能特性](#功能特性)
- [项目结构](#项目结构)
- [快速开始](#快速开始)
- [开发指南](#开发指南)
- [API对接说明](#api对接说明)
- [代码质量](#代码质量)
- [Bug修复记录](#bug修复记录)
- [部署指南](#部署指南)
- [常见问题](#常见问题)

---

## 项目概述

### 基本信息
- **项目名称**：计算机学院贴吧系统 - 前端
- **版本**：V1.0
- **开发时间**：2024年11月
- **开发团队**：College BBS Team
- **项目描述**：基于React 18 + Vite + Ant Design 5构建的现代化贴吧系统前端应用

### 项目目标
为计算机学院师生提供一个在线交流平台，方便大家讨论课程问题、分享学习资料、交流校园生活。

---

## 技术栈

### 核心框架
- **React 18** - 前端框架
- **Vite** - 构建工具（快速开发和热更新）
- **React Router 6** - 路由管理

### UI组件库
- **Ant Design 5** - 企业级UI组件库
- **Ant Design Icons** - 图标库

### 状态管理
- **React Hooks** - 使用useState、useEffect、useCallback等

### 网络请求
- **Axios** - HTTP客户端
- 自动添加JWT Token
- 统一错误处理
- 请求/响应拦截器

### 代码质量
- **ESLint** - 代码检查
- **JSDoc** - 代码注释规范

---

## 功能特性

### V1.0 核心功能

#### 用户模块
- ✅ 用户注册（用户名、邮箱、密码）
- ✅ 用户登录（JWT Token认证）
- ✅ 用户登出（清除本地认证信息）
- ✅ 查看个人信息（发帖数、回复数统计）

#### 帖子模块
- ✅ 发布帖子（5-100字标题，10-10000字内容）
- ✅ 查看帖子列表（分页显示，每页20条）
- ✅ 查看帖子详情（完整内容、回复列表）
- ✅ 删除自己的帖子（权限验证）
- ✅ 浏览数自动增加

#### 回复模块
- ✅ 回复帖子（1-1000字）
- ✅ 删除自己的回复
- ✅ 回复数自动更新

#### 点赞模块
- ✅ 点赞/取消点赞（Toggle机制）
- ✅ 实时显示点赞状态
- ✅ 点赞数自动更新

### UI/UX特性
- ✅ 响应式布局
- ✅ Loading加载状态
- ✅ Toast消息提示
- ✅ 表单实时验证
- ✅ 确认对话框（删除操作）
- ✅ 分页组件
- ✅ 空状态提示
- ✅ 乐观UI更新（点赞功能）

### 安全特性
- ✅ JWT Token自动携带
- ✅ Token过期自动跳转登录
- ✅ 受保护路由（未登录自动跳转）
- ✅ 表单输入验证（防注入）
- ✅ XSS防护（React默认转义）

---

## 项目结构

```
college-bbs-frontend/
├── public/                     # 静态资源
│   └── vite.svg
├── src/
│   ├── components/             # 共享组件（4个）
│   │   ├── Header.jsx          # 导航栏（登录状态、用户菜单）
│   │   ├── PostCard.jsx        # 帖子卡片（列表项）
│   │   ├── ReplyItem.jsx       # 回复项（权限控制）
│   │   └── ProtectedRoute.jsx  # 路由保护（需要登录）
│   ├── pages/                  # 页面组件（6个）
│   │   ├── Login.jsx           # 登录页（表单验证）
│   │   ├── Register.jsx        # 注册页（密码确认）
│   │   ├── Home.jsx            # 首页（帖子列表+分页）
│   │   ├── PostDetail.jsx      # 帖子详情（点赞+回复）
│   │   ├── CreatePost.jsx      # 发帖页（长度验证）
│   │   └── Profile.jsx         # 个人中心（统计数据）
│   ├── services/               # API服务层（5个）
│   │   ├── api.js              # Axios配置（拦截器）
│   │   ├── userService.js      # 用户API（注册/登录/信息）
│   │   ├── postService.js      # 帖子API（CRUD）
│   │   ├── replyService.js     # 回复API（创建/删除）
│   │   └── likeService.js      # 点赞API（切换）
│   ├── utils/                  # 工具函数（1个）
│   │   └── auth.js             # Token管理（localStorage）
│   ├── App.jsx                 # 主应用（路由配置）
│   ├── App.css                 # 全局样式
│   ├── main.jsx                # 入口文件（Ant Design配置）
│   └── index.css               # 基础样式
├── .env                        # 环境变量（开发）
├── .env.production             # 环境变量（生产）
├── .gitignore                  # Git忽略配置
├── index.html                  # HTML模板
├── package.json                # 项目配置
├── vite.config.js              # Vite配置
├── README.md                   # 项目说明
├── QUICKSTART.md               # 快速开始指南
├── DEPLOYMENT.md               # 部署指南
└── PROJECT_DOCUMENTATION.md    # 本文档
```

### 组件说明

#### 共享组件
1. **Header.jsx** - 导航栏
   - 显示Logo和网站名称
   - 登录/未登录状态切换
   - 用户下拉菜单（个人中心、退出登录）
   - 导航菜单（首页、发帖）

2. **PostCard.jsx** - 帖子卡片
   - 显示标题、作者、时间
   - 显示浏览数、回复数、点赞数
   - 点击跳转到详情页

3. **ReplyItem.jsx** - 回复项
   - 显示回复内容、作者、时间
   - 删除按钮（仅作者可见）

4. **ProtectedRoute.jsx** - 路由保护
   - 检查登录状态
   - 未登录跳转到登录页

#### 页面组件
1. **Login.jsx** - 登录页
   - 用户名/密码表单
   - 表单验证
   - 跳转到注册页链接

2. **Register.jsx** - 注册页
   - 用户名/邮箱/密码/确认密码
   - 实时表单验证
   - 密码一致性检查

3. **Home.jsx** - 首页
   - 帖子列表（PostCard组件）
   - 分页组件
   - Loading状态

4. **PostDetail.jsx** - 帖子详情
   - 帖子完整内容
   - 点赞按钮（乐观更新）
   - 回复列表
   - 回复输入框
   - 删除帖子按钮（仅作者）

5. **CreatePost.jsx** - 发帖页
   - 标题输入（5-100字）
   - 内容输入（10-10000字）
   - 字数统计

6. **Profile.jsx** - 个人中心
   - 用户基本信息
   - 发帖数/回复数统计

---

## 快速开始

### 前置要求
- Node.js >= 16.0.0
- npm >= 8.0.0
- 后端服务运行在 `http://localhost:8080`

### 安装步骤

1. **克隆项目**
```bash
cd college-bbs-backend/college-bbs-frontend
```

2. **安装依赖**
```bash
npm install
```

3. **配置环境变量**

编辑 `.env` 文件：
```bash
VITE_API_BASE_URL=http://localhost:8080
```

4. **启动开发服务器**
```bash
npm run dev
```

5. **访问应用**
```
浏览器打开 http://localhost:5173
```

### 测试账号
- 用户名：`zhangsan` / 密码：`123456`
- 用户名：`lisi` / 密码：`123456`

或注册新账号测试。

---

## 开发指南

### 开发环境运行
```bash
npm run dev
```
- 开发服务器运行在 `http://localhost:5173`
- 支持热更新（HMR）
- 自动打开浏览器

### 生产环境构建
```bash
npm run build
```
- 构建产物输出到 `dist/` 目录
- 自动压缩和优化
- 可直接部署到静态服务器

### 预览生产构建
```bash
npm run preview
```

### 代码检查
```bash
npm run lint
```

### 代码规范

#### 命名规范
- 组件文件：PascalCase（如：PostCard.jsx）
- 工具函数：camelCase（如：auth.js）
- 常量：UPPER_SNAKE_CASE（如：API_BASE_URL）

#### 注释规范
所有文件都使用JSDoc格式注释：
```javascript
/**
 * @file PostCard.jsx
 * @brief 帖子卡片组件
 * @details 在帖子列表中显示单个帖子的摘要信息
 */

/**
 * @brief 处理点赞操作
 * @param {number} postId - 帖子ID
 * @return {Promise<void>}
 */
const handleLike = async (postId) => {
  // ...
};
```

---

## API对接说明

### 后端API地址
- 开发环境：`http://localhost:8080`
- 生产环境：在 `.env.production` 中配置

### 统一响应格式
```json
{
  "code": 0,           // 0表示成功，非0表示失败
  "msg": "success",    // 消息描述
  "data": {...}        // 业务数据
}
```

### API列表

#### 用户API（3个）
| 方法 | 路径 | 说明 | 需要Token |
|------|------|------|----------|
| POST | `/user/register` | 用户注册 | ❌ |
| POST | `/user/login` | 用户登录 | ❌ |
| GET | `/user/info` | 获取用户信息 | ✅ |

#### 帖子API（4个）
| 方法 | 路径 | 说明 | 需要Token |
|------|------|------|----------|
| GET | `/post/list` | 获取帖子列表 | ❌ |
| GET | `/post/detail` | 获取帖子详情 | ❌ |
| POST | `/post/create` | 创建帖子 | ✅ |
| DELETE | `/post/delete` | 删除帖子 | ✅ |

#### 回复API（2个）
| 方法 | 路径 | 说明 | 需要Token |
|------|------|------|----------|
| POST | `/reply/create` | 创建回复 | ✅ |
| DELETE | `/reply/delete` | 删除回复 | ✅ |

#### 点赞API（1个）
| 方法 | 路径 | 说明 | 需要Token |
|------|------|------|----------|
| POST | `/like/toggle` | 切换点赞状态 | ✅ |

### Token管理

#### 存储方式
Token存储在 `localStorage`：
```javascript
localStorage.setItem('college_bbs_token', token);
```

#### 自动携带
Axios拦截器自动添加到请求头：
```javascript
Authorization: Bearer {token}
```

#### 过期处理
- Token过期时，后端返回401
- 前端自动清除Token并跳转登录页

---

## 代码质量

### 代码统计
- **组件总数**：10个（4个共享 + 6个页面）
- **API服务**：5个文件，13个API方法
- **工具函数**：1个文件，8个函数
- **路由数量**：6个路由
- **代码行数**：约2000行（不含注释）
- **注释行数**：约800行

### 代码注释覆盖率
- ✅ 所有文件都有文件级注释
- ✅ 所有函数都有函数级注释
- ✅ 关键逻辑都有行内注释
- ✅ 覆盖率：100%

### 注释示例
```javascript
/**
 * @file PostDetail.jsx
 * @brief 帖子详情页组件
 * @details
 * 功能列表：
 * - 显示帖子完整内容
 * - 显示回复列表
 * - 支持点赞/回复
 * - 支持删除操作
 *
 * @author College BBS Team
 * @date 2024-11-17
 * @version 1.1
 */
```

---

## Bug修复记录

### 修复的Bug（共9个）

#### 🔴 严重Bug（3个）

1. **内存泄漏** - PostDetail.jsx
   - **问题**：组件卸载后异步setState
   - **修复**：添加isMountedRef + useEffect清理函数
   - **影响**：防止内存泄漏和React警告

2. **401重复跳转** - api.js
   - **问题**：登录页收到401时重复跳转
   - **修复**：检查当前路径，避免重复跳转
   - **影响**：防止死循环

3. **点赞竞态条件** - PostDetail.jsx
   - **问题**：快速点击导致并发请求
   - **修复**：添加liking状态 + 乐观UI更新
   - **影响**：确保点赞数准确

#### 🟡 中等Bug（3个）

4. **响应格式验证缺失** - api.js
   - **修复**：添加code字段验证

5. **ID类型不一致** - PostDetail.jsx
   - **修复**：使用parseInt确保数字类型

6. **空数组默认值缺失** - PostDetail.jsx
   - **修复**：提供默认空数组

#### 🟢 轻微Bug（3个）

7. **useEffect依赖警告** - 使用useCallback
8. **请求取消未处理** - 添加isCancel检查
9. **超时错误细化** - 区分超时和网络错误

详细信息见：`BUG_FIXES_SUMMARY.md`

---

## 部署指南

### 方法一：Nginx静态部署

1. **构建生产版本**
```bash
npm run build
```

2. **配置Nginx**
```nginx
server {
    listen 80;
    server_name your-domain.com;
    root /var/www/college-bbs-frontend/dist;
    index index.html;

    location / {
        try_files $uri $uri/ /index.html;
    }

    location /api {
        proxy_pass http://localhost:8080;
    }
}
```

### 方法二：Docker部署

1. **创建Dockerfile**
```dockerfile
FROM node:18-alpine as build
WORKDIR /app
COPY package*.json ./
RUN npm install
COPY . .
RUN npm run build

FROM nginx:alpine
COPY --from=build /app/dist /usr/share/nginx/html
EXPOSE 80
CMD ["nginx", "-g", "daemon off;"]
```

2. **构建并运行**
```bash
docker build -t college-bbs-frontend .
docker run -d -p 80:80 college-bbs-frontend
```

详细部署指南见：`DEPLOYMENT.md`

---

## 常见问题

### Q: 启动后页面空白？
A: 检查浏览器控制台错误，确认 `.env` 中的API地址配置正确。

### Q: 登录后刷新页面会退出吗？
A: 不会。Token存储在localStorage中，刷新页面不会丢失。

### Q: API请求失败？
A: 确认后端服务是否运行，检查CORS配置，查看Network标签。

### Q: 点赞后刷新页面状态丢失？
A: 这是已知限制。当前后端未返回user_liked字段，刷新后点赞状态会重置。

### Q: 如何添加新页面？
A: 
1. 在 `src/pages/` 创建新组件
2. 在 `src/App.jsx` 添加路由
3. 如需要登录，用 `<ProtectedRoute>` 包裹

---

## 开发团队

- **前端开发**：College BBS Team
- **项目开始**：2024年11月
- **当前版本**：V1.0

## 许可证

MIT License

---

## 更新日志

### V1.0 (2024-11-17)
- ✅ 完成核心功能开发
- ✅ 实现用户认证系统
- ✅ 实现帖子CRUD功能
- ✅ 实现回复和点赞功能
- ✅ 完成API对接
- ✅ 修复9个潜在bug
- ✅ 添加详细代码注释

---

**文档版本**：1.0  
**最后更新**：2024-11-17  
**维护者**：College BBS Team

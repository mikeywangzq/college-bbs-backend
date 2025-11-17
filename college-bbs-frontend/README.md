# 计算机学院贴吧系统 - 前端项目

## 项目简介

计算机学院贴吧系统的前端项目，基于 React 18 + Vite + Ant Design 5 构建的现代化 Web 应用。

## 技术栈

- **框架**: React 18
- **构建工具**: Vite
- **UI 组件库**: Ant Design 5
- **路由**: React Router 6
- **HTTP 客户端**: Axios
- **状态管理**: React Hooks
- **样式**: CSS + Ant Design

## 功能特性

### V1.0 核心功能

- ✅ 用户注册、登录、登出
- ✅ 查看个人信息
- ✅ 发布帖子
- ✅ 查看帖子列表（分页）
- ✅ 查看帖子详情
- ✅ 删除自己的帖子
- ✅ 回复帖子
- ✅ 删除自己的回复
- ✅ 点赞/取消点赞帖子

## 项目结构

```
college-bbs-frontend/
├── src/
│   ├── components/         # 共享组件
│   │   ├── Header.jsx      # 导航栏
│   │   ├── PostCard.jsx    # 帖子卡片
│   │   ├── ReplyItem.jsx   # 回复项
│   │   └── ProtectedRoute.jsx # 受保护路由
│   ├── pages/              # 页面组件
│   │   ├── Login.jsx       # 登录页
│   │   ├── Register.jsx    # 注册页
│   │   ├── Home.jsx        # 首页（帖子列表）
│   │   ├── PostDetail.jsx  # 帖子详情
│   │   ├── CreatePost.jsx  # 发帖页
│   │   └── Profile.jsx     # 个人中心
│   ├── services/           # API 服务
│   │   ├── api.js          # Axios 配置
│   │   ├── userService.js  # 用户 API
│   │   ├── postService.js  # 帖子 API
│   │   ├── replyService.js # 回复 API
│   │   └── likeService.js  # 点赞 API
│   ├── utils/              # 工具函数
│   │   └── auth.js         # Token 管理
│   ├── App.jsx             # 主应用组件
│   └── main.jsx            # 入口文件
└── package.json            # 项目配置
```

## 开始使用

### 前置要求

- Node.js >= 16.0.0
- npm >= 8.0.0

### 安装依赖

```bash
cd college-bbs-frontend
npm install
```

### 开发环境运行

```bash
npm run dev
```

访问 http://localhost:5173

### 生产环境构建

```bash
npm run build
```

## 环境配置

修改 `.env` 文件配置后端 API 地址：

```bash
VITE_API_BASE_URL=http://localhost:8080
```

## API 接口对接

所有 API 请求通过 Axios 统一处理，自动添加 JWT Token。

## 路由说明

| 路径 | 组件 | 说明 | 需要登录 |
|------|------|------|---------|
| `/` | Home | 首页 | ❌ |
| `/login` | Login | 登录页 | ❌ |
| `/register` | Register | 注册页 | ❌ |
| `/post/:id` | PostDetail | 帖子详情 | ❌ |
| `/create-post` | CreatePost | 发帖页 | ✅ |
| `/profile` | Profile | 个人中心 | ✅ |

## 后端对接说明

确保后端服务运行在 `http://localhost:8080`，并已配置 CORS 允许前端访问。

后端 API 文档详见：`../docs/API.md`

## 开发指南

1. 克隆项目
2. 安装依赖：`npm install`
3. 启动后端服务（端口 8080）
4. 启动前端开发服务器：`npm run dev`
5. 浏览器访问：`http://localhost:5173`

## 联系方式

项目地址：https://github.com/yourname/college-bbs-backend

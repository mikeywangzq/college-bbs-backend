/**
 * @file App.jsx
 * @brief 主应用组件 - 配置路由和全局布局
 * @details
 * 这是应用的根组件，负责：
 * - 配置React Router路由
 * - 区分需要Header的页面和不需要的页面（登录/注册）
 * - 设置受保护路由（需要登录的页面）
 * - 提供全局Layout布局
 *
 * @author College BBS Team
 * @date 2024-11-17
 * @version 1.0
 */

import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import { Layout } from 'antd';
import Header from './components/Header';
import ProtectedRoute from './components/ProtectedRoute';
import Home from './pages/Home';
import Login from './pages/Login';
import Register from './pages/Register';
import PostDetail from './pages/PostDetail';
import CreatePost from './pages/CreatePost';
import Profile from './pages/Profile';
import './App.css';

/**
 * @brief 主应用组件
 * @details
 * 路由结构：
 * - /login, /register: 不需要Header的独立页面
 * - 其他页面: 包含Header的布局
 *
 * 受保护路由（需要登录）：
 * - /create-post: 发帖页
 * - /profile: 个人中心
 *
 * 公开路由（无需登录）：
 * - /: 首页
 * - /post/:id: 帖子详情
 *
 * @return {JSX.Element} 应用根组件
 */
function App() {
  return (
    <Router>
      <Layout style={{ minHeight: '100vh' }}>
        <Routes>
          {/* 登录和注册页面 - 不需要Header */}
          <Route path="/login" element={<Login />} />
          <Route path="/register" element={<Register />} />

          {/* 其他页面 - 需要Header */}
          <Route
            path="/*"
            element={
              <>
                <Header />
                <Routes>
                  {/* 公开路由 - 无需登录 */}
                  <Route path="/" element={<Home />} />
                  <Route path="/post/:id" element={<PostDetail />} />

                  {/* 受保护路由 - 需要登录 */}
                  <Route
                    path="/create-post"
                    element={
                      <ProtectedRoute>
                        <CreatePost />
                      </ProtectedRoute>
                    }
                  />
                  <Route
                    path="/profile"
                    element={
                      <ProtectedRoute>
                        <Profile />
                      </ProtectedRoute>
                    }
                  />
                </Routes>
              </>
            }
          />
        </Routes>
      </Layout>
    </Router>
  );
}

export default App;

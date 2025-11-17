/**
 * @file ProtectedRoute.jsx
 * @brief 受保护路由组件
 * @details 需要登录才能访问的路由，未登录则跳转到登录页
 */

import { Navigate } from 'react-router-dom';
import { isAuthenticated } from '../utils/auth';

const ProtectedRoute = ({ children }) => {
  if (!isAuthenticated()) {
    // 未登录，重定向到登录页
    return <Navigate to="/login" replace />;
  }

  // 已登录，渲染子组件
  return children;
};

export default ProtectedRoute;

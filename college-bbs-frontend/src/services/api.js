/**
 * @file api.js
 * @brief Axios配置和请求拦截器
 * @details 统一处理请求和响应，自动添加Token，统一错误处理
 */

import axios from 'axios';
import { message } from 'antd';
import { getToken, clearAuth } from '../utils/auth';

// API Base URL - 开发环境
const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080';

/**
 * @brief 创建Axios实例
 */
const apiClient = axios.create({
  baseURL: API_BASE_URL,
  timeout: 10000,
  headers: {
    'Content-Type': 'application/json',
  },
});

/**
 * @brief 请求拦截器
 * @details 自动添加Authorization头
 */
apiClient.interceptors.request.use(
  (config) => {
    const token = getToken();
    if (token) {
      config.headers.Authorization = `Bearer ${token}`;
    }
    return config;
  },
  (error) => {
    return Promise.reject(error);
  }
);

/**
 * @brief 响应拦截器
 * @details 统一处理响应和错误
 */
apiClient.interceptors.response.use(
  (response) => {
    // 后端统一返回格式: { code, msg, data }
    const { code, msg, data } = response.data;

    if (code === 0) {
      // 成功响应
      return data;
    } else {
      // 业务错误
      message.error(msg || '请求失败');
      return Promise.reject(new Error(msg || '请求失败'));
    }
  },
  (error) => {
    // HTTP错误
    if (error.response) {
      const { status } = error.response;

      switch (status) {
        case 401:
          message.error('未授权，请重新登录');
          clearAuth();
          window.location.href = '/login';
          break;
        case 403:
          message.error('无权限访问');
          break;
        case 404:
          message.error('请求的资源不存在');
          break;
        case 500:
          message.error('服务器内部错误');
          break;
        default:
          message.error(error.response.data?.msg || '请求失败');
      }
    } else if (error.request) {
      message.error('网络错误，请检查网络连接');
    } else {
      message.error(error.message || '请求失败');
    }

    return Promise.reject(error);
  }
);

export default apiClient;

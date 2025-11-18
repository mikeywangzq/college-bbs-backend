/**
 * @file api.js
 * @brief Axios配置和请求拦截器
 * @details
 * 统一HTTP请求处理模块，提供以下功能：
 * - 自动添加JWT Token到请求头
 * - 统一处理响应数据格式
 * - 统一错误处理和用户提示
 * - 自动处理401未授权（跳转登录）
 * - 网络错误友好提示
 *
 * 后端响应格式约定：
 * {
 *   code: 0,           // 0表示成功，非0表示失败
 *   msg: "success",    // 消息描述
 *   data: {...}        // 业务数据
 * }
 *
 * @author College BBS Team
 * @date 2024-11-17
 * @version 1.0
 */

import axios from 'axios';
import { message } from 'antd';
import { getToken, clearAuth } from '../utils/auth';

/**
 * API Base URL
 * @description 从环境变量读取，如果未设置则使用默认值
 * 开发环境: http://localhost:8080
 * 生产环境: 在.env.production中配置
 */
const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080';

/**
 * @brief 创建Axios实例
 * @details
 * 配置说明：
 * - baseURL: API基础地址，所有请求都会拼接此地址
 * - timeout: 请求超时时间（10秒），超时后会自动取消请求
 * - headers: 默认请求头，设置Content-Type为JSON格式
 */
const apiClient = axios.create({
  baseURL: API_BASE_URL,
  timeout: 10000, // 10秒超时
  headers: {
    'Content-Type': 'application/json',
  },
});

/**
 * @brief 请求拦截器
 * @details
 * 在每个请求发送前执行：
 * 1. 从localStorage读取JWT Token
 * 2. 如果Token存在，自动添加到Authorization请求头
 * 3. Token格式: "Bearer {token}"
 *
 * @param {Object} config - Axios请求配置对象
 * @return {Object} 修改后的配置对象
 *
 * @example
 * // Token会自动添加到请求头，无需手动设置
 * // Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
 *
 * @note 如果用户未登录（无Token），请求头不会包含Authorization字段
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
    // 请求配置错误时的处理
    console.error('请求配置错误:', error);
    return Promise.reject(error);
  }
);

/**
 * @brief 响应拦截器
 * @details
 * 在每个响应返回后执行，分为两种情况：
 *
 * 1. 成功响应（2xx状态码）：
 *    - 解析后端统一格式 { code, msg, data }
 *    - code === 0: 业务成功，返回data
 *    - code !== 0: 业务失败，显示错误消息并reject
 *
 * 2. 失败响应（非2xx状态码）：
 *    - 401: Token过期或无效，清除认证信息并跳转登录
 *    - 403: 无权限访问
 *    - 404: 资源不存在
 *    - 500: 服务器错误
 *    - 网络错误: 无响应（断网、超时等）
 *
 * @param {Object} response - Axios响应对象
 * @return {Promise} 成功返回data，失败返回rejected promise
 *
 * @note 所有错误都会通过Ant Design的message组件显示给用户
 * @note 401错误会自动清除本地认证信息并跳转到登录页
 *
 * @example 成功响应
 * // 后端返回: { code: 0, msg: "success", data: { user_id: 1, username: "test" } }
 * // 拦截器返回: { user_id: 1, username: "test" }
 *
 * @example 业务错误
 * // 后端返回: { code: 1002, msg: "用户名已存在", data: null }
 * // 拦截器: 显示错误消息 "用户名已存在"，并reject
 */
apiClient.interceptors.response.use(
  (response) => {
    // 后端统一返回格式: { code, msg, data }
    const { code, msg, data } = response.data || {};

    // BUG FIX: 检查响应数据格式是否正确
    if (code === undefined) {
      console.error('后端响应格式错误，缺少code字段:', response.data);
      message.error('服务器响应格式错误');
      return Promise.reject(new Error('响应格式错误'));
    }

    if (code === 0) {
      // 成功响应，返回业务数据
      return data;
    } else {
      // 业务错误，显示错误消息
      const errorMsg = msg || '请求失败';
      message.error(errorMsg);
      return Promise.reject(new Error(errorMsg));
    }
  },
  (error) => {
    // BUG FIX: 处理请求被取消的情况（用户主动取消或超时）
    if (axios.isCancel(error)) {
      console.log('请求已取消:', error.message);
      return Promise.reject(error);
    }

    // HTTP错误或网络错误
    if (error.response) {
      // 服务器返回了错误状态码（4xx, 5xx）
      const { status } = error.response;

      switch (status) {
        case 401:
          // 未授权：Token无效或过期
          message.error('登录已过期，请重新登录');
          clearAuth();
          // BUG FIX: 避免重复跳转，检查当前是否已在登录页
          if (window.location.pathname !== '/login') {
            window.location.href = '/login';
          }
          break;
        case 403:
          // 禁止访问：权限不足
          message.error('无权限访问该资源');
          break;
        case 404:
          // 资源不存在
          message.error('请求的资源不存在');
          break;
        case 500:
          // 服务器内部错误
          message.error('服务器内部错误，请稍后重试');
          break;
        default:
          // 其他错误状态码
          const errorMsg = error.response.data?.msg || `请求失败 (${status})`;
          message.error(errorMsg);
      }
    } else if (error.request) {
      // 请求已发送但没有收到响应（网络错误、超时等）
      if (error.code === 'ECONNABORTED') {
        message.error('请求超时，请检查网络连接');
      } else {
        message.error('网络连接失败，请检查网络设置');
      }
    } else {
      // 请求配置错误
      console.error('请求错误:', error.message);
      message.error(error.message || '请求失败');
    }

    return Promise.reject(error);
  }
);

export default apiClient;

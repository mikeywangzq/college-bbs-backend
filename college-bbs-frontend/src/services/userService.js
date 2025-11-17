/**
 * @file userService.js
 * @brief 用户相关API服务
 * @details 提供用户注册、登录、获取信息等接口
 */

import apiClient from './api';

/**
 * @brief 用户注册
 * @param {Object} userData - 用户注册信息
 * @param {string} userData.username - 用户名
 * @param {string} userData.password - 密码
 * @param {string} userData.email - 邮箱
 * @return {Promise<Object>} 注册结果
 */
export const register = (userData) => {
  return apiClient.post('/user/register', userData);
};

/**
 * @brief 用户登录
 * @param {Object} credentials - 登录凭证
 * @param {string} credentials.username - 用户名
 * @param {string} credentials.password - 密码
 * @return {Promise<Object>} 登录结果，包含token和用户信息
 */
export const login = (credentials) => {
  return apiClient.post('/user/login', credentials);
};

/**
 * @brief 获取当前用户信息
 * @return {Promise<Object>} 用户信息
 */
export const getUserInfo = () => {
  return apiClient.get('/user/info');
};

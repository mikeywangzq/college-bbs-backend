/**
 * @file auth.js
 * @brief Token管理工具
 * @details 提供Token的存储、获取、删除和验证功能
 */

const TOKEN_KEY = 'college_bbs_token';
const USER_KEY = 'college_bbs_user';

/**
 * @brief 保存Token到localStorage
 * @param {string} token - JWT Token
 */
export const setToken = (token) => {
  localStorage.setItem(TOKEN_KEY, token);
};

/**
 * @brief 获取Token
 * @return {string|null} JWT Token或null
 */
export const getToken = () => {
  return localStorage.getItem(TOKEN_KEY);
};

/**
 * @brief 删除Token
 */
export const removeToken = () => {
  localStorage.removeItem(TOKEN_KEY);
};

/**
 * @brief 保存用户信息
 * @param {Object} user - 用户信息对象
 */
export const setUser = (user) => {
  localStorage.setItem(USER_KEY, JSON.stringify(user));
};

/**
 * @brief 获取用户信息
 * @return {Object|null} 用户信息对象或null
 */
export const getUser = () => {
  const userStr = localStorage.getItem(USER_KEY);
  return userStr ? JSON.parse(userStr) : null;
};

/**
 * @brief 删除用户信息
 */
export const removeUser = () => {
  localStorage.removeItem(USER_KEY);
};

/**
 * @brief 检查是否已登录
 * @return {boolean} 是否已登录
 */
export const isAuthenticated = () => {
  return !!getToken();
};

/**
 * @brief 清除所有认证信息（登出）
 */
export const clearAuth = () => {
  removeToken();
  removeUser();
};

/**
 * @file postService.js
 * @brief 帖子相关API服务
 * @details 提供帖子的CRUD操作接口
 */

import apiClient from './api';

/**
 * @brief 获取帖子列表
 * @param {number} page - 页码（从1开始）
 * @param {number} size - 每页数量
 * @return {Promise<Object>} 帖子列表和分页信息
 */
export const getPostList = (page = 1, size = 20) => {
  return apiClient.get('/post/list', {
    params: { page, size },
  });
};

/**
 * @brief 获取帖子详情
 * @param {number} postId - 帖子ID
 * @return {Promise<Object>} 帖子详情和回复列表
 */
export const getPostDetail = (postId) => {
  return apiClient.get('/post/detail', {
    params: { id: postId },
  });
};

/**
 * @brief 创建新帖子
 * @param {Object} postData - 帖子数据
 * @param {string} postData.title - 帖子标题
 * @param {string} postData.content - 帖子内容
 * @return {Promise<Object>} 创建结果，包含post_id
 */
export const createPost = (postData) => {
  return apiClient.post('/post/create', postData);
};

/**
 * @brief 删除帖子
 * @param {number} postId - 帖子ID
 * @return {Promise<Object>} 删除结果
 */
export const deletePost = (postId) => {
  return apiClient.delete('/post/delete', {
    data: { post_id: postId },
  });
};

/**
 * @file likeService.js
 * @brief 点赞相关API服务
 * @details 提供点赞切换接口
 */

import apiClient from './api';

/**
 * @brief 切换点赞状态
 * @param {number} postId - 帖子ID
 * @return {Promise<Object>} 点赞结果，包含liked状态和like_count
 */
export const toggleLike = (postId) => {
  return apiClient.post('/like/toggle', { post_id: postId });
};

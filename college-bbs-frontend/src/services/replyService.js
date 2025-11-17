/**
 * @file replyService.js
 * @brief 回复相关API服务
 * @details 提供回复的创建和删除接口
 */

import apiClient from './api';

/**
 * @brief 创建回复
 * @param {Object} replyData - 回复数据
 * @param {number} replyData.post_id - 帖子ID
 * @param {string} replyData.content - 回复内容
 * @return {Promise<Object>} 创建结果，包含reply_id
 */
export const createReply = (replyData) => {
  return apiClient.post('/reply/create', replyData);
};

/**
 * @brief 删除回复
 * @param {number} replyId - 回复ID
 * @return {Promise<Object>} 删除结果
 */
export const deleteReply = (replyId) => {
  return apiClient.delete('/reply/delete', {
    data: { reply_id: replyId },
  });
};

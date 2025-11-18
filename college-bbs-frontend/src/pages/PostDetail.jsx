/**
 * @file PostDetail.jsx
 * @brief 帖子详情页组件
 * @details
 * 功能列表：
 * - 显示帖子完整内容（标题、内容、作者、时间、统计数据）
 * - 显示所有回复列表
 * - 支持点赞/取消点赞（需登录）
 * - 支持发表回复（需登录）
 * - 支持删除自己的帖子和回复（权限控制）
 * - Loading状态和错误处理
 *
 * 权限控制：
 * - 查看帖子：所有用户（包括游客）
 * - 点赞/回复：需要登录
 * - 删除帖子：仅帖子作者
 * - 删除回复：仅回复作者（在ReplyItem组件中控制）
 *
 * 状态管理：
 * - loading: 数据加载状态
 * - post: 帖子详情数据
 * - replies: 回复列表
 * - replyContent: 回复输入内容
 * - submitting: 回复提交状态
 * - liked: 当前用户点赞状态
 * - liking: 点赞操作进行中（防止重复点击）
 *
 * @author College BBS Team
 * @date 2024-11-17
 * @version 1.1
 */

import { useState, useEffect, useCallback, useRef } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import {
  Layout,
  Typography,
  Space,
  Button,
  Divider,
  Spin,
  Input,
  message,
  Popconfirm,
  Card,
} from 'antd';
import {
  EyeOutlined,
  HeartOutlined,
  HeartFilled,
  MessageOutlined,
  ClockCircleOutlined,
  DeleteOutlined,
} from '@ant-design/icons';
import { getPostDetail, deletePost } from '../services/postService';
import { createReply, deleteReply } from '../services/replyService';
import { toggleLike } from '../services/likeService';
import ReplyItem from '../components/ReplyItem';
import { isAuthenticated, getUser } from '../utils/auth';

const { Content } = Layout;
const { Title, Text, Paragraph } = Typography;
const { TextArea } = Input;

/**
 * @brief 帖子详情页组件
 * @return {JSX.Element} 帖子详情页面
 *
 * @example
 * // 路由配置
 * <Route path="/post/:id" element={<PostDetail />} />
 *
 * @note 组件卸载时会清理未完成的异步请求，避免内存泄漏
 */
const PostDetail = () => {
  // 路由参数和导航
  const { id } = useParams(); // 帖子ID（字符串类型）
  const navigate = useNavigate();

  // 组件状态
  const [loading, setLoading] = useState(false); // 页面加载状态
  const [post, setPost] = useState(null); // 帖子详情数据
  const [replies, setReplies] = useState([]); // 回复列表
  const [replyContent, setReplyContent] = useState(''); // 回复输入内容
  const [submitting, setSubmitting] = useState(false); // 回复提交中
  const [liked, setLiked] = useState(false); // 当前用户点赞状态
  const [liking, setLiking] = useState(false); // BUG FIX: 点赞操作进行中，防止重复点击

  // BUG FIX: 使用ref标记组件是否已卸载，防止在卸载后调用setState
  const isMountedRef = useRef(true);

  // 获取当前用户信息
  const currentUser = getUser();
  const isLoggedIn = isAuthenticated();
  const isAuthor = currentUser && post && currentUser.user_id === post.author_id;

  /**
   * @brief 获取帖子详情
   * @details
   * 从后端获取帖子的完整信息和所有回复
   * 每次查看会增加浏览数（后端处理）
   *
   * @async
   * @return {Promise<void>}
   *
   * @note 使用useCallback包裹，避免useEffect依赖警告
   * @note 添加组件卸载检查，避免setState on unmounted component警告
   */
  const fetchPostDetail = useCallback(async () => {
    setLoading(true);
    try {
      // BUG FIX: 将字符串ID转换为数字
      const data = await getPostDetail(parseInt(id));

      // BUG FIX: 检查组件是否已卸载
      if (!isMountedRef.current) return;

      setPost(data.post);
      setReplies(data.replies || []); // BUG FIX: 提供默认空数组

      // BUG FIX: 如果后端返回了用户点赞状态，使用它；否则需要从user_liked字段判断
      // 注意：当前后端未返回user_liked字段，这是一个需要后端支持的功能
      // 临时方案：刷新页面后点赞状态会丢失
      if (data.post.user_liked !== undefined) {
        setLiked(data.post.user_liked);
      }
    } catch (error) {
      // 错误已在axios拦截器中处理
      // 如果帖子不存在或其他错误，post保持为null，会显示"帖子不存在"
    } finally {
      if (isMountedRef.current) {
        setLoading(false);
      }
    }
  }, [id]); // 仅依赖id，id变化时重新获取

  /**
   * @brief 组件挂载时获取帖子详情
   * @note 组件卸载时清理isMountedRef，防止内存泄漏
   */
  useEffect(() => {
    fetchPostDetail();

    // 清理函数：组件卸载时执行
    return () => {
      isMountedRef.current = false;
    };
  }, [fetchPostDetail]);

  /**
   * @brief 处理点赞/取消点赞
   * @details
   * 1. 检查登录状态，未登录跳转登录页
   * 2. 调用后端API切换点赞状态
   * 3. 更新本地点赞状态和点赞数
   *
   * @async
   * @return {Promise<void>}
   *
   * @note 添加liking状态防止重复点击
   * @note 乐观更新UI，如果失败则回滚
   */
  const handleLike = async () => {
    // 检查登录状态
    if (!isLoggedIn) {
      message.warning('请先登录');
      navigate('/login');
      return;
    }

    // BUG FIX: 防止重复点击
    if (liking) {
      return;
    }

    // 保存当前状态，用于失败时回滚
    const prevLiked = liked;
    const prevLikeCount = post.like_count;

    // 乐观更新UI（先更新UI，再调用API）
    setLiking(true);
    setLiked(!liked);
    setPost({ ...post, like_count: liked ? prevLikeCount - 1 : prevLikeCount + 1 });

    try {
      const data = await toggleLike(post.id);

      // BUG FIX: 使用后端返回的实际数据更新状态
      if (isMountedRef.current) {
        setLiked(data.liked);
        setPost((prevPost) => ({ ...prevPost, like_count: data.like_count }));
        message.success(data.liked ? '点赞成功' : '取消点赞');
      }
    } catch (error) {
      // API调用失败，回滚UI状态
      if (isMountedRef.current) {
        setLiked(prevLiked);
        setPost((prevPost) => ({ ...prevPost, like_count: prevLikeCount }));
      }
    } finally {
      if (isMountedRef.current) {
        setLiking(false);
      }
    }
  };

  /**
   * @brief 删除帖子
   * @details
   * 1. 调用后端API删除帖子
   * 2. 删除成功后跳转到首页
   * 3. 后端会级联删除所有回复和点赞记录
   *
   * @async
   * @return {Promise<void>}
   *
   * @note 此函数只会在用户确认Popconfirm后调用
   */
  const handleDeletePost = async () => {
    try {
      await deletePost(post.id);
      message.success('删除成功');
      navigate('/'); // 跳转到首页
    } catch (error) {
      // 错误已在axios拦截器中处理
    }
  };

  /**
   * @brief 提交回复
   * @details
   * 1. 检查登录状态
   * 2. 验证回复内容（非空、长度限制）
   * 3. 调用后端API创建回复
   * 4. 成功后清空输入框并刷新帖子详情
   *
   * @async
   * @return {Promise<void>}
   *
   * @note 回复成功后会重新获取帖子详情，确保回复数量和列表同步
   */
  const handleSubmitReply = async () => {
    // 检查登录状态
    if (!isLoggedIn) {
      message.warning('请先登录');
      navigate('/login');
      return;
    }

    // 验证回复内容
    const trimmedContent = replyContent.trim();
    if (!trimmedContent) {
      message.warning('回复内容不能为空');
      return;
    }

    if (trimmedContent.length > 1000) {
      message.warning('回复内容不能超过1000字');
      return;
    }

    setSubmitting(true);
    try {
      // BUG FIX: 确保post_id是数字类型
      await createReply({
        post_id: parseInt(id),
        content: trimmedContent, // 使用trim后的内容
      });

      if (isMountedRef.current) {
        message.success('回复成功');
        setReplyContent(''); // 清空输入框
        // 刷新帖子详情，获取最新的回复列表和回复数
        await fetchPostDetail();
      }
    } catch (error) {
      // 错误已在axios拦截器中处理
    } finally {
      if (isMountedRef.current) {
        setSubmitting(false);
      }
    }
  };

  /**
   * @brief 删除回复
   * @details
   * 1. 调用后端API删除回复
   * 2. 成功后刷新帖子详情
   *
   * @async
   * @param {number} replyId - 要删除的回复ID
   * @return {Promise<void>}
   *
   * @note 权限验证在ReplyItem组件中完成，这里假设已通过验证
   * @note 删除成功后刷新整个帖子详情，确保reply_count同步
   */
  const handleDeleteReply = async (replyId) => {
    try {
      await deleteReply(replyId);

      if (isMountedRef.current) {
        message.success('删除成功');
        // 刷新帖子详情，更新回复列表和回复数
        await fetchPostDetail();
      }
    } catch (error) {
      // 错误已在axios拦截器中处理
    }
  };

  /**
   * 加载中状态
   */
  if (loading) {
    return (
      <Content style={{ padding: '24px 50px', textAlign: 'center' }}>
        <Spin size="large" tip="加载中..." />
      </Content>
    );
  }

  /**
   * 帖子不存在或加载失败
   */
  if (!post) {
    return (
      <Content style={{ padding: '24px 50px', textAlign: 'center' }}>
        <Text type="secondary" style={{ fontSize: '16px' }}>
          帖子不存在或已被删除
        </Text>
        <div style={{ marginTop: 16 }}>
          <Button type="primary" onClick={() => navigate('/')}>
            返回首页
          </Button>
        </div>
      </Content>
    );
  }

  /**
   * 帖子详情页面
   */
  return (
    <Content style={{ padding: '24px 50px', minHeight: 'calc(100vh - 64px)' }}>
      <div style={{ maxWidth: 1000, margin: '0 auto' }}>
        {/* ========== 帖子内容卡片 ========== */}
        <Card>
          {/* 帖子标题 */}
          <Title level={2}>{post.title}</Title>

          {/* 作者和发布时间 */}
          <Space style={{ marginBottom: 16 }}>
            <Text type="secondary">作者：{post.author}</Text>
            <Text type="secondary">
              <ClockCircleOutlined /> {post.created_at}
            </Text>
          </Space>

          {/* 统计数据：浏览数、回复数、点赞数 */}
          <Space split={<span style={{ color: '#d9d9d9' }}>|</span>}>
            <Space size="small">
              <EyeOutlined />
              <Text type="secondary">{post.view_count}</Text>
            </Space>
            <Space size="small">
              <MessageOutlined />
              <Text type="secondary">{post.reply_count}</Text>
            </Space>
            <Space size="small">
              {liked ? <HeartFilled style={{ color: '#ff4d4f' }} /> : <HeartOutlined />}
              <Text type="secondary">{post.like_count}</Text>
            </Space>
          </Space>

          <Divider />

          {/* 帖子内容（保留换行） */}
          <Paragraph style={{ fontSize: '16px', whiteSpace: 'pre-wrap' }}>
            {post.content}
          </Paragraph>

          {/* 操作按钮：点赞、删除 */}
          <Space>
            {/* 点赞按钮 */}
            <Button
              type={liked ? 'primary' : 'default'}
              icon={liked ? <HeartFilled /> : <HeartOutlined />}
              onClick={handleLike}
              loading={liking} // BUG FIX: 添加loading状态
              danger={liked}
            >
              {liked ? '取消点赞' : '点赞'}
            </Button>

            {/* 删除按钮（仅作者可见） */}
            {isAuthor && (
              <Popconfirm
                title="确定要删除这篇帖子吗？"
                description="删除后将无法恢复，所有回复也会被删除"
                onConfirm={handleDeletePost}
                okText="确定"
                cancelText="取消"
              >
                <Button danger icon={<DeleteOutlined />}>
                  删除帖子
                </Button>
              </Popconfirm>
            )}
          </Space>
        </Card>

        {/* ========== 回复列表卡片 ========== */}
        <Card style={{ marginTop: 16 }}>
          <Title level={4}>回复 ({replies.length})</Title>
          <Divider />

          {replies.length > 0 ? (
            // 渲染回复列表
            replies.map((reply) => (
              <ReplyItem
                key={reply.id}
                reply={reply}
                onDelete={handleDeleteReply}
              />
            ))
          ) : (
            // 暂无回复
            <div style={{ textAlign: 'center', padding: '20px 0' }}>
              <Text type="secondary">暂无回复，快来发表第一条回复吧~</Text>
            </div>
          )}
        </Card>

        {/* ========== 回复输入框卡片 ========== */}
        <Card style={{ marginTop: 16 }}>
          <Title level={4}>发表回复</Title>
          <TextArea
            rows={4}
            value={replyContent}
            onChange={(e) => setReplyContent(e.target.value)}
            placeholder={isLoggedIn ? '输入回复内容（1-1000字）...' : '请先登录后再回复'}
            disabled={!isLoggedIn}
            maxLength={1000}
            showCount
          />
          <div style={{ marginTop: 16, textAlign: 'right' }}>
            <Button
              type="primary"
              onClick={handleSubmitReply}
              loading={submitting}
              disabled={!isLoggedIn || !replyContent.trim()} // 禁用条件：未登录或内容为空
            >
              发表回复
            </Button>
          </div>
        </Card>
      </div>
    </Content>
  );
};

export default PostDetail;

/**
 * @file PostDetail.jsx
 * @brief 帖子详情页
 * @details 显示帖子完整内容、回复列表、支持点赞和回复
 */

import { useState, useEffect } from 'react';
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

const PostDetail = () => {
  const { id } = useParams();
  const navigate = useNavigate();
  const [loading, setLoading] = useState(false);
  const [post, setPost] = useState(null);
  const [replies, setReplies] = useState([]);
  const [replyContent, setReplyContent] = useState('');
  const [submitting, setSubmitting] = useState(false);
  const [liked, setLiked] = useState(false);

  const currentUser = getUser();
  const isLoggedIn = isAuthenticated();
  const isAuthor = currentUser && post && currentUser.user_id === post.author_id;

  // 获取帖子详情
  const fetchPostDetail = async () => {
    setLoading(true);
    try {
      const data = await getPostDetail(id);
      setPost(data.post);
      setReplies(data.replies);
    } catch (error) {
      // 错误已在axios拦截器中处理
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchPostDetail();
  }, [id]);

  // 点赞/取消点赞
  const handleLike = async () => {
    if (!isLoggedIn) {
      message.warning('请先登录');
      navigate('/login');
      return;
    }

    try {
      const data = await toggleLike(post.id);
      setLiked(data.liked);
      setPost({ ...post, like_count: data.like_count });
      message.success(data.liked ? '点赞成功' : '取消点赞');
    } catch (error) {
      // 错误已在axios拦截器中处理
    }
  };

  // 删除帖子
  const handleDeletePost = async () => {
    try {
      await deletePost(post.id);
      message.success('删除成功');
      navigate('/');
    } catch (error) {
      // 错误已在axios拦截器中处理
    }
  };

  // 提交回复
  const handleSubmitReply = async () => {
    if (!isLoggedIn) {
      message.warning('请先登录');
      navigate('/login');
      return;
    }

    if (!replyContent.trim()) {
      message.warning('回复内容不能为空');
      return;
    }

    if (replyContent.length > 1000) {
      message.warning('回复内容不能超过1000字');
      return;
    }

    setSubmitting(true);
    try {
      await createReply({
        post_id: parseInt(id),
        content: replyContent,
      });

      message.success('回复成功');
      setReplyContent('');
      // 刷新帖子详情
      fetchPostDetail();
    } catch (error) {
      // 错误已在axios拦截器中处理
    } finally {
      setSubmitting(false);
    }
  };

  // 删除回复
  const handleDeleteReply = async (replyId) => {
    try {
      await deleteReply(replyId);
      message.success('删除成功');
      // 刷新帖子详情
      fetchPostDetail();
    } catch (error) {
      // 错误已在axios拦截器中处理
    }
  };

  if (loading) {
    return (
      <Content style={{ padding: '24px 50px', textAlign: 'center' }}>
        <Spin size="large" />
      </Content>
    );
  }

  if (!post) {
    return (
      <Content style={{ padding: '24px 50px', textAlign: 'center' }}>
        <Text>帖子不存在</Text>
      </Content>
    );
  }

  return (
    <Content style={{ padding: '24px 50px', minHeight: 'calc(100vh - 64px)' }}>
      <div style={{ maxWidth: 1000, margin: '0 auto' }}>
        {/* 帖子内容 */}
        <Card>
          <Title level={2}>{post.title}</Title>

          <Space style={{ marginBottom: 16 }}>
            <Text type="secondary">作者：{post.author}</Text>
            <Text type="secondary">
              <ClockCircleOutlined /> {post.created_at}
            </Text>
          </Space>

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

          <Paragraph style={{ fontSize: '16px', whiteSpace: 'pre-wrap' }}>
            {post.content}
          </Paragraph>

          <Space>
            <Button
              type={liked ? 'primary' : 'default'}
              icon={liked ? <HeartFilled /> : <HeartOutlined />}
              onClick={handleLike}
              danger={liked}
            >
              {liked ? '取消点赞' : '点赞'}
            </Button>

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

        {/* 回复列表 */}
        <Card style={{ marginTop: 16 }}>
          <Title level={4}>回复 ({replies.length})</Title>
          <Divider />

          {replies.length > 0 ? (
            replies.map((reply) => (
              <ReplyItem
                key={reply.id}
                reply={reply}
                onDelete={handleDeleteReply}
              />
            ))
          ) : (
            <Text type="secondary">暂无回复</Text>
          )}
        </Card>

        {/* 回复输入框 */}
        <Card style={{ marginTop: 16 }}>
          <Title level={4}>发表回复</Title>
          <TextArea
            rows={4}
            value={replyContent}
            onChange={(e) => setReplyContent(e.target.value)}
            placeholder={isLoggedIn ? '输入回复内容...' : '请先登录后再回复'}
            disabled={!isLoggedIn}
            maxLength={1000}
            showCount
          />
          <div style={{ marginTop: 16, textAlign: 'right' }}>
            <Button
              type="primary"
              onClick={handleSubmitReply}
              loading={submitting}
              disabled={!isLoggedIn}
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

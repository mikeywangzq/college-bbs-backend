/**
 * @file PostCard.jsx
 * @brief 帖子卡片组件
 * @details 在帖子列表中显示单个帖子的摘要信息
 */

import { Card, Space, Typography, Tag } from 'antd';
import {
  EyeOutlined,
  MessageOutlined,
  HeartOutlined,
  ClockCircleOutlined,
} from '@ant-design/icons';
import { useNavigate } from 'react-router-dom';

const { Title, Text } = Typography;

const PostCard = ({ post }) => {
  const navigate = useNavigate();

  return (
    <Card
      hoverable
      style={{ marginBottom: '16px' }}
      onClick={() => navigate(`/post/${post.id}`)}
    >
      <Space direction="vertical" style={{ width: '100%' }} size="small">
        {/* 标题 */}
        <Title level={4} style={{ margin: 0 }}>
          {post.title}
        </Title>

        {/* 作者和时间 */}
        <Space>
          <Text type="secondary">作者：{post.author}</Text>
          <Text type="secondary">
            <ClockCircleOutlined /> {post.created_at}
          </Text>
        </Space>

        {/* 统计信息 */}
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
            <HeartOutlined />
            <Text type="secondary">{post.like_count}</Text>
          </Space>
        </Space>
      </Space>
    </Card>
  );
};

export default PostCard;

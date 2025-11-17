/**
 * @file ReplyItem.jsx
 * @brief 回复项组件
 * @details 显示单条回复，包含删除功能
 */

import { Comment } from '@ant-design/compatible';
import { Typography, Button, Popconfirm, Space, Avatar } from 'antd';
import { UserOutlined, DeleteOutlined } from '@ant-design/icons';
import { getUser } from '../utils/auth';

const { Text } = Typography;

const ReplyItem = ({ reply, onDelete }) => {
  const currentUser = getUser();
  const isAuthor = currentUser && currentUser.user_id === reply.author_id;

  return (
    <div style={{ borderBottom: '1px solid #f0f0f0', padding: '12px 0' }}>
      <Space direction="vertical" style={{ width: '100%' }} size="small">
        {/* 作者和时间 */}
        <Space>
          <Avatar icon={<UserOutlined />} size="small" />
          <Text strong>{reply.author}</Text>
          <Text type="secondary" style={{ fontSize: '12px' }}>
            {reply.created_at}
          </Text>
        </Space>

        {/* 回复内容 */}
        <Text style={{ marginLeft: '32px' }}>{reply.content}</Text>

        {/* 删除按钮（仅作者可见） */}
        {isAuthor && (
          <div style={{ marginLeft: '32px' }}>
            <Popconfirm
              title="确定要删除这条回复吗？"
              onConfirm={() => onDelete(reply.id)}
              okText="确定"
              cancelText="取消"
            >
              <Button
                type="text"
                size="small"
                danger
                icon={<DeleteOutlined />}
              >
                删除
              </Button>
            </Popconfirm>
          </div>
        )}
      </Space>
    </div>
  );
};

export default ReplyItem;

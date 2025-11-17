/**
 * @file Profile.jsx
 * @brief 个人中心页面
 * @details 显示用户信息和统计数据
 */

import { useState, useEffect } from 'react';
import {
  Layout,
  Typography,
  Card,
  Descriptions,
  Avatar,
  Spin,
  Statistic,
  Row,
  Col,
} from 'antd';
import { UserOutlined, EditOutlined, MessageOutlined } from '@ant-design/icons';
import { getUserInfo } from '../services/userService';

const { Content } = Layout;
const { Title } = Typography;

const Profile = () => {
  const [loading, setLoading] = useState(false);
  const [userInfo, setUserInfo] = useState(null);

  // 获取用户信息
  const fetchUserInfo = async () => {
    setLoading(true);
    try {
      const data = await getUserInfo();
      setUserInfo(data);
    } catch (error) {
      // 错误已在axios拦截器中处理
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchUserInfo();
  }, []);

  if (loading) {
    return (
      <Content style={{ padding: '24px 50px', textAlign: 'center' }}>
        <Spin size="large" />
      </Content>
    );
  }

  if (!userInfo) {
    return null;
  }

  return (
    <Content style={{ padding: '24px 50px', minHeight: 'calc(100vh - 64px)' }}>
      <div style={{ maxWidth: 800, margin: '0 auto' }}>
        <Title level={2}>个人中心</Title>

        {/* 用户基本信息 */}
        <Card style={{ marginBottom: 16 }}>
          <div style={{ display: 'flex', alignItems: 'center', marginBottom: 24 }}>
            <Avatar size={80} icon={<UserOutlined />} />
            <div style={{ marginLeft: 24 }}>
              <Title level={3} style={{ margin: 0 }}>
                {userInfo.username}
              </Title>
            </div>
          </div>

          <Descriptions bordered column={1}>
            <Descriptions.Item label="用户名">
              {userInfo.username}
            </Descriptions.Item>
            <Descriptions.Item label="邮箱">
              {userInfo.email}
            </Descriptions.Item>
            <Descriptions.Item label="注册时间">
              {userInfo.created_at}
            </Descriptions.Item>
          </Descriptions>
        </Card>

        {/* 统计数据 */}
        <Card title="我的统计">
          <Row gutter={16}>
            <Col span={12}>
              <Statistic
                title="发帖数"
                value={userInfo.post_count}
                prefix={<EditOutlined />}
              />
            </Col>
            <Col span={12}>
              <Statistic
                title="回复数"
                value={userInfo.reply_count}
                prefix={<MessageOutlined />}
              />
            </Col>
          </Row>
        </Card>
      </div>
    </Content>
  );
};

export default Profile;

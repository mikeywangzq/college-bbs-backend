/**
 * @file CreatePost.jsx
 * @brief 发帖页面
 * @details 创建新帖子，包含标题和内容表单
 */

import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  Layout,
  Typography,
  Form,
  Input,
  Button,
  Card,
  message,
} from 'antd';
import { createPost } from '../services/postService';

const { Content } = Layout;
const { Title } = Typography;
const { TextArea } = Input;

const CreatePost = () => {
  const navigate = useNavigate();
  const [loading, setLoading] = useState(false);
  const [form] = Form.useForm();

  const onFinish = async (values) => {
    setLoading(true);
    try {
      const data = await createPost(values);

      message.success('发帖成功');
      // 跳转到新创建的帖子详情页
      navigate(`/post/${data.post_id}`);
    } catch (error) {
      // 错误已在axios拦截器中处理
    } finally {
      setLoading(false);
    }
  };

  return (
    <Content style={{ padding: '24px 50px', minHeight: 'calc(100vh - 64px)' }}>
      <div style={{ maxWidth: 800, margin: '0 auto' }}>
        <Card>
          <Title level={2}>发布新帖</Title>

          <Form
            form={form}
            layout="vertical"
            onFinish={onFinish}
            autoComplete="off"
          >
            <Form.Item
              label="帖子标题"
              name="title"
              rules={[
                { required: true, message: '请输入帖子标题' },
                { min: 5, max: 100, message: '标题长度为5-100个字符' },
              ]}
            >
              <Input
                placeholder="请输入帖子标题（5-100字）"
                maxLength={100}
                showCount
              />
            </Form.Item>

            <Form.Item
              label="帖子内容"
              name="content"
              rules={[
                { required: true, message: '请输入帖子内容' },
                { min: 10, max: 10000, message: '内容长度为10-10000个字符' },
              ]}
            >
              <TextArea
                rows={12}
                placeholder="请输入帖子内容（10-10000字）"
                maxLength={10000}
                showCount
              />
            </Form.Item>

            <Form.Item>
              <Button
                type="primary"
                htmlType="submit"
                loading={loading}
                size="large"
                style={{ marginRight: 16 }}
              >
                发布
              </Button>
              <Button
                size="large"
                onClick={() => navigate('/')}
              >
                取消
              </Button>
            </Form.Item>
          </Form>
        </Card>
      </div>
    </Content>
  );
};

export default CreatePost;

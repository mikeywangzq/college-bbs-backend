/**
 * @file Home.jsx
 * @brief 首页（帖子列表）
 * @details 显示所有帖子列表，支持分页
 */

import { useState, useEffect } from 'react';
import { Layout, Typography, Pagination, Spin, Empty } from 'antd';
import PostCard from '../components/PostCard';
import { getPostList } from '../services/postService';

const { Content } = Layout;
const { Title } = Typography;

const Home = () => {
  const [posts, setPosts] = useState([]);
  const [loading, setLoading] = useState(false);
  const [pagination, setPagination] = useState({
    current: 1,
    pageSize: 20,
    total: 0,
  });

  // 获取帖子列表
  const fetchPosts = async (page = 1, size = 20) => {
    setLoading(true);
    try {
      const data = await getPostList(page, size);
      setPosts(data.posts);
      setPagination({
        current: data.page,
        pageSize: data.size,
        total: data.total,
      });
    } catch (error) {
      // 错误已在axios拦截器中处理
    } finally {
      setLoading(false);
    }
  };

  // 页面加载时获取帖子列表
  useEffect(() => {
    fetchPosts();
  }, []);

  // 分页变化
  const handlePageChange = (page, pageSize) => {
    fetchPosts(page, pageSize);
    window.scrollTo({ top: 0, behavior: 'smooth' });
  };

  return (
    <Content style={{ padding: '24px 50px', minHeight: 'calc(100vh - 64px)' }}>
      <div style={{ maxWidth: 1000, margin: '0 auto' }}>
        <Title level={2} style={{ marginBottom: 24 }}>
          帖子列表
        </Title>

        {loading ? (
          <div style={{ textAlign: 'center', padding: '50px 0' }}>
            <Spin size="large" />
          </div>
        ) : posts.length > 0 ? (
          <>
            {posts.map((post) => (
              <PostCard key={post.id} post={post} />
            ))}

            <div style={{ textAlign: 'center', marginTop: 24 }}>
              <Pagination
                current={pagination.current}
                pageSize={pagination.pageSize}
                total={pagination.total}
                onChange={handlePageChange}
                showSizeChanger={false}
                showTotal={(total) => `共 ${total} 条`}
              />
            </div>
          </>
        ) : (
          <Empty description="暂无帖子" />
        )}
      </div>
    </Content>
  );
};

export default Home;

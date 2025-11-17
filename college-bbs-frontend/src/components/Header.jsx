/**
 * @file Header.jsx
 * @brief 导航栏组件
 * @details 显示网站Logo、导航菜单、用户信息
 */

import { Layout, Menu, Button, Space, Avatar, Dropdown } from 'antd';
import {
  HomeOutlined,
  EditOutlined,
  UserOutlined,
  LogoutOutlined,
  LoginOutlined,
} from '@ant-design/icons';
import { useNavigate, useLocation } from 'react-router-dom';
import { isAuthenticated, getUser, clearAuth } from '../utils/auth';
import { message } from 'antd';

const { Header: AntHeader } = Layout;

const Header = () => {
  const navigate = useNavigate();
  const location = useLocation();
  const isLoggedIn = isAuthenticated();
  const user = getUser();

  // 处理登出
  const handleLogout = () => {
    clearAuth();
    message.success('已退出登录');
    navigate('/login');
  };

  // 用户菜单项
  const userMenuItems = [
    {
      key: 'profile',
      icon: <UserOutlined />,
      label: '个人中心',
      onClick: () => navigate('/profile'),
    },
    {
      key: 'logout',
      icon: <LogoutOutlined />,
      label: '退出登录',
      onClick: handleLogout,
    },
  ];

  // 导航菜单项
  const menuItems = [
    {
      key: '/',
      icon: <HomeOutlined />,
      label: '首页',
      onClick: () => navigate('/'),
    },
    ...(isLoggedIn
      ? [
          {
            key: '/create-post',
            icon: <EditOutlined />,
            label: '发帖',
            onClick: () => navigate('/create-post'),
          },
        ]
      : []),
  ];

  return (
    <AntHeader
      style={{
        position: 'sticky',
        top: 0,
        zIndex: 1,
        width: '100%',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'space-between',
        padding: '0 50px',
        background: '#001529',
      }}
    >
      {/* Logo */}
      <div
        style={{
          color: 'white',
          fontSize: '20px',
          fontWeight: 'bold',
          cursor: 'pointer',
        }}
        onClick={() => navigate('/')}
      >
        计算机学院贴吧
      </div>

      {/* 导航菜单 */}
      <Menu
        theme="dark"
        mode="horizontal"
        selectedKeys={[location.pathname]}
        items={menuItems}
        style={{ flex: 1, minWidth: 0, marginLeft: '50px' }}
      />

      {/* 用户区域 */}
      <Space>
        {isLoggedIn ? (
          <Dropdown menu={{ items: userMenuItems }} placement="bottomRight">
            <Space style={{ cursor: 'pointer', color: 'white' }}>
              <Avatar icon={<UserOutlined />} />
              <span>{user?.username}</span>
            </Space>
          </Dropdown>
        ) : (
          <Space>
            <Button
              type="link"
              icon={<LoginOutlined />}
              onClick={() => navigate('/login')}
              style={{ color: 'white' }}
            >
              登录
            </Button>
            <Button type="primary" onClick={() => navigate('/register')}>
              注册
            </Button>
          </Space>
        )}
      </Space>
    </AntHeader>
  );
};

export default Header;

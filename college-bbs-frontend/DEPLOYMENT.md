# 前端部署指南

## 本地开发部署

### 1. 环境准备

确保已安装：
- Node.js >= 16.0.0
- npm >= 8.0.0

### 2. 安装依赖

```bash
cd college-bbs-frontend
npm install
```

### 3. 配置环境变量

编辑 `.env` 文件：

```bash
VITE_API_BASE_URL=http://localhost:8080
```

### 4. 启动开发服务器

```bash
npm run dev
```

访问 http://localhost:5173

## 生产环境部署

### 方法一：Nginx 静态部署

#### 1. 构建生产版本

```bash
npm run build
```

构建产物在 `dist/` 目录

#### 2. 配置 Nginx

创建 `/etc/nginx/sites-available/college-bbs` 文件：

```nginx
server {
    listen 80;
    server_name your-domain.com;

    root /var/www/college-bbs-frontend/dist;
    index index.html;

    # 前端路由支持
    location / {
        try_files $uri $uri/ /index.html;
    }

    # API 代理
    location /api {
        proxy_pass http://localhost:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }

    # 启用 Gzip 压缩
    gzip on;
    gzip_types text/plain text/css application/json application/javascript text/xml application/xml application/xml+rss text/javascript;

    # 静态资源缓存
    location ~* \.(js|css|png|jpg|jpeg|gif|ico|svg)$ {
        expires 1y;
        add_header Cache-Control "public, immutable";
    }
}
```

#### 3. 启用配置并重启 Nginx

```bash
sudo ln -s /etc/nginx/sites-available/college-bbs /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl reload nginx
```

### 方法二：Docker 部署

#### 1. 创建 Dockerfile

```dockerfile
# 构建阶段
FROM node:18-alpine as build

WORKDIR /app

# 复制依赖文件
COPY package*.json ./
RUN npm install

# 复制源代码
COPY . .

# 构建生产版本
RUN npm run build

# 运行阶段
FROM nginx:alpine

# 复制构建产物
COPY --from=build /app/dist /usr/share/nginx/html

# 复制 Nginx 配置
COPY nginx.conf /etc/nginx/conf.d/default.conf

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
```

#### 2. 创建 nginx.conf

```nginx
server {
    listen 80;
    server_name localhost;

    root /usr/share/nginx/html;
    index index.html;

    location / {
        try_files $uri $uri/ /index.html;
    }

    gzip on;
    gzip_types text/plain text/css application/json application/javascript text/xml application/xml;
}
```

#### 3. 构建并运行 Docker 容器

```bash
# 构建镜像
docker build -t college-bbs-frontend .

# 运行容器
docker run -d -p 80:80 --name college-bbs-frontend college-bbs-frontend
```

### 方法三：Docker Compose 部署（前后端一起）

创建 `docker-compose.yml`：

```yaml
version: '3.8'

services:
  # 后端服务
  backend:
    build: ./college-bbs
    ports:
      - "8080:8080"
    environment:
      - DATABASE_HOST=mysql
      - DATABASE_PORT=3306
    depends_on:
      - mysql

  # 前端服务
  frontend:
    build: ./college-bbs-frontend
    ports:
      - "80:80"
    depends_on:
      - backend

  # MySQL 数据库
  mysql:
    image: mysql:8.0
    environment:
      MYSQL_ROOT_PASSWORD: your_password
      MYSQL_DATABASE: college_bbs
    ports:
      - "3306:3306"
    volumes:
      - mysql_data:/var/lib/mysql

volumes:
  mysql_data:
```

运行：

```bash
docker-compose up -d
```

## HTTPS 配置

### 使用 Let's Encrypt（免费 SSL 证书）

```bash
# 安装 Certbot
sudo apt-get update
sudo apt-get install certbot python3-certbot-nginx

# 获取证书并自动配置 Nginx
sudo certbot --nginx -d your-domain.com

# 测试自动续期
sudo certbot renew --dry-run
```

Nginx 配置会自动更新为：

```nginx
server {
    listen 443 ssl;
    server_name your-domain.com;

    ssl_certificate /etc/letsencrypt/live/your-domain.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/your-domain.com/privkey.pem;

    # ... 其他配置 ...
}

server {
    listen 80;
    server_name your-domain.com;
    return 301 https://$server_name$request_uri;
}
```

## 性能优化建议

### 1. 启用 CDN

将静态资源上传到 CDN，修改 `vite.config.js`：

```javascript
export default defineConfig({
  build: {
    rollupOptions: {
      output: {
        manualChunks: {
          'react-vendor': ['react', 'react-dom', 'react-router-dom'],
          'antd-vendor': ['antd'],
        },
      },
    },
  },
});
```

### 2. 启用 Brotli 压缩

```nginx
http {
    brotli on;
    brotli_types text/plain text/css application/json application/javascript text/xml application/xml;
}
```

### 3. 配置 HTTP/2

```nginx
server {
    listen 443 ssl http2;
    # ... 其他配置 ...
}
```

## 监控和日志

### Nginx 访问日志

```bash
# 查看实时日志
tail -f /var/log/nginx/access.log

# 查看错误日志
tail -f /var/log/nginx/error.log
```

### Docker 日志

```bash
# 查看容器日志
docker logs -f college-bbs-frontend
```

## 故障排查

### 1. 页面刷新 404

确保 Nginx 配置了 `try_files $uri $uri/ /index.html;`

### 2. API 请求跨域

检查后端 CORS 配置，确保允许前端域名

### 3. 静态资源加载失败

检查 Nginx root 路径是否正确指向 `dist` 目录

## 回滚操作

### Nginx 部署回滚

```bash
# 备份当前版本
mv /var/www/college-bbs-frontend/dist /var/www/college-bbs-frontend/dist.backup

# 恢复旧版本
mv /var/www/college-bbs-frontend/dist.old /var/www/college-bbs-frontend/dist
```

### Docker 部署回滚

```bash
# 停止当前容器
docker stop college-bbs-frontend

# 使用旧版本镜像
docker run -d -p 80:80 --name college-bbs-frontend college-bbs-frontend:v1.0.0
```

## 安全建议

1. **启用 HTTPS**：生产环境必须使用 HTTPS
2. **配置 CSP**：防止 XSS 攻击
3. **隐藏 Nginx 版本**：`server_tokens off;`
4. **限制请求频率**：防止 DDoS 攻击
5. **定期更新依赖**：修复安全漏洞

## 联系支持

遇到问题请联系开发团队或提交 Issue。

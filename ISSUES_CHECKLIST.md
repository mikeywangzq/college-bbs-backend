# 问题清单 - 快速参考

## 🔴 严重问题（必须修复）

### [ ] Issue #1: PostController::getDetail 竞态条件
- **文件**: `controllers/PostController.cc:185-232`
- **问题**: UPDATE和SELECT并行执行导致浏览次数不准确
- **优先级**: P0 - 阻塞发布
- **预计工作量**: 2小时
- **修复方法**: 在UPDATE的成功回调中执行SELECT

### [ ] Issue #2: ReplyController 数据一致性
- **文件**: `controllers/ReplyController.cc:56-86, 96-143`
- **问题**: INSERT成功但UPDATE失败时数据不一致
- **优先级**: P0 - 阻塞发布
- **预计工作量**: 4小时
- **修复方法**: 使用数据库事务保证原子性

### [ ] Issue #3: LikeController 并发竞态条件
- **文件**: `controllers/LikeController.cc:42-120`
- **问题**: 快速连续点击导致重复INSERT失败
- **优先级**: P1 - 下个迭代
- **预计工作量**: 3小时
- **修复方法**: 使用INSERT IGNORE或捕获UNIQUE约束错误

---

## 🟡 中等问题（应该修复）

### [ ] Issue #4: 测试数据密码哈希不匹配
- **文件**: `sql/init_database.sql:78-80`
- **问题**: 测试数据使用bcrypt，代码使用SHA256+Salt
- **优先级**: P0 - 阻塞测试
- **预计工作量**: 30分钟
- **修复方法**: 更新测试数据或编写工具生成正确hash

### [ ] Issue #5: LikeController 嵌套回调过深
- **文件**: `controllers/LikeController.cc`
- **问题**: 回调地狱，可读性差
- **优先级**: P2 - 技术债务
- **预计工作量**: 6小时
- **修复方法**: 使用C++20协程重构

---

## 🟢 轻微问题（建议修复）

### [ ] Issue #6: 缺少post_id <= 0 的验证
- **文件**: 多个Controller
- **问题**: 允许负数或0的ID查询数据库
- **优先级**: P2
- **预计工作量**: 1小时
- **修复方法**: 添加输入验证

### [ ] Issue #7: 错误日志可能泄露敏感信息
- **文件**: 所有Controller的错误处理
- **问题**: LOG_ERROR可能包含SQL内容
- **优先级**: P2
- **预计工作量**: 2小时
- **修复方法**: 使用错误ID系统，分离详细日志和用户消息

### [ ] Issue #8: lambda捕获生命周期（理论风险）
- **文件**: 所有Controller
- **问题**: 无（当前代码是安全的）
- **优先级**: P3 - 文档/教育
- **预计工作量**: 1小时
- **行动**: 添加代码注释说明捕获策略

---

## 修复进度跟踪

- [x] 已完成第一轮bug修复（6个bug）
- [ ] P0问题修复（3个）
- [ ] P1问题修复（1个）
- [ ] P2问题修复（4个）
- [ ] 回归测试
- [ ] 性能测试
- [ ] 代码审查

---

## 快速修复脚本

### 1. 生成测试密码
```bash
cd college-bbs/build
./generate_test_password
```

### 2. 数据一致性检查
```sql
-- 检查reply_count
SELECT p.id, p.reply_count, COUNT(r.id) as actual
FROM posts p LEFT JOIN replies r ON r.post_id = p.id
GROUP BY p.id HAVING p.reply_count != COUNT(r.id);

-- 检查like_count
SELECT p.id, p.like_count, COUNT(l.id) as actual
FROM posts p LEFT JOIN post_likes l ON l.post_id = p.id
GROUP BY p.id HAVING p.like_count != COUNT(l.id);
```

### 3. 并发测试
```bash
# 测试点赞并发
ab -n 1000 -c 100 -H "Authorization: Bearer TOKEN" \
   -p like_request.json -T application/json \
   http://localhost:8080/api/like/toggle
```

---

## 相关文档

- [BUG_FIXES.md](BUG_FIXES.md) - 第一轮bug修复报告
- [DEEP_CODE_REVIEW.md](DEEP_CODE_REVIEW.md) - 深度代码审查详细报告
- [CLAUDE.md](CLAUDE.md) - AI助手开发指南

---

**最后更新**: 2025-11-16
**下次审查**: 修复P0问题后

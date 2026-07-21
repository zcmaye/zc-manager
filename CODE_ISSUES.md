# Zc管理系统 - 代码问题清单

## 一、客户端代码问题（ZcManager-client）

### 1.1 Config.cpp

**问题描述**：硬编码盘符列表，跨平台兼容性差
- 第21行：`QStringList drivers = { "F:/","E:/","D:/","C:/" };` 只支持Windows系统
- 影响：在Linux或macOS上无法正常工作

**修复建议**：
```cpp
// 使用标准路径或配置文件指定
#ifdef Q_OS_WIN
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
```

---

### 1.2 Config.cpp

**问题描述**：密码以明文形式保存到配置文件
- `LoginPage.cpp` 第117-125行：用户密码被明文保存到 `config.ini`
- 影响：安全风险，密码可能被泄露

**修复建议**：使用加密存储或仅保存用户名

---

### 1.3 HttpMgr.h

**问题描述**：参数命名与类成员冲突
- 第32行：`void get(const QUrl& url, ReqId req_id, Modules mod, const QVariantMap& queryParams = {}, const QHttpHeaders& m_headers = {});`
- 参数名 `m_headers` 与类成员命名风格冲突，容易引起混淆

**修复建议**：将参数名改为 `headers`

---

### 1.4 HttpMacro.hpp

**问题描述**：宏定义拼写错误
- 第36行：`#define HTTP_HANDER_INSERT(REQ_ID)` 应为 `HTTP_HANDLER_INSERT`（缺少字母L）
- 影响：虽然项目中使用的是错误的宏名，但命名不一致容易引起困惑

**修复建议**：统一修改为正确的拼写

---

### 1.5 HttpMgr.cpp

**问题描述**：SSL证书验证被禁用
- 第17行：`config.setPeerVerifyMode(QSslSocket::VerifyNone);`
- 影响：安全风险，容易受到中间人攻击

**修复建议**：启用SSL证书验证，在开发环境可配置跳过验证

---

### 1.6 JsonMapper.cpp

**问题描述**：大量调试输出未清理
- 多处使用 `qDebug()` 和 `qWarning()` 输出调试信息
- 影响：生产环境会产生大量日志，影响性能

**修复建议**：使用条件编译或日志框架控制调试输出

---

### 1.7 LoginPage.cpp

**问题描述**：验证码验证在客户端进行
- 第179行：`if (captcha != m_loginCaptcha.second)` 在客户端验证验证码
- 影响：安全风险，客户端验证可以被绕过

**修复建议**：验证码验证应在服务器端进行

---

## 二、服务器端代码问题（ZcManager-server）

### 2.1 App.cpp

**问题描述**：平台相关代码
- 第76行：`system("chcp 65001")` 仅在Windows上有效
- 影响：在Linux上编译通过但运行时会出错

**修复建议**：使用条件编译

---

### 2.2 Middleware.cpp

**问题描述**：手动管理内存，存在内存泄漏风险
- 第32-35行：`auto context = new UserContext;` 创建对象
- 第50-52行：`delete context;` 删除对象
- 影响：如果在中间处理过程中抛出异常，会导致内存泄漏

**修复建议**：使用智能指针 `std::unique_ptr<UserContext>`

---

### 2.3 ConnectionPool.cpp

**问题描述**：检查线程使用 detach()，资源管理不安全
- 第123行：`_check_thread.detach();`
- 影响：主线程无法等待检查线程结束，可能在程序退出时导致崩溃

**修复建议**：使用 joinable 线程，并在析构函数中正确停止和等待

---

### 2.4 ConnectionPool.cpp

**问题描述**：线程安全问题
- `checkConnectionPro()` 函数中多次获取和释放锁，可能导致竞态条件
- 第170-220行：检查连接和重新创建连接的逻辑不够严谨

**修复建议**：重构连接检查逻辑，确保线程安全

---

### 2.5 UserService.cpp

**问题描述**：事务处理不完整
- 第100-121行：`insertUser` 函数中，异常被捕获后抛出 `BusinessError`，但事务没有显式回滚
- 第123-146行、第164-178行、第180-196行：同样的问题

**修复建议**：使用RAII模式或在catch块中显式回滚事务

---

### 2.6 TokenService.hpp

**问题描述**：常量命名与实际值不符
- 第18行：`long long MINUTE_TEN{ 5 * 60 };` 注释说是"刷新token最大时长"，但值是5分钟，命名为MINUTE_TEN（10分钟）
- 影响：代码可读性差，容易引起误解

**修复建议**：重命名为 `MINUTE_FIVE` 或修改值为 `10 * 60`

---

### 2.7 TokenService.hpp

**问题描述**：时间计算逻辑错误
- 第108行：`loginUser->expireTime = (std::chrono::seconds(loginUser->loginTime) + EXPIRE_TIME).count();`
- `loginUser->loginTime` 已经是秒数，不应再次包装为 `std::chrono::seconds`

**修复建议**：
```cpp
loginUser->expireTime = loginUser->loginTime + EXPIRE_TIME.count();
```

---

### 2.8 LoginController.cpp

**问题描述**：遗留代码未清理
- 第71-96行：`#if 0` 注释掉的代码块应删除
- 影响：代码冗余，影响可读性

**修复建议**：删除注释掉的代码

---

### 2.9 BaseController.hpp

**问题描述**：optional 访问不安全
- 第59行：`return getLoginUser()->user_id;` 直接返回 optional 值
- 如果 `user_id` 为空，会调用 `std::optional::operator*` 在空值上，导致未定义行为

**修复建议**：使用 `value_or` 提供默认值或添加空值检查

---

### 2.10 BaseController.hpp

**问题描述**：异常处理重复
- 第65-80行：`getLoginUser()` 函数中，try-catch 块捕获异常后又抛出相同的异常，没有额外处理
- 影响：代码冗余

**修复建议**：简化异常处理逻辑

---

## 三、构建配置问题

### 3.1 ZcManager-client/CMakeLists.txt

**问题描述**：注释掉的配置代码
- 第10行：`#include(qt.cmake)` 被注释掉
- 影响：如果 qt.cmake 中有重要配置，会导致构建问题

**修复建议**：删除注释或恢复配置

---

### 3.2 ZcManager-server/CMakeLists.txt

**问题描述**：第三方库目录命名不一致
- 第9行：`THRID_PARTY_DIR`（拼写错误）
- 第25行：`third-party/SqlBuilder`（正确拼写）
- 影响：代码风格不一致

**修复建议**：统一命名为 `THIRD_PARTY_DIR`

---

## 四、安全问题

### 4.1 客户端 SSL 验证禁用
- 参考 1.6

### 4.2 密码明文存储
- 参考 1.3

### 4.3 验证码客户端验证
- 参考 1.8

### 4.4 服务器配置文件包含敏感信息
- `application.ini` 中包含数据库密码和 Redis 密码
- 影响：敏感信息泄露风险

**修复建议**：使用环境变量或加密配置

---

## 五、代码风格问题

### 5.1 命名风格不一致
- 混用驼峰命名（`userName`）和下划线命名（`user_name`）
- 结构体和类命名不一致

**修复建议**：统一命名规范

### 5.2 注释风格不一致
- 部分代码使用 `//` 注释，部分使用 `/* */` 注释
- 中文和英文注释混用

**修复建议**：统一注释风格，关键代码添加英文注释

### 5.3 缺少错误处理
- 多处代码直接访问 `std::optional` 的值，没有检查是否为空
- 网络请求失败时的错误处理不够完善

**修复建议**：添加完善的错误处理和空值检查

---

## 六、架构设计问题

### 6.1 单例模式滥用
- `Singleton.hpp` 被广泛使用，但部分场景不适合使用单例
- 影响：测试困难，耦合度高

**修复建议**：考虑使用依赖注入

### 6.2 全局状态管理
- `ContextHolder` 和 `GlobalSignalHub` 使用全局状态
- 影响：线程安全问题，测试困难

**修复建议**：使用更安全的状态管理方式

---

## 七、依赖管理问题

### 7.1 第三方库版本未指定
- CMakeLists.txt 中没有指定第三方库的版本
- 影响：构建不稳定，不同版本可能有兼容性问题

**修复建议**：使用 FetchContent 或指定版本

### 7.2 依赖库路径硬编码
- 部分依赖库路径硬编码在 CMakeLists.txt 中
- 影响：跨平台构建困难

**修复建议**：使用 find_package 或环境变量

---

## 八、测试问题

### 8.1 缺少单元测试
- 项目中只有 `test_mapper.h`，没有完整的测试框架
- 影响：代码质量难以保证

**修复建议**：引入测试框架（如 Google Test）

---

## 总结

| 问题类型 | 数量 | 优先级 |
|---------|------|--------|
| 安全问题 | 4 | 高 |
| 内存管理 | 2 | 高 |
| 逻辑错误 | 2 | 高 |
| 代码风格 | 3 | 中 |
| 构建配置 | 2 | 中 |
| 架构设计 | 2 | 中 |
| 依赖管理 | 2 | 低 |
| 测试缺失 | 1 | 低 |

**建议修复顺序**：
1. 先修复安全问题（SSL验证、密码存储、验证码验证）
2. 修复内存泄漏和逻辑错误
3. 清理遗留代码和调试输出
4. 统一代码风格和命名规范
5. 完善测试和文档
# Zc管理系统

> 一个基于 C++ 技术栈的企业级管理系统，模仿了前端框架若依（RuoYi）管理系统的架构设计，采用客户端-服务器分离的架构模式。

## 目录

- [项目简介](#项目简介)
- [技术栈](#技术栈)
- [功能模块](#功能模块)
- [项目结构](#项目结构)
- [快速开始](#快速开始)
- [数据库配置](#数据库配置)
- [构建说明](#构建说明)
- [使用说明](#使用说明)
- [API 接口](#api-接口)
- [代码规范](#代码规范)
- [常见问题](#常见问题)
- [贡献指南](#贡献指南)
- [许可证](#许可证)

---

## 项目简介

Zc管理系统是一个完全基于 C++ 技术栈开发的企业级管理系统，主要特点包括：

- **架构设计**：参考若依（RuoYi）管理系统，采用前后端分离架构
- **技术选型**：Qt6 客户端 + libhv 高性能服务器
- **功能完整**：包含用户、角色、菜单、部门、岗位等完整的权限管理功能
- **跨平台**：支持 Windows、Linux 等多平台部署

---

## 技术栈

### 客户端（ZcManager-client）

| 技术 | 版本 | 说明 |
|-----|------|------|
| Qt | 6.10+ | UI 框架，用于构建桌面应用 |
| C++ | 20 | 编程语言 |
| QXlsx | - | Excel 文件读写库 |
| CMake | 3.16+ | 构建工具 |

### 服务器（ZcManager-server）

| 技术 | 版本 | 说明 |
|-----|------|------|
| libhv | - | 高性能 HTTP 服务器库 |
| MySQL | 8.0+ | 关系型数据库 |
| Redis | 6.0+ | 缓存数据库（可选） |
| OpenSSL | 3.0+ | 加密库，用于 TLS/SSL 支持 |
| nlohmann-json | 3.12+ | JSON 处理库 |
| mysql-connector-c++ | 9.2+ | MySQL C++ 连接器 |
| hiredis / redis++ | - | Redis 连接器 |
| SqlBuilder | - | SQL 语句生成器 |
| C++ | 20 | 编程语言 |
| CMake | 3.16+ | 构建工具 |

---

## 功能模块

### 1. 用户管理
- 用户列表查询
- 用户新增/编辑/删除
- 用户状态管理
- 用户导入/导出（Excel）
- 用户密码重置

### 2. 角色管理
- 角色列表查询
- 角色新增/编辑/删除
- 角色菜单权限分配
- 角色用户分配

### 3. 菜单管理
- 菜单列表查询（树形结构）
- 菜单新增/编辑/删除
- 菜单权限配置

### 4. 部门管理
- 部门列表查询（树形结构）
- 部门新增/编辑/删除

### 5. 岗位管理
- 岗位列表查询
- 岗位新增/编辑/删除

### 6. 个人信息
- 昵称修改
- 密码修改
- 头像上传/修改

### 7. 系统功能
- 用户登录/退出
- 验证码验证
- 路由权限控制
- Token 认证

---

## 项目结构

```
ZcManager-Qt/
├── ZcManager-client/          # 客户端代码
│   ├── common/                # 公共模块
│   │   ├── base/             # 基础类（单例、事件、信号中心）
│   │   ├── config/           # 配置管理
│   │   ├── constants/        # 常量定义
│   │   ├── delegate/         # 委托类（按钮、复选框等）
│   │   ├── json/             # JSON 处理
│   │   ├── network/          # 网络请求管理
│   │   ├── notify/           # 通知提示框
│   │   ├── utils/            # 工具类（验证码、Excel、字符串等）
│   │   └── widgets/          # 自定义控件
│   ├── domain/               # 领域模型
│   │   ├── dto/              # 数据传输对象
│   │   ├── entity/           # 实体类
│   │   └── vo/               # 视图对象
│   ├── modules/              # 功能模块
│   │   ├── login/            # 登录模块
│   │   ├── home/             # 首页模块
│   │   ├── user/             # 用户管理
│   │   ├── role/             # 角色管理
│   │   ├── menu/             # 菜单管理
│   │   ├── dept/             # 部门管理
│   │   ├── post/             # 岗位管理
│   │   └── personal-info/    # 个人信息
│   ├── Resource/             # 资源文件（图标、图片、样式）
│   ├── thrid-party/          # 第三方库（QXlsx）
│   ├── sql/                  # SQL 脚本
│   ├── CMakeLists.txt        # CMake 配置
│   ├── main.cpp              # 入口文件
│   ├── MainWindow.cpp        # 主窗口
│   └── ContextHolder.h       # 全局上下文
│
├── ZcManager-server/         # 服务器代码
│   ├── zc-admin/             # 应用入口
│   │   ├── web/controller/   # HTTP 控制器
│   │   └── resource/         # 配置文件
│   ├── zc-system/            # 业务逻辑层
│   │   ├── system/dao/       # 数据访问层
│   │   ├── system/service/   # 业务服务层
│   │   └── system/domain/    # 领域模型
│   ├── zc-framework/         # 框架层
│   │   ├── framework/config/ # 配置管理
│   │   ├── framework/context/# 请求上下文
│   │   ├── framework/handler/# 中间件
│   │   ├── framework/server/ # HTTP 路由
│   │   └── framework/service/# 基础服务
│   ├── zc-common/            # 公共模块
│   │   ├── common/base/      # 基础类
│   │   ├── common/cache/     # 缓存管理
│   │   ├── common/domain/    # 领域模型
│   │   ├── common/exception/ # 异常处理
│   │   ├── common/sql/       # 数据库连接池
│   │   └── common/utils/     # 工具类
│   ├── third-party/          # 第三方库
│   │   ├── SqlBuilder/       # SQL 构建器
│   │   ├── json-3.12.0/      # JSON 库
│   │   ├── hv/               # libhv 库
│   │   └── hiredis/          # Redis 客户端
│   ├── sql/                  # SQL 脚本
│   └── CMakeLists.txt        # CMake 配置
│
├── CODE_ISSUES.md            # 代码问题清单
└── README.md                 # 项目说明文档
```

---

## 快速开始

### 环境要求

- **操作系统**：Windows 10+ / Linux
- **Qt 版本**：6.10 及以上（客户端）
- **编译器**：MSVC 2019+ / GCC 11+
- **CMake**：3.16+
- **MySQL**：8.0+（服务器）
- **Redis**：6.0+（可选，服务器）
- **OpenSSL**：3.0+（服务器，必需）

### 步骤 1：克隆项目

```bash
git clone <repository-url>
cd ZcManager-Qt
```

### 步骤 2：数据库初始化

```bash
# 创建数据库
mysql -u root -p
CREATE DATABASE zc-manager CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

# 导入 SQL 脚本（服务器端）
mysql -u root -p zc-manager < ZcManager-server/sql/zc-manager.sql

# 导入 SQL 脚本（客户端示例数据）
mysql -u root -p zc-manager < ZcManager-client/sql/hdy_manager_system.sql
```

### 步骤 3：配置服务器

编辑 `ZcManager-server/zc-admin/resource/application.ini`：

```ini
[app]
name=Zc-Manager
port=8080

[mysql]
host=127.0.0.1
port=3306
user=root
password=your_password
db=zc-manager
max-size=5

[redis]
host=127.0.0.1
port=6379
password=your_redis_password
db=0
```

### 步骤 4：构建服务器

```bash
cd ZcManager-server
mkdir -p out/build/x64-debug
cd out/build/x64-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug
```

### 步骤 5：构建客户端

```bash
cd ZcManager-client
mkdir -p out/build/debug
cd out/build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug
```

### 步骤 6：运行

```bash
# 启动服务器
ZcManager-server/out/build/x64-debug/zc-admin/Zc-Manager-server.exe

# 启动客户端
ZcManager-client/out/build/debug/Zc-Manager-client.exe
```

---

## 数据库配置

### MySQL 配置

服务器使用 MySQL 8.0+ 作为主数据库，数据库名称默认为 `zc-manager`。

**表结构**：

| 表名 | 说明 |
|-----|------|
| sys_user | 用户表 |
| sys_role | 角色表 |
| sys_menu | 菜单表 |
| sys_dept | 部门表 |
| sys_post | 岗位表 |
| sys_user_role | 用户角色关联表 |
| sys_user_post | 用户岗位关联表 |
| sys_role_menu | 角色菜单关联表 |

### Redis 配置（可选）

服务器支持使用 Redis 作为缓存，通过 CMake 选项启用：

```bash
cmake -DUSE_REDIS_CACHE=ON ..
```

### OpenSSL 配置（必需）

服务器依赖 OpenSSL 进行加密操作，需要确保 OpenSSL 已正确安装并配置。

**Windows 环境**：

1. 下载 OpenSSL 安装包：https://slproweb.com/products/Win32OpenSSL.html
2. 安装时选择 "Copy OpenSSL DLLs to the Windows system directory" 或记住安装路径
3. 如果**安装到非默认路径**，需要设置环境变量（经过测试会自动设置）：
   ```bash
   set OPENSSL_ROOT_DIR=F:\Tools\OpenSSL-Win64
   set OPENSSL_INCLUDE_DIR=F:\Tools\OpenSSL-Win64\include
   set OPENSSL_LIB_DIR=F:\Tools\OpenSSL-Win64\lib\VC\x64\MD
   ```

**Linux 环境**：

```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# CentOS/RHEL
sudo yum install openssl-devel

# Fedora
sudo dnf install openssl-devel
```

---

## 构建说明

### 客户端构建

**Windows（MSVC）**：

```bash
cd ZcManager-client
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

**Linux（GCC）**：

```bash
cd ZcManager-client
cmake -B build -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 服务器构建

**Windows（MSVC）**：

```bash
cd ZcManager-server
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

**Linux（GCC）**：

```bash
cd ZcManager-server
cmake -B build -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### CMake 选项

| 选项 | 默认值 | 说明 |
|-----|-------|------|
| USE_REDIS_CACHE | OFF | 是否启用 Redis 缓存 |
| CMAKE_BUILD_TYPE | Debug | 构建类型（Debug/Release） |

---

## 使用说明

### 登录

1. 启动服务器和客户端
2. 在登录页面输入用户名和密码
3. 默认管理员账号：`admin` / `123456`
4. 输入验证码，点击登录

### 用户管理

1. 登录系统后，点击左侧菜单"系统管理" -> "用户管理"
2. 在用户管理页面可以查看、新增、编辑、删除用户
3. 支持导入/导出用户数据（Excel 格式）

### 角色管理

1. 点击左侧菜单"系统管理" -> "角色管理"
2. 在角色管理页面可以管理角色信息
3. 点击"权限设置"可以分配角色的菜单权限
4. 点击"分配用户"可以为角色分配用户

### 菜单管理

1. 点击左侧菜单"系统管理" -> "菜单管理"
2. 在菜单管理页面可以管理系统菜单
3. 支持树形结构显示

### 个人信息

1. 点击右上角用户头像，选择"个人信息"
2. 在个人信息页面可以修改昵称、密码、头像

---

## API 接口

### 认证接口

| 方法 | 路径 | 说明 |
|-----|------|------|
| POST | /login | 用户登录 |
| GET | /logout | 用户退出 |
| GET | /getInfo | 获取用户信息 |
| GET | /getRouters | 获取路由信息 |

### 用户接口

| 方法 | 路径 | 说明 |
|-----|------|------|
| GET | /system/user/list | 用户列表 |
| POST | /system/user | 新增用户 |
| PUT | /system/user | 修改用户 |
| DELETE | /system/user/{userId} | 删除用户 |

### 角色接口

| 方法 | 路径 | 说明 |
|-----|------|------|
| GET | /system/role/list | 角色列表 |
| POST | /system/role | 新增角色 |
| PUT | /system/role | 修改角色 |
| DELETE | /system/role/{roleId} | 删除角色 |

### 菜单接口

| 方法 | 路径 | 说明 |
|-----|------|------|
| GET | /system/menu/list | 菜单列表 |
| POST | /system/menu | 新增菜单 |
| PUT | /system/menu | 修改菜单 |
| DELETE | /system/menu/{menuId} | 删除菜单 |

### 部门接口

| 方法 | 路径 | 说明 |
|-----|------|------|
| GET | /system/dept/list | 部门列表 |
| POST | /system/dept | 新增部门 |
| PUT | /system/dept | 修改部门 |
| DELETE | /system/dept/{deptId} | 删除部门 |

### 岗位接口

| 方法 | 路径 | 说明 |
|-----|------|------|
| GET | /system/post/list | 岗位列表 |
| POST | /system/post | 新增岗位 |
| PUT | /system/post | 修改岗位 |
| DELETE | /system/post/{postId} | 删除岗位 |

### 个人信息接口

| 方法 | 路径 | 说明 |
|-----|------|------|
| GET | /profile/avatar | 获取头像 |
| POST | /profile/avatar | 更新头像 |
| PUT | /profile/password | 修改密码 |

---

## 代码规范

### 命名规范

- **类名**：大驼峰命名（PascalCase），如 `MainWindow`
- **函数名**：小驼峰命名（camelCase），如 `getUserList`
- **变量名**：小驼峰命名（camelCase），如 `userName`
- **常量名**：全大写，下划线分隔，如 `MAX_SIZE`
- **文件命名**：与类名一致，如 `MainWindow.cpp`

### 代码风格

- 使用 4 空格缩进
- 花括号 `{` 独占一行
- 行尾不保留空格
- 中文注释使用 `//`，英文注释使用 `//` 或 `/* */`

### 异常处理

- 使用 `try-catch` 块捕获异常
- 自定义异常类继承 `std::exception`
- 异常信息使用英文描述

### 内存管理

- 使用智能指针 `std::unique_ptr` 和 `std::shared_ptr`
- 避免手动 `new/delete`
- 使用 RAII 模式管理资源

---

## 常见问题

### Q1：编译时找不到 Qt 头文件

**解决方案**：确保 Qt 环境变量已正确配置，或在 CMake 中指定 Qt 路径：

```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/qt ..
```

### Q2：数据库连接失败

**解决方案**：

1. 检查 MySQL 服务是否启动
2. 检查 `application.ini` 中的数据库配置
3. 确保数据库用户有权限访问 `zc-manager` 数据库

### Q3：Redis 连接失败

**解决方案**：

1. 检查 Redis 服务是否启动
2. 检查 `application.ini` 中的 Redis 配置
3. 如果不需要 Redis，在 CMake 中禁用：`-DUSE_REDIS_CACHE=OFF`

### Q4：客户端无法连接服务器

**解决方案**：

1. 检查服务器是否启动
2. 检查服务器端口是否正确（默认 8080）
3. 检查防火墙设置，确保端口开放

---

## 贡献指南

### 提交代码

1. Fork 项目
2. 创建功能分支：`git checkout -b feature/xxx`
3. 提交代码：`git commit -m "feat: xxx"`
4. 推送到远程：`git push origin feature/xxx`
5. 创建 Pull Request

### 代码审查

- 确保代码符合项目规范
- 添加必要的注释和文档
- 编写单元测试
- 确保所有测试通过

---

## 许可证

本项目采用 MIT 许可证，详见 LICENSE 文件。

---

## 致谢

- [RuoYi](https://gitee.com/y_project/RuoYi) - 若依管理系统，提供了优秀的架构设计参考
- [libhv](https://github.com/ithewei/libhv) - 高性能 HTTP 服务器库
- [Qt](https://www.qt.io/) - 优秀的跨平台 UI 框架
- [nlohmann-json](https://github.com/nlohmann/json) - 现代 C++ JSON 库

---

## 联系方式

如有问题或建议，请提交 Issue 或发送邮件。
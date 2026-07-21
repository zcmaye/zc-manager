-- -----------
-- 1,用户表
-- -----------
DROP TABLE IF EXISTS sys_user;
CREATE TABLE sys_user(
  id INT PRIMARY KEY AUTO_INCREMENT COMMENT 'ID',
  dept_id INT NOT NULL COMMENT '部门ID',
  user_name VARCHAR(32) UNIQUE NOT NULL COMMENT '用户名(用于登录)',
  nick_name VARCHAR(32) NOT NULL COMMENT '昵称',
  password VARCHAR(128) NOT NULL COMMENT '密码',
  phone_number VARCHAR(16) NULL DEFAULT NULL  COMMENT '手机号码',
  email VARCHAR(64) NULL DEFAULT NULL COMMENT '邮箱',
  avatar VARCHAR(128) NULL DEFAULT NULL COMMENT '头像',
  sex TINYINT DEFAULT 2 COMMENT '0-女 1-男 2-未知',
  birthday DATE NULL DEFAULT NULL COMMENT '生日',
  login_time DATETIME NULL DEFAULT NULL COMMENT '最后登录时间',
  status TINYINT DEFAULT 1 COMMENT '用户状态:0-禁用 1-启用',
  is_deleted TINYINT DEFAULT 0 COMMENT '删除标志:0-未删除 1-已删除',
  create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  create_by   VARCHAR(32) NULL DEFAULT NULL COMMENT '创建人',
  update_time DATETIME NULL DEFAULT NULL COMMENT '更新时间',
  update_by VARCHAR(32) NULL DEFAULT NULL COMMENT '更新人',
  remark VARCHAR(256) NULL DEFAULT NULL COMMENT '备注',
  UNIQUE INDEX idx_phone_number(phone_number)
);

INSERT INTO sys_user(id,dept_id,user_name,nick_name,password,phone_number,email,sex,birthday,remark) 
           VALUE(1,101,'admin','管理员','8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92','5280620','admin@gmail.com',1,'2010-04-26','我是超级无敌管理员！嘿嘿');
INSERT INTO sys_user(id,dept_id,user_name,nick_name,password,phone_number,email,sex,birthday,remark) 
           VALUE(2,101,'maye','顽石老师','8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92','5280669','zcmaye@gmail.com',1,'2010-04-26','我是萌萌哒的玩蛇老师！嘿嘿');


SELECT * FROM sys_user;

-- -----------
-- 2,菜单表
-- -----------
DROP TABLE IF EXISTS sys_menu;
CREATE TABLE sys_menu(
  menu_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '菜单ID',
  menu_name VARCHAR(32) NOT NULL UNIQUE  COMMENT '菜单名称',
  parent_id INT DEFAULT 0 COMMENT '父菜单ID',
  order_num INT DEFAULT 0 COMMENT '显示顺序',
  path VARCHAR(128) DEFAULT '' COMMENT '路由地址(如果本地页面，写模块名；如果是外连接，直接写url包含http[s]://)',
  component VARCHAR(128) DEFAULT NULL COMMENT '组件路径(此菜单对应的页面位置)',
  is_frame TINYINT DEFAULT 0  COMMENT '是否是外链(0-否 1-是)',
  menu_type CHAR(1) DEFAULT '' COMMENT '菜单类型(D-目录 M-菜单 B-按钮)',
  is_visible TINYINT DEFAULT 1 COMMENT '是否显示(0-隐藏 1-显示)',
  is_active  TINYINT DEFAULT 1 COMMENT '菜单状态(0-禁用 1-正常)',
  perms      VARCHAR(64)  DEFAULT NULL COMMENT '菜单权限标识',
  icon       VARCHAR(64)  DEFAULT '#'  COMMENT '菜单的图标',
  create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  create_by   VARCHAR(32) NULL DEFAULT NULL COMMENT '创建人',
  update_time DATETIME NULL DEFAULT NULL COMMENT '更新时间',
  update_by VARCHAR(32) NULL DEFAULT NULL COMMENT '更新人',
  remark VARCHAR(256) NULL DEFAULT NULL COMMENT '备注'
);


-- -----------
-- 初始化-菜单数据
-- -----------
-- 一级菜单
INSERT INTO sys_menu VALUE(1,'系统管理'  ,0,1,'system',NULL,0,'D',1,1,'','system',NOW(),'admin',NULL,NULL,'系统管理目录');
INSERT INTO sys_menu VALUE(2,'系统监控'  ,0,2,'monitor',NULL,0,'D',1,1,'','monitor',NOW(),'admin',NULL,NULL,'系统监控目录');
INSERT INTO sys_menu VALUE(3,'系统工具'  ,0,3,'tool',NULL,0,'D',1,1,'','tool',NOW(),'admin',NULL,NULL,'系统工具目录');
INSERT INTO sys_menu VALUE(4,'宏定义官网',0,4,'https://appmbamikht2838.h5.xiaoeknow.com',NULL,1,'D',1,1,'','guide',NOW(),'admin',NULL,NULL,'宏定义官网地址');
-- 二级级菜单
INSERT INTO sys_menu VALUE(100,'用户管理'  ,1,1,'user','system/user/index',0,'M',1,1,'system:user:list','user'      ,NOW(),'admin',NULL,NULL,'用户管理菜单');
INSERT INTO sys_menu VALUE(101,'角色管理'  ,1,2,'role','system/role/index',0,'M',1,1,'system:role:list','peoples'   ,NOW(),'admin',NULL,NULL,'角色管理菜单');
INSERT INTO sys_menu VALUE(102,'菜单管理'  ,1,3,'menu','system/menu/index',0,'M',1,1,'system:menu:list','tree-table',NOW(),'admin',NULL,NULL,'菜单管理菜单');
insert into sys_menu value(103,'部门管理'  ,1,4,'dept','system/dept/index',0,'M',1,1,'system:dept:list','tree'      ,NOW(),'admin',NULL,NULL, '部门管理菜单');
insert into sys_menu value(104,'岗位管理'  ,1,5,'post','system/post/index',0,'M',1,1,'system:post:list','post'      ,NOW(),'admin',NULL,NULL, '岗位管理菜单');
INSERT INTO sys_menu VALUE(105,'日志管理'  ,1,6,'log','',0,'D',1,1,'','log',NOW(),'admin',NULL,NULL,'日志管理菜单');
INSERT INTO sys_menu VALUE(106,'在线用户'  ,2,1,'online','system/online/index',0,'M',1,1,'system:online:list','online',NOW(),'admin',NULL,NULL,'菜单管理菜单');
-- 三级级菜单
INSERT INTO sys_menu VALUE(500,'操作日志'  ,105,1,'operlog','system/operlog/index',0,'M',1,1,'system:operlog:list','form',NOW(),'admin',NULL,NULL,'操作日志菜单');
INSERT INTO sys_menu VALUE(501,'登录日志'  ,105,2,'logininfor','system/logininfor/index',0,'M',1,1,'system:logininfor:list','logininfor',NOW(),'admin',NULL,NULL,'登录日志菜单');
-- 用户管理按钮
insert into sys_menu values('1000', '用户查询', '100', '1',  '', '',  0, 'B', 1 , 1, 'system:user:query',          '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1001', '用户新增', '100', '2',  '', '',  0, 'B', 1 , 1, 'system:user:add',            '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1002', '用户修改', '100', '3',  '', '',  0, 'B', 1 , 1, 'system:user:edit',           '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1003', '用户删除', '100', '4',  '', '',  0, 'B', 1 , 1, 'system:user:remove',         '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1004', '用户导出', '100', '5',  '', '',  0, 'B', 1 , 1, 'system:user:export',         '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1005', '用户导入', '100', '6',  '', '',  0, 'B', 1 , 1, 'system:user:import',         '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1006', '重置密码', '100', '7',  '', '',  0, 'B', 1 , 1, 'system:user:resetPwd',       '#',  NOW(),'admin', NULL, null, '');
-- 角色管理按钮                                                                                              
insert into sys_menu values('1007', '角色查询', '101', '1',  '', '',  0, 'B', 1 , 1, 'system:role:query',          '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1008', '角色新增', '101', '2',  '', '',  0, 'B', 1 , 1, 'system:role:add',            '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1009', '角色修改', '101', '3',  '', '',  0, 'B', 1 , 1, 'system:role:edit',           '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1010', '角色删除', '101', '4',  '', '',  0, 'B', 1 , 1, 'system:role:remove',         '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1011', '角色导出', '101', '5',  '', '',  0, 'B', 1 , 1, 'system:role:export',         '#',  NOW(),'admin', NULL, null, '');
-- 菜单管理按钮                                                                                                
insert into sys_menu values('1012', '菜单查询', '102', '1',  '', '',  0, 'B', 1 , 1, 'system:menu:query',          '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1013', '菜单新增', '102', '2',  '', '',  0, 'B', 1 , 1, 'system:menu:add',            '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1014', '菜单修改', '102', '3',  '', '',  0, 'B', 1 , 1, 'system:menu:edit',           '#',  NOW(),'admin', NULL, null, '');
insert into sys_menu values('1015', '菜单删除', '102', '4',  '', '',  0, 'B', 1 , 1, 'system:menu:remove',         '#',  NOW(),'admin', NULL, null, '');
-- 部门管理按钮
insert into sys_menu values('1016', '部门查询', '103', '1',  '', '',  0, 'B', 1 , 1, 'system:dept:query',          '#',  NOW(),'admin', NULL, NULL, '');
insert into sys_menu values('1017', '部门新增', '103', '2',  '', '',  0, 'B', 1 , 1, 'system:dept:add',            '#',  NOW(),'admin', NULL, NULL, '');
insert into sys_menu values('1018', '部门修改', '103', '3',  '', '',  0, 'B', 1 , 1, 'system:dept:edit',           '#',  NOW(),'admin', NULL, NULL, '');
insert into sys_menu values('1019', '部门删除', '103', '4',  '', '',  0, 'B', 1 , 1, 'system:dept:remove',         '#',  NOW(),'admin', NULL, NULL, '');
-- 岗位管理按钮 B1 ,  NOW()NULL, NULL'admin', 
insert into sys_menu values('1020', '岗位查询', '104', '1',  '', '',  0, 'B', 1 , 1, 'system:post:query',          '#',  NOW(),'admin', NULL, NULL, '');
insert into sys_menu values('1021', '岗位新增', '104', '2',  '', '',  0, 'B', 1 , 1, 'system:post:add',            '#',  NOW(),'admin', NULL, NULL, '');
insert into sys_menu values('1022', '岗位修改', '104', '3',  '', '',  0, 'B', 1 , 1, 'system:post:edit',           '#',  NOW(),'admin', NULL, NULL, '');
insert into sys_menu values('1023', '岗位删除', '104', '4',  '', '',  0, 'B', 1 , 1, 'system:post:remove',         '#',  NOW(),'admin', NULL, NULL, '');
insert into sys_menu values('1024', '岗位导出', '104', '5',  '', '',  0, 'B', 1 , 1, 'system:post:export',         '#',  NOW(),'admin', NULL, NULL, '');



-- -----------
-- 3,角色表
-- -----------
DROP TABLE IF EXISTS sys_role;
CREATE TABLE sys_role(
  role_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '菜单ID',
  role_name VARCHAR(32) NOT NULL COMMENT '角色名称',
  role_key VARCHAR(64) NOT NULL COMMENT '角色权限字符',
  role_sort TINYINT NOT NULL COMMENT '显示排序',
  is_active TINYINT NOT NULL DEFAULT 1 COMMENT '角色状态 0-禁用 1-正常',
  is_deleted TINYINT NOT NULL DEFAULT 0 COMMENT '删除标志 0-已删除 1-正常',
  create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  create_by   VARCHAR(32) NULL DEFAULT NULL COMMENT '创建人',
  update_time DATETIME NULL DEFAULT NULL COMMENT '更新时间',
  update_by VARCHAR(32) NULL DEFAULT NULL COMMENT '更新人',
  remark VARCHAR(256) NULL DEFAULT NULL COMMENT '备注'
);

-- -----------
-- 初始化-角色数据
-- -----------
INSERT INTO sys_role VALUES(1,'管理员','admin',1,1,0,NOW(),'admin',NULL,NULL,'拥有所有权限');
INSERT INTO sys_role VALUES(2,'普通角色','common',2,1,0,NOW(),'admin',NULL,NULL,'拥有指定的权限');


-- -----------
-- 4,角色-菜单关联表 角色1-N菜单
-- -----------
DROP TABLE IF EXISTS sys_role_menu;
CREATE TABLE sys_role_menu(
  role_id BIGINT NOT NULL COMMENT '角色ID',
  menu_id BIGINT NOT NULL COMMENT '菜单ID',
  PRIMARY KEY(role_id,menu_id)
);

-- -----------
-- 初始化-角色和菜单关联表数据
-- -----------
INSERT INTO sys_role_menu VALUES(2,1);
INSERT INTO sys_role_menu VALUES(2,100);
INSERT INTO sys_role_menu VALUES(2,1000);
INSERT INTO sys_role_menu VALUES(2,1001);
INSERT INTO sys_role_menu VALUES(2,101);
INSERT INTO sys_role_menu VALUES(2,1007);
INSERT INTO sys_role_menu VALUES(2,1008);
INSERT INTO sys_role_menu VALUES(2,1010);
INSERT INTO sys_role_menu VALUES(2,1011);
INSERT INTO sys_role_menu VALUES(2,102);
INSERT INTO sys_role_menu VALUES(2,1012);
INSERT INTO sys_role_menu VALUES(2,103);
INSERT INTO sys_role_menu VALUES(2,1016);
INSERT INTO sys_role_menu VALUES(2,1017);
INSERT INTO sys_role_menu VALUES(2,1018);
INSERT INTO sys_role_menu VALUES(2,1019);
INSERT INTO sys_role_menu VALUES(2,104);
INSERT INTO sys_role_menu VALUES(2,1020);
INSERT INTO sys_role_menu VALUES(2,1021);
INSERT INTO sys_role_menu VALUES(2,1022);
INSERT INTO sys_role_menu VALUES(2,1023);
INSERT INTO sys_role_menu VALUES(2,1024);
INSERT INTO sys_role_menu VALUES(2,2);
INSERT INTO sys_role_menu VALUES(2,105);
INSERT INTO sys_role_menu VALUES(2,500);
INSERT INTO sys_role_menu VALUES(2,501);
INSERT INTO sys_role_menu VALUES(2,106);
INSERT INTO sys_role_menu VALUES(2,4);


-- -----------
-- 5,用户-角色关联表 角色1-N用户
-- -----------
DROP TABLE IF EXISTS sys_user_role;
CREATE TABLE sys_user_role(
  user_id BIGINT NOT NULL COMMENT '用户ID',
  role_id BIGINT NOT NULL COMMENT '角色ID',
  PRIMARY KEY(user_id,role_id)
);

-- -----------
-- 初始化-用户和角色关联表数据
-- -----------
INSERT INTO sys_user_role VALUES(1,1);
INSERT INTO sys_user_role VALUES(2,2);


-- -----------
-- 6,部门表
-- -----------
DROP TABLE IF EXISTS sys_dept;
CREATE TABLE sys_dept(
  dept_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '部门ID',
  parent_id INT DEFAULT 0 COMMENT '父部门ID',
  ancestors VARCHAR(32) DEFAULT '' COMMENT '祖级列表',
  dept_name VARCHAR(32) NOT NULL UNIQUE  COMMENT '部门名称',
  order_num INT DEFAULT 0 COMMENT '显示顺序',
  leader VARCHAR(32) DEFAULT NULL COMMENT '负责人',
  phone VARCHAR(32) DEFAULT NULL COMMENT '联系电话',
  email VARCHAR(64) DEFAULT NULL COMMENT '邮箱',
  is_deleted TINYINT DEFAULT 0 COMMENT '是否删除(0-没有 1-删除了)',
  is_active  TINYINT DEFAULT 1 COMMENT '部门状态(0-禁用 1-正常)',
  create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  create_by   VARCHAR(32) NULL DEFAULT NULL COMMENT '创建人',
  update_time DATETIME NULL DEFAULT NULL COMMENT '更新时间',
  update_by VARCHAR(32) NULL DEFAULT NULL COMMENT '更新人',
  remark VARCHAR(256) NULL DEFAULT NULL COMMENT '备注'
);
-- -----------
-- 初始化-部门表数据
-- -----------
INSERT INTO sys_dept VALUES(100,0,'0','宏定义科技',0,'微笑老师','15888888888','hongdingyi@qq.com',0,1,NOW(),'admin',NULL,NULL,'整个公司');
INSERT INTO sys_dept VALUES(101,100,'0,100','长沙总公司',1,'微笑老师','15888888888','hongdingyi@qq.com',0,1,NOW(),'admin',NULL,NULL,'');
INSERT INTO sys_dept VALUES(102,100,'0,100','长沙分公司',2,'微笑老师','15888888888','hongdingyi@qq.com',0,1,NOW(),'admin',NULL,NULL,'');
INSERT INTO sys_dept VALUES(103,101,'0,100,101','C/C++部门',1,'顽石老师','15888888888','hongdingyi@qq.com',0,1,NOW(),'admin',NULL,NULL,'');
INSERT INTO sys_dept VALUES(104,101,'0,100,101','Python部门',2,'微笑老师','15888888888','hongdingyi@qq.com',0,1,NOW(),'admin',NULL,NULL,'');

-- -----------
-- 7,岗位表
-- -----------
DROP TABLE IF EXISTS post;
CREATE TABLE sys_post(
  post_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '岗位ID',
  post_code VARCHAR(32) NOT NULL UNIQUE  COMMENT '岗位代码',
  post_name VARCHAR(32) NOT NULL UNIQUE  COMMENT '岗位名称',
  post_sort INT DEFAULT 0 COMMENT '显示顺序',
  is_active  TINYINT DEFAULT 1 COMMENT '岗位状态(0-禁用 1-正常)',
  create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  create_by   VARCHAR(32) NULL DEFAULT NULL COMMENT '创建人',
  update_time DATETIME NULL DEFAULT NULL COMMENT '更新时间',
  update_by VARCHAR(32) NULL DEFAULT NULL COMMENT '更新人',
  remark VARCHAR(256) NULL DEFAULT NULL COMMENT '备注'
);
-- -----------
-- 初始化-岗位表数据
-- -----------
INSERT INTO sys_post VALUE(1,'teacher','讲师',1,1,NOW(),'admin',NULL,NULL,'负责讲课和解答');
INSERT INTO sys_post VALUE(2,'sales','转化',2,1,NOW(),'admin',NULL,NULL,'负责邀请公开学员听课，转化为VIP学员');
INSERT INTO sys_post VALUE(3,'assistant','助教',3,1,NOW(),'admin',NULL,NULL,'负责发布流量视频，引流');
INSERT INTO sys_post VALUE(4,'after_sales','售后',4,1,NOW(),'admin',NULL,NULL,'负责VIP学员的权限和心理辅导');


-- -----------
-- 8,用户-岗位关联表 岗位1-N用户
-- -----------
DROP TABLE IF EXISTS sys_user_post;
CREATE TABLE sys_user_post(
  user_id BIGINT NOT NULL COMMENT '用户ID',
  post_id BIGINT NOT NULL COMMENT '岗位ID',
  PRIMARY KEY(user_id,post_id)
);


-- -----------
-- 初始化-用户和岗位关联表数据
-- -----------
INSERT INTO sys_user_post VALUES(1,1);
INSERT INTO sys_user_post VALUES(2,2);



-- SELECT LAST_INSERT_ID();
-- SELECT COUNT(*) FROM user u LEFT JOIN sys_user_role ur ON u.id = ur.user_id WHERE u.is_deleted = 0 AND ur.role_id = 2  ORDER BY u.id ASC;

  





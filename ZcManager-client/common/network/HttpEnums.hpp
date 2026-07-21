#pragma once

#include <stdint.h>
#include "common/config/Config.h"

#define URL(path) QUrl(Config::instance()->value("app/host").toString() + (path))
//#define URL(path) QUrl(QString("http://localhost:8000") + (path))

#ifdef _DEBUG
	#define	HTTPCLIENT_DEBUG  true
#else
	#define	HTTPCLIENT_DEBUG false
#endif

enum ErrorCode :int32_t {
	Success,
	Error,
	NetworkError
};

/**
 * 模块枚举.
 */
enum class Modules {
	LoginReg,		/* !>登录注册*/
	PersonalInfo,	/* !>个人信息*/
	MainWindow,		/* !>MainWindow*/
	UserMgr,		/* !>用户管理*/
	DeptMgr,		/* !>部门管理*/
	MenuMgr,		/* !>菜单管理理*/
	PostMgr,		/* !>岗位管理*/
	RoleMgr,		/* !>角色管理*/
	RoleAuthUserMgr,		/* !>角色管理*/
};

/**
 * 请求ID.
 */
enum class ReqId
{
	None,

	/** 登录注册模块 */
    Login,
	Register,
	GetInfo,
	GetRouters,

	/** 个人薪资模块*/
	profile,
	profile_reset_pwd,
	profile_avatar_upload,

	/** MainWindow*/
	avatar_get,

	/** 用户管理*/
	user_list,
	user_add,
	user_edit,
	user_del,
	user_update_status,
	user_get_by_id,

	/** 部门管理*/
	dept_tree,
	dept_add,
	dept_edit,
	dept_del,
	dept_update_status,
	dept_treeselect,

	/** 菜单管理*/
	menu_tree,
	menu_add,
	menu_edit,
	menu_del,
	menu_update_status,
	menu_treeselect,

	/** 岗位管理*/
	post_list,
	post_add,
	post_edit,
	post_del,
	post_update_status,
	post_get_by_id,

	/** 角色管理*/
	role_list,
	role_add,
	role_edit,
	role_del,
	role_update_status,
	role_get_by_id,
	role_get_checked_keys,

	/** 分配用户角色管理*/
	role_auth_user_allocated_list,
	role_auth_user_unallocated_list,
	role_auth_user_cancel,
	role_auth_user_cancel_all,
	role_auth_user_select_all,
};

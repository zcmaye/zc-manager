#pragma once

#include <QWidget>
#include "ui_UserEditDlg.h"
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class UserEditDlgClass; };
QT_END_NAMESPACE

struct TreeSelect;
struct Post;
struct Role;
struct User;

class UserEditDlg : public QWidget
{
	Q_OBJECT

public:
	UserEditDlg(QWidget *parent = nullptr);
	~UserEditDlg();

	void clear();
	/**
	 * 设置部门选择树.
	 */
	void setDeptTreeSelect(const QList<std::shared_ptr<TreeSelect>>& deptTreeSelect);

	/**
	 * 设置岗位选择.
	 */
	void setPosts(const QList<std::shared_ptr<Post>>& posts);
	/**
	 * 设置角色选择.
	 */
	void setRoles(const QList<std::shared_ptr<Role>>& roles);
	/**
	 * 设置选择的岗位.
	 */
	void setSelectPosts(const QList<qint32>& postIds);
	/**
	 * 设置选择的角色.
	 */
	void setSelectRoles(const QList<qint32>& roleIds);

	/**
	 * 设置修改的用户.
	 */
	void setUser(const std::shared_ptr<User>& user);
public slots:
	void on_okBtn_clicked();
private:
	Ui::UserEditDlgClass *ui;
	std::shared_ptr<User> m_user;
};


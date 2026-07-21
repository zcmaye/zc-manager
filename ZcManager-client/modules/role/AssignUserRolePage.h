#pragma once

#include <QWidget>
#include "ui_AssignUserRolePage.h"
#include <QStandardItemModel>
#include <QPointer>
#include "common/network/HttpMgr.h"

class MessageBox;
class AssignUserRoleAddDlg;
struct User;
struct Role;

QT_BEGIN_NAMESPACE
namespace Ui { class AssignUserRolePageClass; };
QT_END_NAMESPACE

class AssignUserRolePage : public QWidget
{
	Q_OBJECT

public:
	AssignUserRolePage(QWidget *parent = nullptr);
	~AssignUserRolePage();

	void setRole(const std::shared_ptr<Role>& role);
public slots:
	void on_resetBtn_clicked();
	void on_searchBtn_clicked();

	void on_addUserBtn_clicked();
	void on_cancelAuthBtn_clicked();
	void on_shutBtn_clicked();

	void on_hideSearchBarBtn_clicked();
	void on_refreshBtn_clicked();
private:
	Ui::AssignUserRolePageClass *ui;
	QStandardItemModel* m_model{};
	int m_checkedCount{0};

	QPointer<MessageBox> m_delMsgBox;
	QPointer<MessageBox> delMsgBox();
	QPointer<AssignUserRoleAddDlg> m_assignUserRoleAddDlg;

	std::shared_ptr<Role> m_role;

	QVariantMap searchParams();

	HTTP_HANDLER_DECL(auth_role_user_mgr);
};


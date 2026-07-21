#pragma once

#include <QWidget>
#include "ui_RoleMgrPage.h"
#include <QStandardItemModel>
#include <QPointer>
#include "common/network/HttpMgr.h"

class ButtonDelegate;
class SwitchDelegate;
class MessageBox;
struct Role;
class RoleAddDlg;
class RoleEditDlg;

QT_BEGIN_NAMESPACE
namespace Ui { class RoleMgrPageClass; };
QT_END_NAMESPACE

class RoleMgrPage : public QWidget
{
	Q_OBJECT

public:
	RoleMgrPage(QWidget *parent = nullptr);
	~RoleMgrPage();

public slots:
	void on_resetBtn_clicked();
	void on_searchBtn_clicked();

	void on_addBtn_clicked();
	void on_alterBtn_clicked();
	void on_delBtn_clicked();
	void on_exportBtn_clicked();

	void on_hideSearchBarBtn_clicked();
	void on_refreshBtn_clicked();

	void slot_update_alter_del();
	void slot_assignUserRolePage();
private:
	Ui::RoleMgrPageClass *ui;
	std::shared_ptr<Role> currentIndexRole()const;
	QStandardItemModel* m_model{};
	int m_checkedCount{0};

	QPointer<MessageBox> m_delMsgBox;
	QPointer<MessageBox> delMsgBox();

	QPointer<RoleAddDlg> m_roleAddDlg;
	QPointer<RoleEditDlg> m_roleEditDlg;

	ButtonDelegate* m_buttonDelegate;
	SwitchDelegate* m_switchDelegate;

	QVariantMap searchParams();

	HTTP_HANDLER_DECL(role_mgr);
};


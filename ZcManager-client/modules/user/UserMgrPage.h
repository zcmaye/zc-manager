#pragma once

#include <QWidget>
#include "ui_UserMgrPage.h"
#include <QStandardItemModel>
#include <QPointer>
#include "common/network/HttpMgr.h"

class XlsxUploadDlg;
class UserAddDlg;
class UserEditDlg;
class ButtonDelegate;
class SwitchDelegate;
class MessageBox;
struct User;

QT_BEGIN_NAMESPACE
namespace Ui { class UserMgrPageClass; };
QT_END_NAMESPACE

class UserMgrPage : public QWidget
{
	Q_OBJECT

public:
	UserMgrPage(QWidget *parent = nullptr);
	~UserMgrPage();

public slots:
	void on_resetBtn_clicked();
	void on_searchBtn_clicked();

	void on_addBtn_clicked();
	void on_alterBtn_clicked();
	void on_delBtn_clicked();
	void on_importBtn_clicked();
	void on_exportBtn_clicked();

	void on_hideSearchBarBtn_clicked();
	void on_refreshBtn_clicked();

	void slot_update_user(const std::shared_ptr<User>& menu);
	void slot_update_alter_del();
private:
	Ui::UserMgrPageClass *ui;
	QStandardItemModel* m_model{};
	int m_checkedCount{0};

	QPointer<MessageBox> m_delMsgBox;
	QPointer<MessageBox> delMsgBox();

	QPointer<MessageBox> m_exportMsgBox;
	QPointer<MessageBox> exportMsgBox();

	ButtonDelegate* m_buttonDelegate{};
	SwitchDelegate* m_switchDelegate{};

	QPointer<UserAddDlg> m_userAddDlg;
	QPointer<UserEditDlg> m_userEditDlg;
	QPointer<XlsxUploadDlg> m_xlsxUploadDlg;

	std::shared_ptr<User> currentIndexUser() const;
	std::shared_ptr<User> searchUser() const;
	QVariantMap searchParams() const;

	HTTP_HANDLER_DECL(user_mgr);
};


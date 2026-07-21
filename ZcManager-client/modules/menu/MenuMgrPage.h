#pragma once

#include <QWidget>
#include "ui_MenuMgrPage.h"
#include <QStandardItemModel>
#include <QPointer>
#include "common/network/HttpMgr.h"

class ButtonDelegate;
class MenuAddDlg;
class MenuEditDlg;
class MessageBox;
struct Menu;

QT_BEGIN_NAMESPACE
namespace Ui { class MenuMgrPageClass; };
QT_END_NAMESPACE

class MenuMgrPage : public QWidget
{
	Q_OBJECT

public:
	MenuMgrPage(QWidget *parent = nullptr);
	~MenuMgrPage();

public slots:
	void on_resetBtn_clicked();
	void on_searchBtn_clicked();
	void on_addBtn_clicked(const std::shared_ptr<Menu>& menu = nullptr);
	void on_expandBtn_clicked(bool checked);
	void on_hideSearchBarBtn_clicked();
	void on_refreshBtn_clicked();

	void slot_update_menu(const std::shared_ptr<Menu>& menu);
private:
	Ui::MenuMgrPageClass *ui;
	QStandardItemModel* m_model{};

	QPointer<MenuAddDlg> m_menuAddDlg;
	QPointer<MenuEditDlg> m_menuEditDlg;

	QPointer<MessageBox> m_delMsgBox;
	QPointer<MessageBox> delMsgBox();

	ButtonDelegate* m_buttonDelegate{};

	QVariantMap searchParams();

	HTTP_HANDLER_DECL(menu_mgr);
};


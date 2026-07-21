#pragma once

#include <QWidget>
#include "ui_DeptMgrPage.h"
#include <QStandardItemModel>
#include <QPointer>
#include "common/network/HttpMgr.h"

class ButtonDelegate;
class DeptAddDlg;
class DeptEditDlg;
class MessageBox;
struct Dept;

QT_BEGIN_NAMESPACE
namespace Ui { class DeptMgrPageClass; };
QT_END_NAMESPACE

class DeptMgrPage : public QWidget
{
	Q_OBJECT

public:
	DeptMgrPage(QWidget *parent = nullptr);
	~DeptMgrPage();

	void updateDeptTreeSelect();
public slots:
	void on_resetBtn_clicked();
	void on_searchBtn_clicked();
	void on_addBtn_clicked(const std::shared_ptr<Dept>& dept = nullptr);
	void on_expandBtn_clicked(bool checked);
	void on_hideSearchBarBtn_clicked();
	void on_refreshBtn_clicked();

	void slot_update_dept(const std::shared_ptr<Dept>& dept);
private:
	Ui::DeptMgrPageClass *ui;
	QStandardItemModel* m_model{};

	QPointer<DeptAddDlg> m_deptAddDlg;
	QPointer<DeptEditDlg> m_deptEditDlg;

	QPointer<MessageBox> m_delMsgBox;
	QPointer<MessageBox> delMsgBox();

	ButtonDelegate* m_buttonDelegate{};

	QVariantMap searchParams() const;

	HTTP_HANDLER_DECL(dept_mgr);
};


#pragma once

#include <QWidget>
#include "ui_PostMgrPage.h"
#include <QStandardItemModel>
#include <QPointer>
#include "common/network/HttpMgr.h"

class ButtonDelegate;
class MessageBox;
struct Post;
class PostAddDlg;
class PostEditDlg;

QT_BEGIN_NAMESPACE
namespace Ui { class PostMgrPageClass; };
QT_END_NAMESPACE

class PostMgrPage : public QWidget
{
	Q_OBJECT

public:
	PostMgrPage(QWidget *parent = nullptr);
	~PostMgrPage();

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
private:
	Ui::PostMgrPageClass *ui;
	std::shared_ptr<Post> currentIndexPost()const;
	QStandardItemModel* m_model{};
	int m_checkedCount{0};

	QPointer<MessageBox> m_delMsgBox;
	QPointer<MessageBox> delMsgBox();

	QPointer<PostAddDlg> m_postAddDlg;
	QPointer<PostEditDlg> m_postEditDlg;

	ButtonDelegate* m_buttonDelegate;

	QVariantMap searchParams();

	HTTP_HANDLER_DECL(post_mgr);
};


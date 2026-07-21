#pragma once

#include <QWidget>
#include "ui_UserAddDlg.h"
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class UserAddDlgClass; };
QT_END_NAMESPACE

struct TreeSelect;
struct Post;
struct Role;

class UserAddDlg : public QWidget
{
	Q_OBJECT

public:
	UserAddDlg(QWidget *parent = nullptr);
	~UserAddDlg();

	void clear();
	void setDeptTreeSelect(const QList<std::shared_ptr<TreeSelect>>& deptTreeSelect);

	void setPosts(const QList<std::shared_ptr<Post>>& posts);
	void setRoles(const QList<std::shared_ptr<Role>>& roles);
public slots:
	void on_okBtn_clicked();
private:
	Ui::UserAddDlgClass *ui;
};


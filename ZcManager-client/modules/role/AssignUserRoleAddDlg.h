#pragma once

#include <QWidget>
#include "ui_AssignUserRoleAddDlg.h"
#include <QStandardItemModel>
#include <QPointer>
#include "domain/dto/PagingDto.hpp"

class MessageBox;
struct User;
struct Role;

QT_BEGIN_NAMESPACE
namespace Ui { class AssignUserRoleAddDlgClass; };
QT_END_NAMESPACE

class AssignUserRoleAddDlg : public QWidget
{
	Q_OBJECT

public:
	AssignUserRoleAddDlg(QWidget *parent = nullptr);
	~AssignUserRoleAddDlg();

	void setRole(const std::shared_ptr<Role>& role);
	void setUnallocatedList(const std::shared_ptr<PagingDto<User>>& list);
public slots:
	void on_resetBtn_clicked();
	void on_searchBtn_clicked();
	void on_okBtn_clicked();

private:
	Ui::AssignUserRoleAddDlgClass *ui;
	QStandardItemModel* m_model{};
	int m_checkedCount{0};

	std::shared_ptr<Role> m_role;
	//获取选中的菜单id
	QList<int> getCheckUserIds()const;

	QVariantMap searchParams()const;
};


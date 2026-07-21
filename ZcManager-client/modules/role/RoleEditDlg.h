#pragma once
#include <memory>
#include <utility>
#include <qabstractitemmodel.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qset.h>
#include <qtconfigmacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qevent.h>
#include <qwidget.h>

#include <QWidget>
#include "ui_RoleEditDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RoleEditDlgClass; };
QT_END_NAMESPACE

struct TreeSelect;
struct Role;
class QStandardItemModel;
class QStandardItem;


class RoleEditDlg : public QWidget
{
	Q_OBJECT

public:
	RoleEditDlg(QWidget *parent = nullptr);
	~RoleEditDlg();

	void clear();
	void setMenuTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelects);
	void setRole(const std::shared_ptr<Role>& role);
	void setCheckedKeys(const QSet<qint32>& checkedKeys);
protected:
	void resizeEvent(QResizeEvent* ev)override;
public slots:
	void on_expandChx_checkStateChanged(Qt::CheckState state);
	void on_selectAllChx_checkStateChanged(Qt::CheckState state);
	void on_okBtn_clicked();

	void slot_state_changed(Qt::CheckState state,const QModelIndex& index);
private:
	Ui::RoleEditDlgClass *ui;
	QStandardItemModel* m_model{};
	std::shared_ptr<Role> m_role;

	//获取兄弟节点总数和选中的数量
	std::tuple<int, int,int> siblingItemCount(QStandardItem* item)const;
	//设置父item的选中状态
	void recursionParentFn(QStandardItem* item, Qt::CheckState state);
	//获取选中的菜单id
	QList<int> getCheckMenuIds()const;

	int hitHeight();
	int recursionHeight(QStandardItem* item);
};


#pragma once

#include <QWidget>
#include "ui_RoleAddDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RoleAddDlgClass; };
QT_END_NAMESPACE

struct TreeSelect;
class QStandardItemModel;
class QStandardItem;

class RoleAddDlg : public QWidget
{
	Q_OBJECT

public:
	RoleAddDlg(QWidget *parent = nullptr);
	~RoleAddDlg();

	void clear();
	void setMenuTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelects);
protected:
	void resizeEvent(QResizeEvent* ev)override;
public slots:
	void on_expandChx_checkStateChanged(Qt::CheckState state);
	void on_selectAllChx_checkStateChanged(Qt::CheckState state);
	void on_okBtn_clicked();

	void slot_state_changed(Qt::CheckState state,const QModelIndex& index);
private:
	Ui::RoleAddDlgClass *ui;
	QStandardItemModel* m_model{};

	//获取兄弟节点总数和选中的数量
	std::tuple<int, int,int> siblingItemCount(QStandardItem* item)const;
	//设置父item的选中状态
	void recursionParentFn(QStandardItem* item, Qt::CheckState state);
	//获取选中的菜单id
	QList<int> getCheckMenuIds()const;

	int hitHeight();
	int recursionHeight(QStandardItem* item);
};


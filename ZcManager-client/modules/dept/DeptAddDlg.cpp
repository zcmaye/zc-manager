#include "DeptAddDlg.h"
#include "common/constants/UserConstants.hpp"
#include "common/widgets/IconSelectView.h"
#include "common/notify/NotifyTipManager.h"
#include "common/network/HttpMgr.h"

#include "domain/entity/Dept.hpp"
#include "domain/vo/TreeSelect.hpp"

#include <QTreeView>
#include <QStandardItemModel>
#include <QHeaderView>


DeptAddDlg::DeptAddDlg(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DeptAddDlgClass())
{
	ui->setupUi(this);
}

DeptAddDlg::~DeptAddDlg()
{
	delete ui;
}

template<typename T>
static void recursionFn(const QList<std::shared_ptr<TreeSelect>>& treeSelect, T* parentItem)
{
	for (auto& ts : treeSelect) {
		auto item = new QStandardItem(ts->label);
		item->setData(QVariant::fromValue(ts), Qt::UserRole);
		recursionFn(ts->children, item);
		parentItem->appendRow(item);
	}
}

void DeptAddDlg::setDeptTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect)
{
	auto m = dynamic_cast<QStandardItemModel*>(ui->parentDeptCbx->model());
	if (!m) {
		qWarning() << "model is nullptr!";
		return;
	}
	recursionFn(treeSelect, m);
}

void DeptAddDlg::setDept(const std::shared_ptr<Dept>& dept)
{
	if (!dept) {
		qWarning() << "dept is nullptr";
		return;
	}
	//设置父部门
	if (dept->parent_id == 0) {
		auto index = ui->parentDeptCbx->model()->index(0, 0);
		if(index.isValid())
			ui->parentDeptCbx->setCurrentIndex(index);
	}
	else {
		auto index = ui->parentDeptCbx->findData(QVariant::fromValue(dept),
			[](const QVariant& d, const QVariant& userData)
			{
				auto dDept = d.value<std::shared_ptr<TreeSelect>>();
				auto sDept = userData.value<std::shared_ptr<Dept>>();
				if (!dDept || !sDept) {
					return false;
				}
				return dDept->id == sDept->dept_id;
			});
		if (index.isValid()) {
			ui->parentDeptCbx->setCurrentIndex(index);
		}
	}
}



void DeptAddDlg::clear()
{
	ui->parentDeptCbx->clear();

	ui->deptNameEdit->clear();
	ui->orderNumBox->setValue(0);
	ui->leaderEdit->clear();
	ui->phoneEdit->clear();
	ui->emailEdit->clear();
	ui->statusRBtn_1->setChecked(true);
}

void DeptAddDlg::on_okBtn_clicked()
{
	auto deptName= ui->deptNameEdit->text();
	auto orderNum = ui->orderNumBox->value();
	auto leader = ui->leaderEdit->text();
	auto phone= ui->phoneEdit->text();
	auto email = ui->emailEdit->text();

	if (orderNum <= 0) {
		NotifyTipManager::instance()->addNotifyTip("显示排序不能为负数或0");
		return;
	}
	if (deptName.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("部门名不能为空");
		return;
	}

	QJsonObject jdept;

	auto parent = ui->parentDeptCbx->currentData(Qt::UserRole).value<std::shared_ptr<TreeSelect>>();
	if (!parent)
		jdept["parent_id"] = 100;
	else
		jdept["parent_id"] = parent->id;

	jdept["order_num"] = orderNum;
	jdept["dept_name"] = deptName;
	jdept["is_active"] = ui->statusRBtn_0->isChecked() ? 0 : 1;
	jdept["leader"] = leader;
	jdept["phone"] = phone;
	jdept["email"] = email;

	HttpMgr::instance()->post(URL("/dept/add"), jdept, ReqId::dept_add, Modules::DeptMgr);
}

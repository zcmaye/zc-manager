#include "DeptEditDlg.h"
#include "DeptEditDlg.h"
#include "common/constants/UserConstants.hpp"
#include "common/notify/NotifyTipManager.h"
#include "common/widgets/IconSelectView.h"
#include "common/network/HttpMgr.h"

#include "domain/entity/Dept.hpp"
#include "domain/vo/TreeSelect.hpp"

#include <QTreeView>
#include <QStandardItemModel>
#include <QHeaderView>


DeptEditDlg::DeptEditDlg(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DeptEditDlgClass())
{
	ui->setupUi(this);
}

DeptEditDlg::~DeptEditDlg()
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

void DeptEditDlg::setDeptTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect)
{
	auto m = dynamic_cast<QStandardItemModel*>(ui->parentDeptCbx->model());
	if (!m) {
		qWarning() << "model is nullptr!";
		return;
	}
	recursionFn(treeSelect, m);
}

/*void DeptEditDlg::setParentDept(const std::shared_ptr<Dept>& dept)
{
	if (!dept)
		return;

	auto w = ui->parentDeptCbx;

	auto index = w->findText(dept->dept_name,Qt::MatchFlag::MatchRecursive);
	if (!index.isValid()) {
		index = w->findData(QVariant::fromValue(dept), [](const QVariant& left, const QVariant& right) {
			return left == right;
			});
		if (index.isValid()) {
			qWarning() << dept->dept_name << "not found!";
			return;
		}
	}

	w->setCurrentIndex(index);
}*/

void DeptEditDlg::setDept(const std::shared_ptr<Dept>& dept)
{
	m_dept = dept;
	if (!m_dept) {
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
				return dDept->id == sDept->parent_id;
			});
		if (index.isValid()) {
			ui->parentDeptCbx->setCurrentIndex(index);
		}
	}

	ui->deptNameEdit->setText(dept->dept_name);
	ui->orderNumBox->setValue(dept->order_num);
	ui->leaderEdit->setText(dept->leader);
	ui->phoneEdit->setText(dept->phone);
	ui->emailEdit->setText(dept->email);
	if (m_dept->is_active)
		ui->statusRBtn_1->setChecked(true);
	else
		ui->statusRBtn_0->setChecked(true);
}

void DeptEditDlg::clear()
{
	ui->parentDeptCbx->clear();

	ui->deptNameEdit->clear();
	ui->orderNumBox->setValue(0);
	ui->leaderEdit->clear();
	ui->phoneEdit->clear();
	ui->emailEdit->clear();
	ui->statusRBtn_1->setChecked(true);
}

void DeptEditDlg::on_okBtn_clicked()
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
	jdept["dept_id"] = m_dept->dept_id;

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


	HttpMgr::instance()->put(URL("/dept/edit"), jdept, ReqId::dept_edit, Modules::DeptMgr);
}

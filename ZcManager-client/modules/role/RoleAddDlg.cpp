#include "RoleAddDlg.h"

#include "domain/vo/TreeSelect.hpp"
#include "domain/entity/Role.hpp"
#include "domain/entity/User.hpp"

#include "common/delegate/CheckBoxDelegate.h"
#include "common/notify/NotifyTipManager.h"
#include "common/network/HttpMgr.h"

#include "ContextHolder.h"
#include <QStandardItemModel>


RoleAddDlg::RoleAddDlg(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::RoleAddDlgClass())
	, m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	ui->menuTreeSelectView->setModel(m_model);
	ui->menuTreeSelectView->header()->hide();
	ui->menuTreeSelectView->setEditTriggers(QTreeView::NoEditTriggers);
	connect(ui->menuTreeSelectView, &QTreeView::expanded, this, [this]() {
		ui->menuTreeSelectView->setFixedHeight(hitHeight());
		});
	connect(ui->menuTreeSelectView, &QTreeView::collapsed, this, [this]() {
		ui->menuTreeSelectView->setFixedHeight(hitHeight());
		});

	//选择委托
	auto checkboxDelegate = new CheckBoxDelegate(this);
	ui->menuTreeSelectView->setItemDelegateForColumn(0, checkboxDelegate);
	connect(checkboxDelegate, &CheckBoxDelegate::checkStateChanged, this, &RoleAddDlg::slot_state_changed);
}

RoleAddDlg::~RoleAddDlg()
{
	delete ui;
}

void RoleAddDlg::clear()
{
	ui->roleNameEdit->clear();
	ui->roleKeyEdit->clear();
	ui->roleSortBox->clear();
	ui->remarkEdit->clear();
	m_model->clear();
}

template<typename T>
static void recursionFn(const QList<std::shared_ptr<TreeSelect>>& treeSelects, T* parentItem)
{
	for (auto& menu : treeSelects) {
		auto checkedItem = new QStandardItem;
		checkedItem->setSizeHint(QSize(32,30));
		checkedItem->setData(QVariant::fromValue(menu), Qt::UserRole);
		auto item = new QStandardItem(menu->label);
		recursionFn(menu->children, checkedItem);
		parentItem->appendRow({ checkedItem ,item });
	}
}

void RoleAddDlg::setMenuTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelects)
{
	m_model->clear();
	recursionFn(treeSelects, m_model);
}

void RoleAddDlg::resizeEvent(QResizeEvent* ev)
{
	ui->menuTreeSelectView->setFixedHeight(hitHeight());
}

void RoleAddDlg::on_expandChx_checkStateChanged(Qt::CheckState state)
{
	if (state == Qt::Checked) {
		ui->menuTreeSelectView->expandAll();
	}
	else {
		ui->menuTreeSelectView->collapseAll();
		ui->menuTreeSelectView->setFixedHeight(hitHeight());
	}
}

template<typename T>
static void recursionFn(T* parentItem,Qt::CheckState state)
{
	for (int i = 0; i < parentItem->rowCount(); i++) {
		auto item = parentItem->child(i, 0);
		item->setData(state, CheckBoxDelegate::CheckStateRole);
		recursionFn(item, state);
	}
}

void RoleAddDlg::on_selectAllChx_checkStateChanged(Qt::CheckState state) 
{
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i, 0);
		item->setData(state, CheckBoxDelegate::CheckStateRole);
		recursionFn(item, state);
	}
}

void RoleAddDlg::on_okBtn_clicked()
{
	auto roleName = ui->roleNameEdit->text().trimmed();
	auto roleKey = ui->roleKeyEdit->text().trimmed();
	auto roleSort = ui->roleSortBox->value();
	auto status = ui->isActive1_RBtn->isChecked();
	auto remark = ui->remarkEdit->toPlainText().trimmed();
	auto menuIds = getCheckMenuIds();

	if (roleName.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("角色名称不能为空~");
		return;
	}

	if (roleKey.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("角色权限不能为空~");
		return;
	}

	if (roleSort <= 0) {
		NotifyTipManager::instance()->addNotifyTip("角色排序不能小于等于0~");
		return;
	}

	QJsonArray jarray;
	for (auto id : menuIds) {
		jarray.append(id);
	}

	QJsonObject jrole;
	jrole["role_name"] = roleName;
	jrole["role_key"] = roleKey;
	jrole["role_sort"] = roleSort;
	jrole["is_active"] = status;
	jrole["remark"] = remark;
	jrole["menuIds"] =  jarray;

	HttpMgr::instance()->post(URL("/role/add"), jrole, ReqId::role_add, Modules::RoleMgr);
}

void RoleAddDlg::slot_state_changed(Qt::CheckState state, const QModelIndex& index)
{
	//父子联动
	if (ui->fatherSonInteractionChx->isChecked()) {
		auto item = m_model->itemFromIndex(index);
		//选择/取消选择child
		recursionFn(item, state);
		//选择/取消选择parent
		auto tp = siblingItemCount(item);
		//所有兄弟都选中了
		if (std::get<0>(tp)== std::get<1>(tp)) {
			//选中父item
			recursionParentFn(item, Qt::Checked);
		}
		//没有全部选中
		else if (std::get<2>(tp) > 0 || std::get<1>(tp) > 0) {
			//半选父item
			recursionParentFn(item, Qt::PartiallyChecked);
		}
		//全部没有选中
		else {
			//取消选中父item
			recursionParentFn(item, Qt::Unchecked);
		}
	}
}

std::tuple<int, int,int> RoleAddDlg::siblingItemCount(QStandardItem* item) const
{
	auto parentItem =  item->parent();
	if (!parentItem) {
		return {};
	}

	int checkCount = 0;
	int pCheckCount = 0;
	for (int i = 0; i < parentItem->rowCount(); i++) {
		auto child = parentItem->child(i, 0);
		if (child ) {
			auto state = child->data(CheckBoxDelegate::CheckStateRole).value<Qt::CheckState>(); 
			if (state == Qt::Checked)
				checkCount++;
			else if (state == Qt::PartiallyChecked)
				pCheckCount++;
		}
	}

	return  { parentItem->rowCount(),checkCount,pCheckCount};
}

void RoleAddDlg::recursionParentFn(QStandardItem* item, Qt::CheckState state)
{
	auto parentItem =  item->parent();
	if (!parentItem)
		return;
	parentItem->setData(state, CheckBoxDelegate::CheckStateRole);
	//选择/取消选择parent
	auto tp = siblingItemCount(parentItem);
	//所有兄弟都选中了
	if (std::get<0>(tp) == std::get<1>(tp)) {
		//选中父item
		recursionParentFn(parentItem, Qt::Checked);
	}
	//没有全部选中
	else if (std::get<2>(tp) > 0 || std::get<1>(tp) > 0) {
		//半选父item
		recursionParentFn(parentItem, Qt::PartiallyChecked);
	}
	//全部没有选中
	else  if (std::get<2>(tp) == 0) {
		//取消选中父item
		recursionParentFn(parentItem, Qt::Unchecked);
	}
}

static void recursionMenuId(QStandardItem* item, QList<int>& menuIds)
{
	if (!item)
		return;
	for (int i = 0; i < item->rowCount(); i++) {
		auto child = item->child(i, 0);
		if (!child)
			continue;
		if (child->data(CheckBoxDelegate::CheckStateRole).value<Qt::CheckState>() != Qt::Unchecked) {
			recursionMenuId(child, menuIds);
			auto treeSelect = child->data(Qt::UserRole).value<std::shared_ptr<TreeSelect>>();
			if (treeSelect)
				menuIds.append(treeSelect->id);
		}
	}

}

QList<int> RoleAddDlg::getCheckMenuIds() const
{
	QList<int> menuIds;
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto child = m_model->item(i, 0);
		if (!child)
			continue;
		if (child->data(CheckBoxDelegate::CheckStateRole).value<Qt::CheckState>() != Qt::Unchecked) {
			recursionMenuId(child, menuIds);
			auto treeSelect = child->data(Qt::UserRole).value<std::shared_ptr<TreeSelect>>();
			if (treeSelect)
				menuIds.append(treeSelect->id);
		}
	}

	return menuIds;
}

int RoleAddDlg::hitHeight()
{
	auto m = m_model;
	if (!m) {
		auto frame = findChild<QFrame*>();
		return frame->height();
	}

	int h = 0;
	for (int i = 0; i < m->rowCount(); i++) {
		auto item = m->item(i, 0);
		h += recursionHeight(item);
	}

	return qMin(h, 400);
}

int RoleAddDlg::recursionHeight(QStandardItem* parent)
{
	int h = 0;
	h += ui->menuTreeSelectView->visualRect(parent->index()).height();

	for (int i = 0; i < parent->rowCount(); i++) {
		auto item = parent->child(i, 0);
		h += recursionHeight(item);
	}

	return h;
}



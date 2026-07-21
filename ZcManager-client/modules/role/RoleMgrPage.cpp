#include "RoleMgrPage.h"
#include "RoleAddDlg.h"
#include "RoleEditDlg.h"

#include "domain/entity/Role.hpp"
#include "domain/entity/User.hpp"
#include "domain/vo/TreeSelect.hpp"

#include "common/base/Event.hpp"
#include "common/utils/OverlayWidget.h"
#include "common/delegate/ButtonDelegate.h"
#include "common/delegate/SwitchDelegate.h"
#include "common/delegate/CheckBoxDelegate.h"
#include "common/widgets/MessageBox.h"
#include "common/widgets/CheckBoxHeaderView.h"
#include "common/widgets/FlowLayout.h"
#include "common/notify/NotifyTipManager.h"
#include "ContextHolder.h"

#include <QTimer>
#include <QRandomGenerator>
#include <QStackedWidget>
#include <QApplication>

RoleMgrPage::RoleMgrPage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::RoleMgrPageClass())
	, m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
//	updateSearchBarUi();
	Utils::SetSearchBarFlowLayout(ui->searchBar);
	ui->statusCbx->setCurrentIndex(-1);

	//ui("system:role:edit")) {
	ui->addBtn->setPermission("system:role:add");
	ui->alterBtn->setPermission("system:role:edit");
	ui->delBtn->setPermission("system:role:remove");
	ui->exportBtn->setPermission("system:role:export");

	ui->roleTableView->setModel(m_model);
	ui->roleTableView->setEditTriggers(QTableView::NoEditTriggers);
	ui->roleTableView->setSelectionMode(QTableView::NoSelection);
	ui->roleTableView->verticalHeader()->hide();
	ui->roleTableView->setShowGrid(false);
	ui->roleTableView->verticalHeader()->hide();
	ui->roleTableView->verticalHeader()->setDefaultSectionSize(50);	//设置行高

	//可选择表头
	auto headerView = new CheckBoxHeaderView;
	ui->roleTableView->setHorizontalHeader(headerView);

	auto ctx = ContextHolder::instance();

	//开关委托
	auto switchDelegate = new SwitchDelegate(this);
	m_switchDelegate = switchDelegate;
	switchDelegate->setBindValue(1, 0);
	//按钮委托
	m_buttonDelegate = new ButtonDelegate(ui->roleTableView);
	auto buttonDelegate = m_buttonDelegate;
	if (ctx->hasPermission("system:role:edit")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/pen.svg"), "修改");
	}
	if (ctx->hasPermission("system:role:remove")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/del.svg"), "删除");
	}
	if (ctx->hasPermission("system:role:edit")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/user.svg"), "分配用户");
	}
	//选择委托
	auto checkBoxDelegate = new CheckBoxDelegate(this);

	ui->roleTableView->setItemDelegateForColumn(0, checkBoxDelegate);
	ui->roleTableView->setItemDelegateForColumn(5, switchDelegate);
	ui->roleTableView->setItemDelegateForColumn(7, buttonDelegate);

	connect(headerView, &CheckBoxHeaderView::checkStateChanged, this, [this,checkBoxDelegate](Qt::CheckState state) {
		for (int i = 0; i < m_model->rowCount(); i++) {
			auto index = m_model->index(i, 0);
			checkBoxDelegate->setCheckState(index, state);
		}
		if (state == Qt::CheckState::Checked) {
			m_checkedCount = m_model->rowCount();
		}
		else if (state == Qt::Unchecked) {
			m_checkedCount = 0;
		}
		slot_update_alter_del();
		});
	connect(checkBoxDelegate, &CheckBoxDelegate::checkStateChanged, [this,headerView](Qt::CheckState state, const QModelIndex& index) {
		if (state == Qt::Checked) {
			m_checkedCount++;
		}
		else if (state == Qt::Unchecked) {
			m_checkedCount--;
		}
		if (m_checkedCount == m_model->rowCount()) {
			headerView->setCheckState(Qt::Checked);
		}
		else if(m_checkedCount == 0) {
			headerView->setCheckState(Qt::Unchecked);
		}
		else {
			headerView->setCheckState(Qt::PartiallyChecked);
		}
		slot_update_alter_del();
		});
	connect(switchDelegate, &SwitchDelegate::stateChanged, [switchDelegate](bool status, const QModelIndex& index)
		{
			qDebug() << status << index;
			auto role = index.data(Qt::UserRole).value<std::shared_ptr<Role>>();
			if (!role) {
				qWarning() << "Role Not found";
				return;
			}
			role->is_active = status;
			switchDelegate->setProperty("index", index);
			HttpMgr::instance()->put(URL("/role/updateStatus"), { {"role_id",role->role_id},{"is_active",role->is_active} },
				ReqId::role_update_status, Modules::RoleMgr);
		});
	connect(buttonDelegate, &ButtonDelegate::clicked, this, [this](int id,const QModelIndex& index,ButtonDelegate::Data *d) {
		auto role = index.data(Qt::UserRole).value<std::shared_ptr<Role>>();
		if (!role) {
			return;
		}
		if (d->text == "修改") {
			qDebug() << "修改";
			on_alterBtn_clicked();
		}
		else if (d->text == "删除") {
			qDebug() << "删除";
			auto box = delMsgBox();
			box->setMessage(QString("是否确认删除名称为\"%1\"的数据项?").arg(role->role_name));
			box->setProperty("index", index);
			OverlayWidget::instance()->popup(box);
		}
		else if(d->text == "分配用户") {
			qDebug() << "分配用户";
			slot_assignUserRolePage();
		}
		});
	connect(ui->paginBar, &PaginationToolBar::pageChanged, this, &RoleMgrPage::on_searchBtn_clicked);
	connect(ui->paginBar, &PaginationToolBar::pageSizeChanged, this, &RoleMgrPage::on_searchBtn_clicked);

	ui->statusCbx->clear();
	ui->statusCbx->addItem("正常", 1);
	ui->statusCbx->addItem("禁用", 0);

	QTimer::singleShot(0, [this] {
		on_searchBtn_clicked();
		});

	HTTP_HANDLER_INIT(role_mgr);
	if (ContextHolder::instance()->menuTreeSelect().empty()) {
		HttpMgr::instance()->get(URL("/menu/treeselect"), ReqId::menu_treeselect, Modules::RoleMgr);
	}
}

RoleMgrPage::~RoleMgrPage()
{
	if (m_roleAddDlg)
		m_roleAddDlg->deleteLater();
	if (m_roleEditDlg)
		m_roleEditDlg->deleteLater();
	if(m_delMsgBox)
		m_delMsgBox->deleteLater();

	delete ui;
}

void RoleMgrPage::on_resetBtn_clicked()
{
	ui->roleNameEdit->clear();
	ui->roleKeyEdit->clear();
	ui->statusCbx->setCurrentIndex(-1);
	ui->dateRangePicker->clear();;
	on_searchBtn_clicked();
}

static QList<QStandardItem*> GetRoleItems(const std::shared_ptr<Role>& role)
{
	QList<QStandardItem*> items;

	auto checkStateItem = items.emplaceBack(new QStandardItem);
	auto idItem = items.emplaceBack(new QStandardItem(QString::number(role->role_id)));
	auto roleNameItem = items.emplaceBack(new QStandardItem(role->role_name));
	auto roleKeyItem = items.emplaceBack(new QStandardItem(role->role_key));
	auto roleSortItem = items.emplaceBack(new QStandardItem(QString::number(role->role_sort)));
	auto statusItem = items.emplaceBack(new QStandardItem(QString::number(role->is_active)));
	auto timeItem = items.emplaceBack(new QStandardItem(role->create_time));
	auto optionItem = items.emplaceBack(new QStandardItem);
	checkStateItem->setData(QVariant::fromValue(role), Qt::UserRole);
	optionItem->setData(QVariant::fromValue(role), Qt::UserRole);
	statusItem->setData(QVariant::fromValue(role), Qt::UserRole);

	for (auto item : items) {
		item->setTextAlignment(Qt::AlignCenter);
		item->setSizeHint(QSize(0,55));
	}

	//如果是管理员
	if (role->role_id == 1 || role->role_name == "admin") {
		optionItem->setData(true, ButtonDelegate::HideRowRole);
	}
	//if (role->role_id == 9) {
	//	optionItem->setData(QVariant::fromValue(QList<int>{2}), ButtonDelegate::HideButtonRole);
	//}

	return items;
}

void RoleMgrPage::on_searchBtn_clicked()
{
	//清空数据
	m_model->clear();

	//取消表头的选择
	auto view = dynamic_cast<CheckBoxHeaderView*>(ui->roleTableView->horizontalHeader());
	view->setCheckState(Qt::Unchecked);

	//设置表头
	QStringList headers = { "","角色编号","角色名称","权限字符","显示顺序","状态","创建时间","操作" };
	m_model->setHorizontalHeaderLabels(headers);
	for (int i = 0; i < m_model->columnCount(); i++) {
		auto item = m_model->horizontalHeaderItem(i);
		if (item) {
			item->setTextAlignment(Qt::AlignCenter);
		}
	}
	//设置调整模式
	
	ui->roleTableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeMode::Stretch);
	ui->roleTableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeMode::Stretch);
	ui->roleTableView->horizontalHeader()->setSectionResizeMode(4,QHeaderView::ResizeMode::Stretch);
	ui->roleTableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeMode::Stretch);
	//ui->roleTableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeMode::Stretch);
	//ui->roleTableView->horizontalHeader()->setSectionResizeMode(6,QHeaderView::ResizeMode::Fixed);

	HttpMgr::instance()->get(URL("/role/list"), ReqId::role_list, Modules::RoleMgr, searchParams());
}

void RoleMgrPage::on_addBtn_clicked()
{
	if (!m_roleAddDlg) {
		m_roleAddDlg = new RoleAddDlg;
	}
	m_roleAddDlg->clear();
	m_roleAddDlg->setMenuTreeSelect(ContextHolder::instance()->menuTreeSelect());
	OverlayWidget::instance()->popup(m_roleAddDlg.get());
}

void RoleMgrPage::on_alterBtn_clicked()
{
	if (m_checkedCount > 1) {
		NotifyTipManager::instance()->addNotifyTip("选择了多个，只能修改一个角色数据~");
		return;
	}

	if (!m_roleEditDlg) {
		m_roleEditDlg = new RoleEditDlg;
	}
	m_roleEditDlg->clear();

	auto role = currentIndexRole();

	HttpMgr::instance()->get(URL("/menu/roleMenuTreeselect/" + QString::number(role->role_id)), ReqId::role_get_checked_keys, Modules::RoleMgr);
	m_roleEditDlg->setProperty("role",QVariant::fromValue(role));
	m_roleEditDlg->setMenuTreeSelect(ContextHolder::instance()->menuTreeSelect());
	OverlayWidget::instance()->popup(m_roleEditDlg.get());
}

void RoleMgrPage::on_delBtn_clicked()
{
	QList<int> roleIds;
	//获取所有选中的行
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i, 0);
		if (!item)
			continue;
		if (item->data(CheckBoxDelegate::CheckStateRole).toBool()) {
			auto role = item->data(Qt::UserRole).value<std::shared_ptr<Role>>();
			if (!role) {
				qWarning() << "role is nullptr";
				continue;
			}
			roleIds.append(role->role_id);
		}
	}

	QStringList roleIdList;
	std::transform(roleIds.begin(), roleIds.end(), std::back_inserter(roleIdList), [](int id) {
			return QString::number(id);
		});

	auto box = delMsgBox();
	box->setMessage(QString("是否确认删除角色编号为\"%1\"的数据项？").arg(roleIdList.join(",")));
	box->setProperty("roleIds", QVariant::fromValue(roleIdList));
	OverlayWidget::instance()->popup(box);
}

void RoleMgrPage::on_exportBtn_clicked()
{
}

void RoleMgrPage::on_hideSearchBarBtn_clicked()
{
	if (ui->searchBar->isHidden()) {
		ui->searchBar->show();
		ui->hideSearchBarBtn->setToolTip("隐藏搜索");
	}
	else {
		ui->searchBar->hide();
		ui->hideSearchBarBtn->setToolTip("显示搜索");
	}
}

void RoleMgrPage::on_refreshBtn_clicked()
{
	on_searchBtn_clicked();
}

void RoleMgrPage::slot_update_alter_del()
{
	ui->alterBtn->setEnabled(m_checkedCount == 1);
	ui->delBtn->setEnabled(m_checkedCount > 0);
}

void RoleMgrPage::slot_assignUserRolePage()
{
	QWidget* w = dynamic_cast<QStackedWidget*>(parentWidget());
	if (!w) {
		qWarning() << "stacked widget is not found";
		return;
	}
	auto parentW =  w->parentWidget();
	if (parentW) {
		auto sev = new StackedWidgetAddPageEvent("system/roleAssign/index", "分配用户", currentIndexRole());
		QApplication::postEvent(parentW, sev);
	}
	else {
		qDebug() << "parentW is nullptr";
	}
}

std::shared_ptr<Role> RoleMgrPage::currentIndexRole() const
{
	auto index =  ui->roleTableView->currentIndex();
	if (!index.isValid()) { 
		qWarning() << "index is invalid";
		return {};
	}
	auto item = m_model->item(index.row(), 0);
	if (!item) {
		qWarning() << "item is nullptr";
		return {};
	}
	return item->data(Qt::UserRole).value<std::shared_ptr<Role>>();
}

QPointer<MessageBox> RoleMgrPage::delMsgBox()
{
	if (!m_delMsgBox) {
		m_delMsgBox = new MessageBox(MessageBox::Warning);
		connect(m_delMsgBox, &MessageBox::closed, this, [this](int ret) {
			if (ret == MessageBox::AcceptRole) {
				//批量删除
				auto roleIds = m_delMsgBox->property("roleIds").toStringList();
				if (!roleIds.empty()) {
					HttpMgr::instance()->del(URL("/role/" + roleIds.join(",")), ReqId::role_del, Modules::RoleMgr);
					//取消设置
					m_delMsgBox->setProperty("roleIds", QVariant());
					//把选中数量置空
					m_checkedCount = 0;
					auto view = dynamic_cast<CheckBoxHeaderView*>(ui->roleTableView->horizontalHeader());
					view->setCheckState(Qt::Unchecked);
					//重新搜索
					on_searchBtn_clicked();
				}
				else {
					auto index = m_delMsgBox->property("index").value<QModelIndex>();
					auto role = index.data(Qt::UserRole).value<std::shared_ptr<Role>>();
					HttpMgr::instance()->del(URL("/role/" + QString::number(role->role_id)), ReqId::role_del, Modules::RoleMgr);
					}
			}
			m_delMsgBox->close();
			});
	}
	return m_delMsgBox;
}

QVariantMap RoleMgrPage::searchParams()
{
	QVariantMap params = {
		{"page",ui->paginBar->page()},
		{"pageSize",ui->paginBar->pageSize()}
	};


	//获取查询条件
	auto roleName = ui->roleNameEdit->text();
	auto roleKey = ui->roleKeyEdit->text();
	auto startDate = ui->dateRangePicker->startDate();
	auto endDate = ui->dateRangePicker->endDate();

	if (!roleName.isEmpty()) {
		params["role_name"] = roleName;
	}

	if (!roleKey.isEmpty()) {
		params["role_key"] = roleKey;
	}

	if (startDate.isValid() && endDate.isValid()) {
		//params["create_time"] = startDate.toString("yyyy-MM-dd") + "," + endDate.toString("yyyy-MM-dd");
		params["begin_time"] = startDate.toString("yyyy-MM-dd");
		params["end_time"] = endDate.toString("yyyy-MM-dd");
	}

	if (ui->statusCbx->currentIndex() !=-1) {
		params["is_active"] = ui->statusCbx->currentData().toInt();
	}

	return params;
}


HTTP_HANDLER_IMPL(RoleMgrPage, role_mgr)
{
	if (errc != ErrorCode::Success) {
		NotifyTipManager::instance()->addNotifyTip(msg, NotifyTipBox::TypeError);
		return;	
	}

	try
	{
		auto json = json_t::parse(msg);
		_handlers[req_id](json);
	}
	catch (const std::exception& e) {
		NotifyTipManager::instance()->addNotifyTip(e.what(), NotifyTipBox::TypeError);
	}

}

void RoleMgrPage::initHandlers()
{
	HTTP_HANDER_INSERT(ReqId::role_list)
	{
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
			return;
		}

		auto pagingDto = json_t(json["data"]).cast<std::shared_ptr<PagingDto<Role>>>();
		//auto pagingDto = UserService::instance()->selectUserList(filter, ui->paginBar->page(), ui->paginBar->pageSize());
		if (pagingDto->totalCount == 0) {
			NotifyTipManager::instance()->addNotifyTip("没有用户数据~", NotifyTipBox::TypeSuccess);
		}
		else {
			for (auto& role: pagingDto->items) {
				auto items = GetRoleItems(role);
				m_model->appendRow(items);
			}
		}
		ui->paginBar->setTotalRecords(pagingDto->totalCount);

		ui->roleTableView->setColumnWidth(0, 50);
		ui->roleTableView->setColumnWidth(1, 100);
		//ui->roleTableView->setColumnWidth(1, 30);
		//ui->roleTableView->setColumnWidth(2, 50);
		//ui->roleTableView->setColumnWidth(3, 120);
		//ui->roleTableView->setColumnWidth(4, 155);
		ui->roleTableView->setColumnWidth(6, 180);
		ui->roleTableView->setColumnWidth(7, 180);
		if (m_buttonDelegate->buttonCount() <= 0) {
			ui->roleTableView->setColumnHidden(m_model->columnCount() - 1, true);
		}

	});

	HTTP_HANDER_INSERT(ReqId::role_update_status) {
		if (json["code"] != 200) {
			m_switchDelegate->rollback(m_switchDelegate->property("index").toModelIndex());
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		NotifyTipManager::instance()->addNotifyTip("角色状态更新成功~", NotifyTipBox::TypeSuccess);
	});

	HTTP_HANDER_INSERT(ReqId::role_del) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		NotifyTipManager::instance()->addNotifyTip("删除成功~", NotifyTipBox::TypeSuccess);
		m_model->removeRow(m_delMsgBox->property("index").toModelIndex().row());
	});


	HTTP_HANDER_INSERT(ReqId::role_add) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		m_roleAddDlg->clear();
		m_roleAddDlg->hide();
	});

	HTTP_HANDER_INSERT(ReqId::role_edit) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		m_roleEditDlg->clear();
		m_roleEditDlg->hide();
	});

	HTTP_HANDER_INSERT(ReqId::menu_treeselect) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		auto menuTreeSelect = json_t(json["data"]).cast<QList<std::shared_ptr<TreeSelect>>>();
		if (menuTreeSelect.isEmpty()) {
			qWarning()<<"menuTreeSelect is empty";
			return;
		}

		if (m_roleAddDlg) {
			m_roleAddDlg->setMenuTreeSelect(menuTreeSelect);
		}

		if (m_roleEditDlg) {
			m_roleEditDlg->setMenuTreeSelect(menuTreeSelect);
			auto role = m_roleEditDlg->property("role").value<std::shared_ptr<Role>>();
			if (role) {
				m_roleEditDlg->setRole(role);
			}
		}
		ContextHolder::instance()->setMenuTreeSelect(menuTreeSelect);
	});

	HTTP_HANDER_INSERT(ReqId::role_get_checked_keys) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		auto treeSelect = json_t(json["menus"]).cast<QList<std::shared_ptr<TreeSelect>>>();
		auto checkedKeys = json_t(json["checkedKeys"]).cast<QSet<int>>();

		auto role = m_roleEditDlg->property("role").value<std::shared_ptr<Role>>();
		if (!role) {
			qWarning() << " role is null";
			return;
		}
		m_roleEditDlg->setRole(role);
		m_roleEditDlg->setMenuTreeSelect(treeSelect);
		m_roleEditDlg->setCheckedKeys(checkedKeys);
	});
}


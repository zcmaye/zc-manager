#include "AssignUserRoleAddDlg.h"

#include "domain/entity/User.hpp"
#include "domain/entity/Role.hpp"
#include "domain/entity/UserRole.hpp"

#include "common/utils/OverlayWidget.h"
#include "common/delegate/ButtonDelegate.h"
#include "common/delegate/MappingDelegate.h"
#include "common/delegate/CheckBoxDelegate.h"
#include "common/widgets/MessageBox.h"
#include "common/widgets/CheckBoxHeaderView.h"
#include "common/widgets/FlowLayout.h"
#include "common/notify/NotifyTipManager.h"
#include "common/network/HttpMgr.h"

#include <QTimer>
#include <QRandomGenerator>

AssignUserRoleAddDlg::AssignUserRoleAddDlg(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AssignUserRoleAddDlgClass())
	, m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	//updateSearchBarUi();
	Utils::SetSearchBarFlowLayout(ui->searchBar);

	ui->userTableView->setModel(m_model);
	ui->userTableView->setEditTriggers(QTableView::NoEditTriggers);
	ui->userTableView->setSelectionMode(QTableView::NoSelection);
	ui->userTableView->verticalHeader()->hide();
	ui->userTableView->setShowGrid(false);
	ui->userTableView->verticalHeader()->setDefaultSectionSize(50);	//设置行高


	//可选择表头
	auto headerView = new CheckBoxHeaderView;
	ui->userTableView->setHorizontalHeader(headerView);

	//映射委托
	auto mappingDelegate = new MappingDelegate(this);
	mappingDelegate->addMapping({ 0,"禁用",QColor(255, 237, 237),QColor(255, 219, 219),QColor(255, 73, 73) });
	mappingDelegate->addMapping({ 1,"正常",QColor(232, 244, 255),QColor(209, 233, 255),QColor(24, 144, 255) });
	//选择委托
	auto checkBoxDelegate = new CheckBoxDelegate(this);

	ui->userTableView->setItemDelegateForColumn(0, checkBoxDelegate);
	ui->userTableView->setItemDelegateForColumn(5, mappingDelegate);

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
		});
	connect(ui->paginBar, &PaginationToolBar::pageChanged, this, &AssignUserRoleAddDlg::on_searchBtn_clicked);
	connect(ui->paginBar, &PaginationToolBar::pageSizeChanged, this, &AssignUserRoleAddDlg::on_searchBtn_clicked);
}

AssignUserRoleAddDlg::~AssignUserRoleAddDlg()
{
	delete ui;
}

void AssignUserRoleAddDlg::setRole(const std::shared_ptr<Role>& role)
{
	if (!role) {
		qWarning() << "role is nullptr";
	}
	m_role = role;
	QTimer::singleShot(0, [this] {
		on_searchBtn_clicked();
		});

}

void AssignUserRoleAddDlg::on_resetBtn_clicked()
{
	ui->userNameEdit->clear();
	ui->phoneEdit->clear();
	on_searchBtn_clicked();
}

static QList<QStandardItem*> GetUserItems(const std::shared_ptr<User>& user)
{
	QList<QStandardItem*> items;

	auto checkStateItem = items.emplaceBack(new QStandardItem);
	auto userNameItem = items.emplaceBack(new QStandardItem(user->user_name));
	auto nickNameItem = items.emplaceBack(new QStandardItem(user->nick_name));
	auto phoneItem = items.emplaceBack(new QStandardItem(user->phone_number));
	auto emailItem = items.emplaceBack(new QStandardItem(user->email));
	auto statusItem = items.emplaceBack(new QStandardItem(QString::number(user->status)));
	auto timeItem = items.emplaceBack(new QStandardItem(user->create_time));
	checkStateItem->setData(QVariant::fromValue(user), Qt::UserRole);
	statusItem->setData(QVariant::fromValue(user), Qt::UserRole);

	for (auto item : items) {
		item->setTextAlignment(Qt::AlignCenter);
		item->setSizeHint(QSize(0,55));
	}
	return items;
}

void AssignUserRoleAddDlg::on_searchBtn_clicked()
{
	if (!m_role)
		return;

	//清空数据
	m_model->clear();

	//取消表头的选择
	auto view = dynamic_cast<CheckBoxHeaderView*>(ui->userTableView->horizontalHeader());
	view->setCheckState(Qt::Unchecked);

	//设置表头
	m_model->setHorizontalHeaderLabels({
		"","用户名称","用户昵称","手机号码","邮箱","状态","创建时间","操作"
		});
	for (int i = 0; i < m_model->columnCount(); i++) {
		auto item = m_model->horizontalHeaderItem(i);
		if (item) {
			item->setTextAlignment(Qt::AlignCenter);
		}
	}
	//设置调整模式
	
	ui->userTableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeMode::Stretch);
	ui->userTableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeMode::Stretch);
	ui->userTableView->horizontalHeader()->setSectionResizeMode(4,QHeaderView::ResizeMode::Stretch);
	ui->userTableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeMode::Stretch);
	//ui->userTableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeMode::Stretch);
	//ui->userTableView->horizontalHeader()->setSectionResizeMode(6,QHeaderView::ResizeMode::Fixed);

	HttpMgr::instance()->get(URL("/role/authUser/unallocatedList"), ReqId::role_auth_user_unallocated_list, Modules::RoleAuthUserMgr, searchParams());
}

void AssignUserRoleAddDlg::setUnallocatedList(const std::shared_ptr<PagingDto<User>>& pagingDto)
{
	//从数据库中查询
	if (pagingDto->totalCount == 0) {
		NotifyTipManager::instance()->addNotifyTip("没有用户数据~", NotifyTipBox::TypeSuccess);
	}
	else {
		for (auto& user : pagingDto->items) {
			auto items = GetUserItems(user);
			m_model->appendRow(items);
		}
	}
	ui->paginBar->setTotalRecords(pagingDto->totalCount);
	
	ui->userTableView->setColumnWidth(0, 50);
	//ui->userTableView->setColumnWidth(1, 30);
	//ui->userTableView->setColumnWidth(2, 50);
	//ui->userTableView->setColumnWidth(3, 120);
	//ui->userTableView->setColumnWidth(4, 155);
	ui->userTableView->setColumnWidth(5, 100);
	ui->userTableView->setColumnWidth(6, 180);
	ui->userTableView->setColumnWidth(7, 180);
}



void AssignUserRoleAddDlg::on_okBtn_clicked()
{
	auto userIds = getCheckUserIds();
	if (userIds.isEmpty()) {
		qDebug() << "no user checked";
		return;
	}
	QJsonArray array;
	for (auto id : userIds) {
		array.push_back(id);
	}

	QJsonObject jobj;
	jobj["role_id"] = m_role->role_id;
	jobj["user_ids"] = array;

	HttpMgr::instance()->put(URL("/role/authUser/selectAll"), jobj, ReqId::role_auth_user_select_all, Modules::RoleAuthUserMgr);
}


static void recursionUserId(QStandardItem* item, QList<int>& userIds)
{
	if (!item)
		return;
	for (int i = 0; i < item->rowCount(); i++) {
		auto child = item->child(i, 0);
		if (!child)
			continue;
		if (child->data(CheckBoxDelegate::CheckStateRole).value<Qt::CheckState>() != Qt::Unchecked) {
			recursionUserId(child, userIds);
			auto user= child->data(Qt::UserRole).value<std::shared_ptr<User>>();
			if (user)
				userIds.append(user->user_id);
		}
	}

}

QList<int> AssignUserRoleAddDlg::getCheckUserIds() const
{
	QList<int> userIds;
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto child = m_model->item(i, 0);
		if (!child)
			continue;
		if (child->data(CheckBoxDelegate::CheckStateRole).value<Qt::CheckState>() != Qt::Unchecked) {
			recursionUserId(child, userIds);
			auto user= child->data(Qt::UserRole).value<std::shared_ptr<User>>();
			if (user)
				userIds.append(user->user_id);
		}
	}
	return userIds;
}

QVariantMap AssignUserRoleAddDlg::searchParams() const
{
	QVariantMap params;

	//获取查询条件
	auto userName = ui->userNameEdit->text();
	auto phone = ui->phoneEdit->text();

	params["role_id"] = m_role->role_id;

	if (!userName.isEmpty()) {
		params["user_name"] = userName;
	}

	if (!phone.isEmpty()) {
		params["phone_number"] = phone;
	}

	return params;
}


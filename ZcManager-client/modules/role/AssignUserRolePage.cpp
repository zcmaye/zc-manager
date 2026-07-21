#include "AssignUserRolePage.h"
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
#include <QTimer>
#include <QRandomGenerator>

AssignUserRolePage::AssignUserRolePage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AssignUserRolePageClass())
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
	//按钮委托
	auto buttonDelegate = new ButtonDelegate(ui->userTableView);
	buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/del.svg"), "取消授权");
	//选择委托
	auto checkBoxDelegate = new CheckBoxDelegate(this);

	ui->userTableView->setItemDelegateForColumn(0, checkBoxDelegate);
	ui->userTableView->setItemDelegateForColumn(5, mappingDelegate);
	ui->userTableView->setItemDelegateForColumn(7, buttonDelegate);

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
		ui->cancelAuthBtn->setEnabled(m_checkedCount > 0);
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
		ui->cancelAuthBtn->setEnabled(m_checkedCount > 0);
		});
	connect(buttonDelegate, &ButtonDelegate::clicked, this, [this](int id,const QModelIndex& index) {
		auto user = index.data(Qt::UserRole).value<std::shared_ptr<User>>();
		if (!user) {
			return;
		}
		if (id == 0) {
			qDebug() << "取消授权";
			auto box = delMsgBox();
			box->setMessage(QString("确认要取消该用户\"%1\"角色吗？").arg(user->user_name));
			box->setProperty("index", index);
			OverlayWidget::instance()->popup(box);
		}
		});
	connect(ui->paginBar, &PaginationToolBar::pageChanged, this, &AssignUserRolePage::on_searchBtn_clicked);
	connect(ui->paginBar, &PaginationToolBar::pageSizeChanged, this, &AssignUserRolePage::on_searchBtn_clicked);

	HTTP_HANDLER_INIT(auth_role_user_mgr);
}

AssignUserRolePage::~AssignUserRolePage()
{
	if (m_assignUserRoleAddDlg)
		m_assignUserRoleAddDlg->deleteLater();
	delete ui;
}

void AssignUserRolePage::setRole(const std::shared_ptr<Role>& role)
{
	if (!role) {
		qWarning() << "role is nullptr";
	}
	m_role = role;
	QTimer::singleShot(0, [this] {
		on_searchBtn_clicked();
		});

}

void AssignUserRolePage::on_resetBtn_clicked()
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
	auto optionItem = items.emplaceBack(new QStandardItem);
	checkStateItem->setData(QVariant::fromValue(user), Qt::UserRole);
	optionItem->setData(QVariant::fromValue(user), Qt::UserRole);
	statusItem->setData(QVariant::fromValue(user), Qt::UserRole);

	for (auto item : items) {
		item->setTextAlignment(Qt::AlignCenter);
		item->setSizeHint(QSize(0,55));
	}

	//如果是管理员
	if (user->user_id == 1 || user->user_name == "admin") {
		optionItem->setData(true, ButtonDelegate::HideRowRole);
	}
	//if (user->user_id == 9) {
	//	optionItem->setData(QVariant::fromValue(QList<int>{2}), ButtonDelegate::HideButtonRole);
	//}

	return items;
}

void AssignUserRolePage::on_searchBtn_clicked()
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

	HttpMgr::instance()->get(URL("/role/authUser/allocatedList"), ReqId::role_auth_user_allocated_list, Modules::RoleAuthUserMgr,searchParams());
}

void AssignUserRolePage::on_addUserBtn_clicked()
{
	qDebug() << __FUNCTION__;

	if (!m_assignUserRoleAddDlg) {
		m_assignUserRoleAddDlg = new AssignUserRoleAddDlg;
	}
	m_assignUserRoleAddDlg->setRole(m_role);
	OverlayWidget::instance()->popup(m_assignUserRoleAddDlg.get());
}


void AssignUserRolePage::on_cancelAuthBtn_clicked()
{
	QList<int> userIds;
	//获取所有选中的行
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i, 0);
		if (!item)
			continue;
		if (item->data(CheckBoxDelegate::CheckStateRole).toBool()) {
			auto user = item->data(Qt::UserRole).value<std::shared_ptr<User>>();
			if (!user) {
				qWarning() << "user is nullptr";
				continue;
			}
			userIds.append(user->user_id);
		}
	}

	QStringList userIdList;
	std::transform(userIds.begin(), userIds.end(), std::back_inserter(userIdList), [](int id) {
			return QString::number(id);
		});

	auto box = delMsgBox();
	box->setMessage(QString("是否确认取消授权用户编号为\"%1\"的数据项？").arg(userIdList.join(",")));
	box->setProperty("userIds", QVariant::fromValue(userIds));
	OverlayWidget::instance()->popup(box);
}

void AssignUserRolePage::on_shutBtn_clicked() {
	this->deleteLater();
}

void AssignUserRolePage::on_hideSearchBarBtn_clicked()
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

void AssignUserRolePage::on_refreshBtn_clicked()
{
	on_searchBtn_clicked();
}


QPointer<MessageBox> AssignUserRolePage::delMsgBox()
{
	if (!m_delMsgBox) {
		m_delMsgBox = new MessageBox(MessageBox::Warning);
		connect(m_delMsgBox, &MessageBox::closed, this, [this](int ret) {
			if (ret == MessageBox::AcceptRole) {
				//批量取消授权
				auto userIds = m_delMsgBox->property("userIds").value<QList<int>>();
				if (!userIds.empty()) {
					QJsonArray arr;
					for (auto id : userIds) {
						arr.push_back(id);
					}

					QJsonObject jobj;
					jobj["role_id"] = m_role->role_id;
					jobj["user_ids"] = arr;

					HttpMgr::instance()->put(URL("/role/authUser/cancelAll"), jobj, ReqId::role_auth_user_cancel_all, Modules::RoleAuthUserMgr);
				}
				else {
					auto index = m_delMsgBox->property("index").value<QModelIndex>();
					auto user = index.data(Qt::UserRole).value<std::shared_ptr<User>>();
					try {
						//创建用户角色
						auto userRole = std::make_shared<UserRole>();
						userRole->role_id = m_role->role_id;
						userRole->user_id = user->user_id;
						//删除
						HttpMgr::instance()->put(URL("/role/authUser/cancel"), userRole, ReqId::role_auth_user_cancel, Modules::RoleAuthUserMgr);
					}
					catch (const std::exception& e) {
						NotifyTipManager::instance()->addNotifyTip(e.what());
					}
				}
			}
			m_delMsgBox->close();
			});
	}
	return m_delMsgBox;
}

QVariantMap AssignUserRolePage::searchParams()
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


HTTP_HANDLER_IMPL(AssignUserRolePage, auth_role_user_mgr)
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

void AssignUserRolePage::initHandlers()
{
	HTTP_HANDER_INSERT(ReqId::role_auth_user_allocated_list)
	{
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
			return;
		}

		auto pagingDto = json_t(json["data"]).cast<std::shared_ptr<PagingDto<User>>>();
		if (pagingDto->totalCount == 0) {
			NotifyTipManager::instance()->addNotifyTip("没有已分配的用户数据~", NotifyTipBox::TypeSuccess);
		}
		else {
			for (auto& user : pagingDto->items) {
				auto items = GetUserItems(user);
				m_model->appendRow(items);
			}
		}
		ui->paginBar->setTotalRecords(pagingDto->totalCount);

		ui->userTableView->setColumnWidth(0, 50);
		ui->userTableView->setColumnWidth(1, 100);
		//ui->userTableView->setColumnWidth(1, 30);
		//ui->userTableView->setColumnWidth(2, 50);
		//ui->userTableView->setColumnWidth(3, 120);
		//ui->userTableView->setColumnWidth(4, 155);
		ui->userTableView->setColumnWidth(6, 100);
		ui->userTableView->setColumnWidth(7, 180);
		ui->userTableView->setColumnWidth(8, 180);
	});

	HTTP_HANDER_INSERT(ReqId::role_auth_user_unallocated_list)
	{
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
			return;
		}

		auto pagingDto = json_t(json["data"]).cast<std::shared_ptr<PagingDto<User>>>();
		if (pagingDto->totalCount == 0) {
			return;
		}
		m_assignUserRoleAddDlg->setUnallocatedList(pagingDto);
	});


	HTTP_HANDER_INSERT(ReqId::role_auth_user_cancel_all) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}
		//取消设置
		m_delMsgBox->setProperty("userIds", QVariant());
		//把选中数量置空
		m_checkedCount = 0;
		auto view = dynamic_cast<CheckBoxHeaderView*>(ui->userTableView->horizontalHeader());
		view->setCheckState(Qt::Unchecked);
		//重新搜索
		on_searchBtn_clicked();
		NotifyTipManager::instance()->addNotifyTip("删除成功~", NotifyTipBox::TypeSuccess);
	});

	HTTP_HANDER_INSERT(ReqId::role_auth_user_cancel) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		NotifyTipManager::instance()->addNotifyTip("删除成功~", NotifyTipBox::TypeSuccess);
		m_model->removeRow(m_delMsgBox->property("index").toModelIndex().row());
	});

	HTTP_HANDER_INSERT(ReqId::role_auth_user_select_all) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		NotifyTipManager::instance()->addNotifyTip("添加用户授权成~", NotifyTipBox::TypeSuccess);
		m_assignUserRoleAddDlg->hide();
	});

}



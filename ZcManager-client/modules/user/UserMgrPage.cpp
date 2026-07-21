#include "UserMgrPage.h"
#include "UserAddDlg.h"
#include "UserEditDlg.h"

#include "common/utils/OverlayWidget.h"
#include "common/delegate/ButtonDelegate.h"
#include "common/delegate/SwitchDelegate.h"
#include "common/delegate/CheckBoxDelegate.h"
#include "common/widgets/MessageBox.h"
#include "common/widgets/CheckBoxHeaderView.h"
#include "common/widgets/FlowLayout.h"
#include "common/widgets/XlsxUploadDlg.h"
#include "common/notify/NotifyTipManager.h"
#include "common/utils/ExcelUtil.h"

#include "domain/entity/User.hpp"
#include "domain/entity/Post.hpp"
#include "domain/entity/Role.hpp"
#include "domain/vo/TreeSelect.hpp"

#include "ContextHolder.h"
#include <QTimer>
#include <QRandomGenerator>
#include <QFileDialog>

class ItemDelegate : public QStyledItemDelegate
{
public:
protected:
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		return QSize(option.rect.width(),55);
	}
};

UserMgrPage::UserMgrPage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::UserMgrPageClass())
	, m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	//updateSearchBarUi();
	Utils::SetSearchBarFlowLayout(ui->searchBar);

	ui->statusCbx->setCurrentIndex(-1);

	ui->addBtn->setPermission("system:user:add");
	ui->alterBtn->setPermission("system:user:edit");
	ui->delBtn->setPermission("system:user:remove");
	ui->importBtn->setPermission("system:user:import");
	ui->exportBtn->setPermission("system:user:export");

	//UserService::instance()->insertTestData(100);

	ui->userTableView->setModel(m_model);
	ui->userTableView->setEditTriggers(QTableView::NoEditTriggers);
	ui->userTableView->setSelectionMode(QTableView::NoSelection);
	ui->userTableView->setShowGrid(false);
	ui->userTableView->verticalHeader()->hide();
	ui->userTableView->verticalHeader()->setDefaultSectionSize(50);	//设置行高

	auto ctx = ContextHolder::instance();

	//可选择表头
	auto headerView = new CheckBoxHeaderView;
	ui->userTableView->setHorizontalHeader(headerView);

	//开关委托
	auto switchDelegate = new SwitchDelegate(this);
	m_switchDelegate = switchDelegate;
	switchDelegate->setBindValue(1, 0);
	//按钮委托
	m_buttonDelegate = new ButtonDelegate(ui->userTableView);
	auto buttonDelegate = m_buttonDelegate;
	if (ctx->hasPermission("system:user:edit")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/pen.svg"), "修改");
	}
	if (ctx->hasPermission("system:user:add")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/add.svg"), "新增");
	}
	if (ctx->hasPermission("system:user:remove")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/del.svg"), "删除");
	}
	//选择委托
	auto checkBoxDelegate = new CheckBoxDelegate(this);

	ui->userTableView->setItemDelegateForColumn(0, checkBoxDelegate);
	ui->userTableView->setItemDelegateForColumn(6, switchDelegate);
	ui->userTableView->setItemDelegateForColumn(8, buttonDelegate);

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
			auto user = index.data(Qt::UserRole).value<std::shared_ptr<User>>();
			if (!user) {
				qWarning() << "User Not found";
				return;
			}
			user->status = status;
			HttpMgr::instance()->put(URL("/user/updateStatus"), { {"user_id",user->user_id}, {"status",status} }, ReqId::user_update_status, Modules::UserMgr);
			switchDelegate->setProperty("index", index);
		});
	connect(buttonDelegate, &ButtonDelegate::clicked, this, [this](int id,const QModelIndex& index,ButtonDelegate::Data* d) {
		auto user = index.data(Qt::UserRole).value<std::shared_ptr<User>>();
		if (!user) {
			return;
		}
		if (d->text == "修改") {
			qDebug() << "修改";
			//slot_update_user(user);
			on_alterBtn_clicked();
		}
		else if (d->text == "新增") {
			on_addBtn_clicked();
			//m_userAddDlg->setParentUser(user);
		}
		else if (d->text == "删除") {
			qDebug() << "删除";
			auto box = delMsgBox();
			box->setMessage(QString("是否确认删除名称为\"%1\"的数据项?").arg(user->user_name));
			box->setProperty("index", index);
			OverlayWidget::instance()->popup(box);
		}
		});
	connect(ui->paginBar, &PaginationToolBar::pageChanged, this, &UserMgrPage::on_searchBtn_clicked);
	connect(ui->paginBar, &PaginationToolBar::pageSizeChanged, this, &UserMgrPage::on_searchBtn_clicked);

	ui->statusCbx->clear();
	ui->statusCbx->addItem("正常", 1);
	ui->statusCbx->addItem("禁用", 0);

	QTimer::singleShot(0, [this] {
		on_searchBtn_clicked();
		});


	HTTP_HANDLER_INIT(user_mgr);

	if (ContextHolder::instance()->deptTreeSelect().empty()) {
		HttpMgr::instance()->get(URL("/dept/options"), ReqId::dept_treeselect, Modules::UserMgr);
	}
}

UserMgrPage::~UserMgrPage()
{
	if(m_delMsgBox)
		m_delMsgBox->deleteLater();
	if(m_exportMsgBox)
		m_exportMsgBox->deleteLater();;
	if(m_userAddDlg)
		m_userAddDlg->deleteLater();;
	if(m_userEditDlg)
		m_userEditDlg->deleteLater();;
	if(m_xlsxUploadDlg)
		m_xlsxUploadDlg->deleteLater();;
	delete ui;
}

void UserMgrPage::on_resetBtn_clicked()
{
	ui->userNameEdit->clear();
	ui->phoneEdit->clear();
	ui->statusCbx->setCurrentIndex(-1);
	ui->dateRangePicker->clear();
	on_searchBtn_clicked();
}

static QList<QStandardItem*> GetUserItems(const std::shared_ptr<User>& user)
{
	QList<QStandardItem*> items;

	auto checkStateItem = items.emplaceBack(new QStandardItem);
	auto idItem = items.emplaceBack(new QStandardItem(QString::number(user->user_id)));
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
		//item->setSizeHint(QSize(0,55));
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

void UserMgrPage::on_searchBtn_clicked()
{
	//清空数据
	m_model->clear();

	//取消表头的选择
	auto view = dynamic_cast<CheckBoxHeaderView*>(ui->userTableView->horizontalHeader());
	view->setCheckState(Qt::Unchecked);

	//设置表头
	QStringList headers = { "","用户编号","用户名称","用户昵称","手机号码","邮箱","状态","创建时间","操作" };
	m_model->setHorizontalHeaderLabels(headers);
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

	HttpMgr::instance()->get(URL("/user/list"), ReqId::user_list, Modules::UserMgr, searchParams());
	
}

void UserMgrPage::on_addBtn_clicked()
{
	if (!m_userAddDlg) {
		m_userAddDlg = new UserAddDlg;
	}
	m_userAddDlg->clear();
	m_userAddDlg->setDeptTreeSelect(ContextHolder::instance()->deptTreeSelect());
	HttpMgr::instance()->get(URL("/user/"), ReqId::user_get_by_id, Modules::UserMgr);
	OverlayWidget::instance()->popup(m_userAddDlg.get());
}

void UserMgrPage::on_alterBtn_clicked()
{
	if (m_checkedCount > 1) {
		NotifyTipManager::instance()->addNotifyTip("选择了多个，只能修改一个用户数据~");
		return;
	}

	if (!m_userEditDlg) {
		m_userEditDlg = new UserEditDlg;
	}
	m_userEditDlg->clear();

	auto user = currentIndexUser();
	HttpMgr::instance()->get(URL("/user/" + QString::number(user->user_id)), ReqId::user_get_by_id, Modules::UserMgr);

	m_userEditDlg->setDeptTreeSelect(ContextHolder::instance()->deptTreeSelect());
	m_userEditDlg->setProperty("user", QVariant::fromValue(user));
	OverlayWidget::instance()->popup(m_userEditDlg.get());
}

void UserMgrPage::on_delBtn_clicked()
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
	box->setMessage(QString("是否确认删除用户编号为\"%1\"的数据项？").arg(userIdList.join(",")));
	box->setProperty("userIds", QVariant::fromValue(userIdList));
	OverlayWidget::instance()->popup(box);
}

void UserMgrPage::on_importBtn_clicked()
{
	/*
	if (!m_xlsxUploadDlg) {
		m_xlsxUploadDlg = new XlsxUploadDlg;
		//下载用户导入模板
		connect(m_xlsxUploadDlg, &XlsxUploadDlg::sig_download_tmeplate, this, [this] {
			auto filename = QFileDialog::getSaveFileName(this, "保存", "./用户数据.xlsx", "Excel (*.xlsx);;All (*.*)");
			if (filename.isEmpty()) {
				return;
			}
			QFile file(filename);
			if (!file.open(QIODevice::WriteOnly)) {
				qWarning() << file.fileName() << " open failed!";
				return;
			}

			ExcelUtil util(EntityType::User);
			if (!util.importTemplateExcel(&file, "用户数据")) {
				qWarning() << " import template failed!";
			}
			});

		//用户导入
		connect(m_xlsxUploadDlg, &XlsxUploadDlg::sig_upload, [this](const QStringList& filenames) {
			if (filenames.isEmpty()) {
				NotifyTipManager::instance()->addNotifyTip("请先选择一个xlsx文件");
				return;
			}
			auto& file =  filenames.first();
			QFile fp(file);
			if (!fp.open(QIODevice::ReadOnly)) {
				qWarning() << fp.fileName() << " open failed!";
				return ;
			}

			ExcelUtil util(EntityType::User);
			auto userList =  util.importExcel<User>({}, &fp, 1);
			auto self = ContextHolder::instance()->self();
			try {
				auto msg = UserService::instance()->importUser(userList, m_xlsxUploadDlg->isUpdateSupport(), self->user_name);
				NotifyTipManager::instance()->addNotifyTip(msg,NotifyTipBox::TypeSuccess);
				qDebug() << msg;
			}
			catch (const std::exception& e) {
				NotifyTipManager::instance()->addNotifyTip(e.what());
				qWarning() << e.what();
			}
			});
	}
	OverlayWidget::instance()->popup(m_xlsxUploadDlg.get());
	*/
}

void UserMgrPage::on_exportBtn_clicked()
{
	auto box = exportMsgBox();
	box->setMessage("确定导出所有用户信息吗?");
	OverlayWidget::instance()->popup(box);
}

void UserMgrPage::on_hideSearchBarBtn_clicked()
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

void UserMgrPage::on_refreshBtn_clicked()
{
	on_searchBtn_clicked();
}

void UserMgrPage::slot_update_user(const std::shared_ptr<User>& user)
{
	//if (!m_userEditDlg) {
	//	m_userEditDlg = new UserEditDlg;
	//}
	//m_userEditDlg->clear();
	//m_userEditDlg->setUsers(UserService::instance()->getUserTree());
	//m_userEditDlg->setUser(user);
	//OverlayWidget::instance()->popup(m_userEditDlg.get());
}

void UserMgrPage::slot_update_alter_del()
{
	ui->alterBtn->setEnabled(m_checkedCount == 1);
	ui->delBtn->setEnabled(m_checkedCount > 0);
}

QPointer<MessageBox> UserMgrPage::delMsgBox()
{
	if (!m_delMsgBox) {
		m_delMsgBox = new MessageBox(MessageBox::Warning);
		connect(m_delMsgBox, &MessageBox::closed, this, [this](int ret) {
			if (ret == MessageBox::AcceptRole) {
				//批量删除
				auto userIds = m_delMsgBox->property("userIds").toStringList();
				if (!userIds.isEmpty()) {
					HttpMgr::instance()->del(URL("/user/" + userIds.join(",")), ReqId::user_del, Modules::UserMgr);
					//取消设置
					m_delMsgBox->setProperty("userIds", QVariant());
					//把选中数量置空
					m_checkedCount = 0;
					auto view = dynamic_cast<CheckBoxHeaderView*>(ui->userTableView->horizontalHeader());
					view->setCheckState(Qt::Unchecked);
					//重新搜索
					on_searchBtn_clicked();
				}
				else {
					auto index = m_delMsgBox->property("index").toModelIndex();
					auto user = index.data(Qt::UserRole).value<std::shared_ptr<User>>();
					HttpMgr::instance()->del(URL("/user/" + QString::number(user->user_id)), ReqId::user_del, Modules::UserMgr);
				}
			}
			m_delMsgBox->close();
			});
	}
	return m_delMsgBox;
}

QPointer<MessageBox> UserMgrPage::exportMsgBox()
{
	/*
	if (!m_exportMsgBox) {
		m_exportMsgBox = new MessageBox(MessageBox::Warning);
		connect(m_exportMsgBox, &MessageBox::closed, this, [this](int ret) {
			if (ret == MessageBox::AcceptRole) {
				auto userList = UserService::instance()->selectUserListBy(searchUser());
				if (userList.isEmpty()) {
					NotifyTipManager::instance()->addNotifyTip("暂无用户数据~", NotifyTipBox::TypeSuccess);
					return;
				}

				auto filename = QFileDialog::getSaveFileName(this, "保存", "./导出用户数据.xlsx", "Excel (*.xlsx);;All (*.*)");
				if (filename.isEmpty()) {
					return;
				}
				QFile file(filename);
				if (!file.open(QIODevice::WriteOnly)) {
					qWarning() << file.fileName() << " open failed!";
					return;
				}

				ExcelUtil util(EntityType::User);
				util.exportExcel<User>(&file, userList, "用户数据");
				NotifyTipManager::instance()->addNotifyTip("导出成功~", NotifyTipBox::TypeSuccess);
			}
			m_exportMsgBox->close();
			});
	}
	*/
	return m_exportMsgBox;
}

std::shared_ptr<User> UserMgrPage::currentIndexUser() const
{
	auto index = ui->userTableView->currentIndex();
	if (!index.isValid()) {
		qWarning() << "index is invalid";
		return {};
	}
	auto item = m_model->item(index.row(), 0);
	if (!item) {
		qWarning() << "item is nullptr";
		return {};
	}
	return item->data(Qt::UserRole).value<std::shared_ptr<User>>();
}

std::shared_ptr<User> UserMgrPage::searchUser() const
{
	//获取查询条件
	auto userName = ui->userNameEdit->text();
	auto phone = ui->phoneEdit->text();
	auto startDate = ui->dateRangePicker->startDate();
	auto endDate = ui->dateRangePicker->endDate();
	auto searchUser = std::make_shared<User>();

	if (!userName.isEmpty()) {
		searchUser->user_name = userName;
	}

	if (!phone.isEmpty()) {
		searchUser->phone_number = phone;
	}

	if (startDate.isValid() && endDate.isValid()) {
		searchUser->create_time = startDate.toString("yyyy-MM-dd") + "," + endDate.toString("yyyy-MM-dd");
	}

	if (ui->statusCbx->currentIndex() != -1) {
		searchUser->status = ui->statusCbx->currentData().toInt();
	}

	return searchUser;
}

QVariantMap UserMgrPage::searchParams() const
{
	//获取查询条件
	auto userName = ui->userNameEdit->text();
	auto phone = ui->phoneEdit->text();
	auto startDate = ui->dateRangePicker->startDate();
	auto endDate = ui->dateRangePicker->endDate();

	//查询参数
	QVariantMap params = {
		{"page",ui->paginBar->page()},
		{"pageSize",ui->paginBar->pageSize()}
	};

	if (!userName.isEmpty()) {
		params["user_name"] = userName;
	}

	if (!phone.isEmpty()) {
		params["phone_number"] = phone;
	}

	if (startDate.isValid() && endDate.isValid()) {
		//params["create_time"] = startDate.toString("yyyy-MM-dd") + "," + endDate.toString("yyyy-MM-dd");
		params["begin_time"] = startDate.toString("yyyy-MM-dd");
		params["end_time"] = endDate.toString("yyyy-MM-dd");
	}

	if (ui->statusCbx->currentIndex() != -1) {
		params["status"] = ui->statusCbx->currentData().toInt();
	}
	return params;
}


HTTP_HANDLER_IMPL(UserMgrPage, user_mgr)
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


void UserMgrPage::initHandlers()
{

	HTTP_HANDER_INSERT(ReqId::user_list)
	{
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
			return;
		}

		auto pagingDto = json_t(json["data"]).cast<std::shared_ptr<PagingDto<User>>>();
		//auto pagingDto = UserService::instance()->selectUserList(filter, ui->paginBar->page(), ui->paginBar->pageSize());
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
		ui->userTableView->setColumnWidth(1, 100);
		//ui->userTableView->setColumnWidth(1, 30);
		//ui->userTableView->setColumnWidth(2, 50);
		//ui->userTableView->setColumnWidth(3, 120);
		//ui->userTableView->setColumnWidth(4, 155);
		ui->userTableView->setColumnWidth(6, 100);
		ui->userTableView->setColumnWidth(7, 180);
		ui->userTableView->setColumnWidth(8, 180);
		if (m_buttonDelegate->buttonCount() <= 0) {
			ui->userTableView->setColumnHidden(m_model->rowCount() - 1, true);
		}
	});

	HTTP_HANDER_INSERT(ReqId::user_update_status) {
		if (json["code"] != 200) {
			m_switchDelegate->rollback(m_switchDelegate->property("index").toModelIndex());
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		NotifyTipManager::instance()->addNotifyTip("用户状态更新成功~", NotifyTipBox::TypeSuccess);
	});

	HTTP_HANDER_INSERT(ReqId::user_del) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		NotifyTipManager::instance()->addNotifyTip("删除成功~", NotifyTipBox::TypeSuccess);
		m_model->removeRow(m_delMsgBox->property("index").toModelIndex().row());
	});

	HTTP_HANDER_INSERT(ReqId::dept_treeselect) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		auto deptTreeSelect = json_t(json["data"]).cast<QList<std::shared_ptr<TreeSelect>>>();
		if (deptTreeSelect.isEmpty()) {
			qWarning()<<"deptTreeSelect is empty";
			return;
		}

		ContextHolder::instance()->setDeptTreeSelect(deptTreeSelect);
	});

	HTTP_HANDER_INSERT(ReqId::user_get_by_id) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		auto roles = json_t(json["roles"]).cast<QList<std::shared_ptr<Role>>>();
		auto posts = json_t(json["posts"]).cast<QList<std::shared_ptr<Post>>>();
		if (roles.isEmpty()) {
			qWarning()<<"roles is empty";
			return;
		}

		if (posts.isEmpty()) {
			qWarning()<<"posts is empty";
			return;	HTTP_HANDER_INSERT(ReqId::user_add) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		m_userAddDlg->clear();
		m_userAddDlg->hide();
	});

	HTTP_HANDER_INSERT(ReqId::user_edit) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		m_userEditDlg->clear();
		m_userEditDlg->hide();
	});

		}

		if (m_userAddDlg) {
			m_userAddDlg->setRoles(roles);
			m_userAddDlg->setPosts(posts);
			m_userAddDlg->setDeptTreeSelect(ContextHolder::instance()->deptTreeSelect());
		}
		if (m_userEditDlg) {
			m_userEditDlg->setRoles(roles);
			m_userEditDlg->setPosts(posts);
			m_userEditDlg->setDeptTreeSelect(ContextHolder::instance()->deptTreeSelect());
			auto user = m_userEditDlg->property("user").value<std::shared_ptr<User>>();
			m_userEditDlg->setUser(user);

			//设置选择的岗位和角色
			if (json.contains("postIds")) {
				m_userEditDlg->setSelectPosts(json_t(json["postIds"]).cast<QList<int32_t>>());
			}
			if (json.contains("roleIds")) {
				m_userEditDlg->setSelectRoles(json_t(json["roleIds"]).cast<QList<int32_t>>());
			}
		}
	});

	HTTP_HANDER_INSERT(ReqId::user_add) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		m_userAddDlg->clear();
		m_userAddDlg->hide();
	});

	HTTP_HANDER_INSERT(ReqId::user_edit) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		m_userEditDlg->clear();
		m_userEditDlg->hide();
	});
}

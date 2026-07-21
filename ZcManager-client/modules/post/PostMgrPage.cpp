#include "PostMgrPage.h"
#include "PostAddDlg.h"
#include "PostEditDlg.h"

#include "domain/entity/Post.hpp"
#include "domain/entity/User.hpp"
#include "common/base/Event.hpp"
#include "common/utils/OverlayWidget.h"
#include "common/delegate/ButtonDelegate.h"
#include "common/delegate/MappingDelegate.h"
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

PostMgrPage::PostMgrPage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PostMgrPageClass())
	, m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	//updateSearchBarUi();
	Utils::SetSearchBarFlowLayout(ui->searchBar);
	ui->statusCbx->setCurrentIndex(-1);

	//ui("system:post:edit")) {
	ui->addBtn->setPermission("system:post:add");
	ui->alterBtn->setPermission("system:post:edit");
	ui->delBtn->setPermission("system:post:remove");
	ui->exportBtn->setPermission("system:post:export");

	ui->postTableView->setModel(m_model);
	ui->postTableView->setEditTriggers(QTableView::NoEditTriggers);
	ui->postTableView->setSelectionMode(QTableView::NoSelection);
	ui->postTableView->setShowGrid(false);
	ui->postTableView->verticalHeader()->hide();
	ui->postTableView->verticalHeader()->setDefaultSectionSize(50);	//设置行高

	//可选择表头
	auto headerView = new CheckBoxHeaderView;
	ui->postTableView->setHorizontalHeader(headerView);

	auto ctx = ContextHolder::instance();

	//映射委托
	auto mappingDelegate = new MappingDelegate(this);
	mappingDelegate->addMapping({ 0,"禁用",QColor(255, 237, 237),QColor(255, 219, 219),QColor(255, 73, 73) });
	mappingDelegate->addMapping({ 1,"正常",QColor(232, 244, 255),QColor(209, 233, 255),QColor(24, 144, 255) });
	//按钮委托
	m_buttonDelegate = new ButtonDelegate(ui->postTableView);
	auto buttonDelegate = m_buttonDelegate;
	if (ctx->hasPermission("system:post:edit")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/pen.svg"), "修改");
	}
	if (ctx->hasPermission("system:post:remove")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/del.svg"), "删除");
	}
	//选择委托
	auto checkBoxDelegate = new CheckBoxDelegate(this);

	ui->postTableView->setItemDelegateForColumn(0, checkBoxDelegate);
	ui->postTableView->setItemDelegateForColumn(5, mappingDelegate);
	ui->postTableView->setItemDelegateForColumn(7, buttonDelegate);

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
	connect(buttonDelegate, &ButtonDelegate::clicked, this, [this](int id,const QModelIndex& index,ButtonDelegate::Data *d) {
		auto post = index.data(Qt::UserRole).value<std::shared_ptr<Post>>();
		if (!post) {
			return;
		}
		if (d->text == "修改") {
			qDebug() << "修改";
			on_alterBtn_clicked();
		}
		else if (d->text == "删除") {
			qDebug() << "删除";
			auto box = delMsgBox();
			box->setMessage(QString("是否确认删除名称为\"%1\"的数据项?").arg(post->post_name));
			box->setProperty("index", index);
			OverlayWidget::instance()->popup(box);
		}
		});
	connect(ui->paginBar, &PaginationToolBar::pageChanged, this, &PostMgrPage::on_searchBtn_clicked);
	connect(ui->paginBar, &PaginationToolBar::pageSizeChanged, this, &PostMgrPage::on_searchBtn_clicked);

	ui->statusCbx->clear();
	ui->statusCbx->addItem("正常", 1);
	ui->statusCbx->addItem("禁用", 0);

	QTimer::singleShot(0, [this] {
		on_searchBtn_clicked();
		});


	HTTP_HANDLER_INIT(post_mgr);
}

PostMgrPage::~PostMgrPage()
{
	if (m_postAddDlg)
		m_postAddDlg->deleteLater();
	if (m_postEditDlg)
		m_postEditDlg->deleteLater();
	if (m_delMsgBox)
		m_delMsgBox->deleteLater();
	delete ui;
}

void PostMgrPage::on_resetBtn_clicked()
{
	ui->postNameEdit->clear();
	ui->postCodeEdit->clear();
	ui->statusCbx->setCurrentIndex(-1);
	on_searchBtn_clicked();
}

static QList<QStandardItem*> GetPostItems(const std::shared_ptr<Post>& post)
{
	QList<QStandardItem*> items;

	auto checkStateItem = items.emplaceBack(new QStandardItem);
	auto idItem = items.emplaceBack(new QStandardItem(QString::number(post->post_id)));
	auto postCodeItem = items.emplaceBack(new QStandardItem(post->post_code));
	auto postNameItem = items.emplaceBack(new QStandardItem(post->post_name));
	auto postSortItem = items.emplaceBack(new QStandardItem(QString::number(post->post_sort)));
	auto statusItem = items.emplaceBack(new QStandardItem(QString::number(post->is_active)));
	auto timeItem = items.emplaceBack(new QStandardItem(post->create_time));
	auto optionItem = items.emplaceBack(new QStandardItem);
	checkStateItem->setData(QVariant::fromValue(post), Qt::UserRole);
	optionItem->setData(QVariant::fromValue(post), Qt::UserRole);
	statusItem->setData(QVariant::fromValue(post), Qt::UserRole);

	for (auto item : items) {
		item->setTextAlignment(Qt::AlignCenter);
		item->setSizeHint(QSize(0,55));
	}

	return items;
}

void PostMgrPage::on_searchBtn_clicked()
{
	//清空数据
	m_model->clear();

	//取消表头的选择
	auto view = dynamic_cast<CheckBoxHeaderView*>(ui->postTableView->horizontalHeader());
	view->setCheckState(Qt::Unchecked);

	//设置表头
	QStringList headers = { "","岗位编号","岗位编码","岗位名称","岗位排序","状态","创建时间","操作"};
	m_model->setHorizontalHeaderLabels(headers);
	for (int i = 0; i < m_model->columnCount(); i++) {
		auto item = m_model->horizontalHeaderItem(i);
		if (item) {
			item->setTextAlignment(Qt::AlignCenter);
		}
	}
	//设置调整模式
	
	ui->postTableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeMode::Stretch);
	ui->postTableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeMode::Stretch);
	ui->postTableView->horizontalHeader()->setSectionResizeMode(4,QHeaderView::ResizeMode::Stretch);
	ui->postTableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeMode::Stretch);
	//ui->postTableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeMode::Stretch);
	//ui->postTableView->horizontalHeader()->setSectionResizeMode(6,QHeaderView::ResizeMode::Fixed);


	HttpMgr::instance()->get(URL("/post/list"), ReqId::post_list, Modules::PostMgr, searchParams());
}

void PostMgrPage::on_addBtn_clicked()
{
	if (!m_postAddDlg) {
		m_postAddDlg = new PostAddDlg;
	}
	m_postAddDlg->clear();

	OverlayWidget::instance()->popup(m_postAddDlg.get());
}

void PostMgrPage::on_alterBtn_clicked()
{
	if (m_checkedCount > 1) {
		NotifyTipManager::instance()->addNotifyTip("选择了多个，只能修改一个岗位数据~");
		return;
	}

	if (!m_postEditDlg) {
		m_postEditDlg = new PostEditDlg;
	}
	m_postEditDlg->clear();
	m_postEditDlg->setPost(currentIndexPost());

	OverlayWidget::instance()->popup(m_postEditDlg.get());
}

void PostMgrPage::on_delBtn_clicked()
{
	QList<int> postIds;
	//获取所有选中的行
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i, 0);
		if (!item)
			continue;
		if (item->data(CheckBoxDelegate::CheckStateRole).toBool()) {
			auto post = item->data(Qt::UserRole).value<std::shared_ptr<Post>>();
			if (!post) {
				qWarning() << "post is nullptr";
				continue;
			}
			postIds.append(post->post_id);
		}
	}

	QStringList postIdList;
	std::transform(postIds.begin(), postIds.end(), std::back_inserter(postIdList), [](int id) {
			return QString::number(id);
		});

	auto box = delMsgBox();
	box->setMessage(QString("是否确认删除岗位编号为\"%1\"的数据项？").arg(postIdList.join(",")));
	box->setProperty("postIds", QVariant::fromValue(postIdList));
	OverlayWidget::instance()->popup(box);
}

void PostMgrPage::on_exportBtn_clicked()
{
}

void PostMgrPage::on_hideSearchBarBtn_clicked()
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

void PostMgrPage::on_refreshBtn_clicked()
{
	on_searchBtn_clicked();
}

void PostMgrPage::slot_update_alter_del()
{
	ui->alterBtn->setEnabled(m_checkedCount == 1);
	ui->delBtn->setEnabled(m_checkedCount > 0);
}

std::shared_ptr<Post> PostMgrPage::currentIndexPost() const
{
	auto index =  ui->postTableView->currentIndex();
	if (!index.isValid()) { 
		qWarning() << "index is invalid";
		return {};
	}
	auto item = m_model->item(index.row(), 0);
	if (!item) {
		qWarning() << "item is nullptr";
		return {};
	}
	return item->data(Qt::UserRole).value<std::shared_ptr<Post>>();
}

QPointer<MessageBox> PostMgrPage::delMsgBox()
{
	if (!m_delMsgBox) {
		m_delMsgBox = new MessageBox(MessageBox::Warning);
		connect(m_delMsgBox, &MessageBox::closed, this, [this](int ret) {
			if (ret == MessageBox::AcceptRole) {
				//批量删除
				auto postIds = m_delMsgBox->property("postIds").toStringList();
				if (!postIds.empty()) {
					HttpMgr::instance()->del(URL("/post/" + postIds.join(",")),ReqId::post_del,Modules::PostMgr);
					//取消设置
					m_delMsgBox->setProperty("postIds", QVariant());
					//把选中数量置空
					m_checkedCount = 0;
					auto view = dynamic_cast<CheckBoxHeaderView*>(ui->postTableView->horizontalHeader());
					view->setCheckState(Qt::Unchecked);
					//重新搜索
					on_searchBtn_clicked();
				}
				else {
					auto index = m_delMsgBox->property("index").value<QModelIndex>();
					auto post = index.data(Qt::UserRole).value<std::shared_ptr<Post>>();
					HttpMgr::instance()->del(URL("/post/" + QString::number(post->post_id)),ReqId::post_del,Modules::PostMgr);
				}
			}
			m_delMsgBox->close();
			});
	}
	return m_delMsgBox;
}

QVariantMap PostMgrPage::searchParams()
{
	QVariantMap params;
	//获取查询条件
	auto postName = ui->postNameEdit->text();
	auto postCode = ui->postCodeEdit->text();

	if (!postName.isEmpty()) {
		params["post_name"] = postName;
	}

	if (!postCode.isEmpty()) {
		params["post_code"] = postCode;
	}

	if (ui->statusCbx->currentIndex() !=-1) {
		params["is_active"] = ui->statusCbx->currentData().toInt();
	}

	return params;
}


HTTP_HANDLER_IMPL(PostMgrPage, post_mgr)
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

void PostMgrPage::initHandlers()
{
	HTTP_HANDER_INSERT(ReqId::post_list)
	{
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
			return;
		}

		auto pagingDto = json_t(json["data"]).cast<std::shared_ptr<PagingDto<Post>>>();
		if (pagingDto->totalCount == 0) {
			NotifyTipManager::instance()->addNotifyTip("没有用户数据~", NotifyTipBox::TypeSuccess);
			return;
		}

		for (auto& post : pagingDto->items) {
			auto items = GetPostItems(post);
			m_model->appendRow(items);
		}

		ui->paginBar->setTotalRecords(pagingDto->totalCount);

		ui->postTableView->setColumnWidth(0, 50);
		ui->postTableView->setColumnWidth(1, 100);
		//ui->postTableView->setColumnWidth(1, 30);
		//ui->postTableView->setColumnWidth(2, 50);
		//ui->postTableView->setColumnWidth(3, 120);
		//ui->postTableView->setColumnWidth(4, 155);
		ui->postTableView->setColumnWidth(6, 180);
		ui->postTableView->setColumnWidth(7, 180);
		if (m_buttonDelegate->buttonCount() <= 0) {
			ui->postTableView->setColumnHidden(m_model->columnCount() - 1, true);
		}
	});

	HTTP_HANDER_INSERT(ReqId::post_del) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}

		NotifyTipManager::instance()->addNotifyTip("删除成功~", NotifyTipBox::TypeSuccess);
		m_model->removeRow(m_delMsgBox->property("index").toModelIndex().row());
	});

	HTTP_HANDER_INSERT(ReqId::post_add) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		m_postAddDlg->clear();
		m_postAddDlg->hide();
	});

	HTTP_HANDER_INSERT(ReqId::post_edit) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
			return;
		}
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		m_postEditDlg->clear();
		m_postEditDlg->hide();
	});

}


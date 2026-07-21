#include "DeptMgrPage.h"
#include "common/utils/OverlayWidget.h"
#include "common/delegate/MappingDelegate.h"
#include "common/delegate/ButtonDelegate.h"
#include "common/widgets/MessageBox.h"
#include "common/notify/NotifyTipManager.h"

#include "domain/vo/TreeSelect.hpp"
#include "domain/entity/Dept.hpp"

#include "ContextHolder.h"
#include "DeptAddDlg.h"
#include "DeptEditDlg.h"
#include <QTimer>

DeptMgrPage::DeptMgrPage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DeptMgrPageClass())
	, m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	ui->statusCbx->setCurrentIndex(-1);

	ui->addBtn->setPermission("system:dept:add");

	ui->deptTreeView->setModel(m_model);
	ui->deptTreeView->setAnimated(true);
	ui->deptTreeView->setEditTriggers(QTreeView::NoEditTriggers);
	ui->deptTreeView->setSelectionMode(QTreeView::NoSelection);

	auto ctx = ContextHolder::instance();


	//映射委托
	auto mappingDelegate = new MappingDelegate(this);
	mappingDelegate->addMapping({ 0,"禁用",QColor(255, 237, 237),QColor(255, 219, 219),QColor(255, 73, 73) });
	mappingDelegate->addMapping({ 1,"正常",QColor(232, 244, 255),QColor(209, 233, 255),QColor(24, 144, 255) });
	//按钮委托
	m_buttonDelegate = new ButtonDelegate(ui->deptTreeView);
	auto buttonDelegate = m_buttonDelegate;
	if (ctx->hasPermission("system:dept:edit")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/pen.svg"), "修改");
	}
	if (ctx->hasPermission("system:dept:add")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/add.svg"), "新增");
	}
	if (ctx->hasPermission("system:dept:remove")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/del.svg"), "删除");
	}

	ui->deptTreeView->setItemDelegateForColumn(2, mappingDelegate);
	ui->deptTreeView->setItemDelegateForColumn(4, buttonDelegate);

	connect(buttonDelegate, &ButtonDelegate::clicked, this, [this](int id,const QModelIndex& index,ButtonDelegate::Data* d) {
		auto dept = index.data(Qt::UserRole).value<std::shared_ptr<Dept>>();
		if (!dept) {
			return;
		}
		if (d->text == "修改") {
			qDebug() << "修改";
			slot_update_dept(dept);
		}
		else if (d->text == "新增") {
			on_addBtn_clicked(dept);
			//m_deptAddDlg->setParentDept(dept);
		}
		else if (d->text == "删除") {
			qDebug() << "删除";
			auto box = delMsgBox();
			box->setMessage(QString("是否确认删除名称为\"%1\"的数据项?").arg(dept->dept_name));
			box->setProperty("dept", index.data(Qt::UserRole));
			box->setProperty("index", index);
			OverlayWidget::instance()->popup(box);
		}
		});

	ui->statusCbx->clear();
	ui->statusCbx->addItem("正常", 1);
	ui->statusCbx->addItem("禁用", 0);

	QTimer::singleShot(0, [this] {
		on_searchBtn_clicked();
		});

	HTTP_HANDLER_INIT(dept_mgr);

	if (ContextHolder::instance()->deptTreeSelect().empty()) {
		HttpMgr::instance()->get(URL("/dept/options"), ReqId::dept_treeselect, Modules::DeptMgr);
	}
}

DeptMgrPage::~DeptMgrPage()
{
	if (m_deptAddDlg)
		m_deptAddDlg->deleteLater();
	if (m_deptEditDlg)
		m_deptEditDlg->deleteLater();
	if (m_delMsgBox)
		m_delMsgBox->deleteLater();
	delete ui;
}

void DeptMgrPage::updateDeptTreeSelect()
{
	ContextHolder::instance()->clearDeptTreeSelect();
	HttpMgr::instance()->get(URL("/dept/options"), ReqId::dept_treeselect, Modules::DeptMgr);
}

void DeptMgrPage::on_resetBtn_clicked()
{
	ui->deptNameEdit->clear();
	ui->statusCbx->setCurrentIndex(-1);
	on_searchBtn_clicked();
}

static QList<QStandardItem*> GetDeptItems(const std::shared_ptr<Dept>& dept)
{
	QList<QStandardItem*> items;
	auto nameItem = items.emplaceBack(new QStandardItem(dept->dept_name));
	auto orderItem = items.emplaceBack(new QStandardItem(QString::number(dept->order_num)));
	auto activeItem = items.emplaceBack(new QStandardItem(QString::number(dept->is_active)));
	auto timeItem = items.emplaceBack(new QStandardItem(dept->create_time));
	auto optionItem = items.emplaceBack(new QStandardItem);
	optionItem->setData(QVariant::fromValue(dept), Qt::UserRole);

	nameItem->setTextAlignment(Qt::AlignCenter);
	orderItem->setTextAlignment(Qt::AlignCenter);
	activeItem->setTextAlignment(Qt::AlignCenter);
	timeItem->setTextAlignment(Qt::AlignCenter);

	if (dept->dept_id == 100) {
		optionItem->setData(QVariant::fromValue(QList{2}), ButtonDelegate::HideButtonRole);
	}

	return items;
}

static void recursionFn(const QList<std::shared_ptr<Dept>>& depts, QStandardItem* parentItem)
{
	for (auto& dept : depts) {
		auto items = GetDeptItems(dept);
		recursionFn(dept->children, items.first());
		parentItem->appendRow(items);
	}
}

void DeptMgrPage::on_searchBtn_clicked()
{
	//清空数据
	m_model->clear();

	//设置表头
	QStringList headers = { "部门名称","排序","状态","创建时间","操作"};
	m_model->setHorizontalHeaderLabels(headers);
	for (int i = 0; i < m_model->columnCount(); i++) {
		auto item = m_model->horizontalHeaderItem(i);
		if (item) {
			item->setTextAlignment(Qt::AlignCenter);
		}
	}
	//设置调整模式
	ui->deptTreeView->header()->setSectionResizeMode(4, QHeaderView::ResizeMode::Stretch);

	HttpMgr::instance()->get(URL("/dept/tree"), ReqId::dept_tree, Modules::DeptMgr, searchParams());
}

void DeptMgrPage::on_addBtn_clicked(const std::shared_ptr<Dept>& dept)
{
	if (!m_deptAddDlg) {
		m_deptAddDlg = new DeptAddDlg;
	}
	m_deptAddDlg->clear();
	m_deptAddDlg->setDeptTreeSelect(ContextHolder::instance()->deptTreeSelect());
	if (dept) {
		m_deptAddDlg->setDept(dept);
		m_deptAddDlg->setProperty("dept", QVariant::fromValue(dept));
	}
	OverlayWidget::instance()->popup(m_deptAddDlg.get());
}

void DeptMgrPage::on_expandBtn_clicked(bool checked)
{
	if (checked) {
		ui->deptTreeView->expandAll();
	}
	else {
		ui->deptTreeView->collapseAll();
	}
}

void DeptMgrPage::on_hideSearchBarBtn_clicked()
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

void DeptMgrPage::on_refreshBtn_clicked()
{
	on_searchBtn_clicked();
}

void DeptMgrPage::slot_update_dept(const std::shared_ptr<Dept>& dept)
{
	if (!m_deptEditDlg) {
		m_deptEditDlg = new DeptEditDlg;
	}
	m_deptEditDlg->clear();
	m_deptEditDlg->setDeptTreeSelect(ContextHolder::instance()->deptTreeSelect());
	m_deptEditDlg->setDept(dept);
	m_deptEditDlg->setProperty("dept",QVariant::fromValue(dept));
	OverlayWidget::instance()->popup(m_deptEditDlg.get());
}

QPointer<MessageBox> DeptMgrPage::delMsgBox()
{
	if (!m_delMsgBox) {
		m_delMsgBox = new MessageBox(MessageBox::Warning);
		connect(m_delMsgBox, &MessageBox::closed, this, [this](int ret) {
			if (ret == MessageBox::AcceptRole) {
				auto dept = m_delMsgBox->property("dept").value<std::shared_ptr<Dept>>();
				HttpMgr::instance()->del(URL("/dept/" + QString::number(dept->dept_id)), ReqId::dept_del, Modules::DeptMgr);
			}
			m_delMsgBox->close();
			});
	}
	return m_delMsgBox;
}

QVariantMap DeptMgrPage::searchParams() const
{
	QVariantMap params;

	//获取查询条件
	auto deptName = ui->deptNameEdit->text();
	if (!deptName.isEmpty()) {
		params["dept_name"] = deptName;
	}

	if (ui->statusCbx->currentIndex() !=-1) {
		params["is_active"] = ui->statusCbx->currentData().toInt();
	}

	return params;
}


HTTP_HANDLER_IMPL(DeptMgrPage, dept_mgr)
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

void DeptMgrPage::initHandlers()
{
	HTTP_HANDER_INSERT(ReqId::dept_tree) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
			return;
		}

		auto deptTree = json_t(json["data"]).cast<QList<std::shared_ptr<Dept>>>();
		if (deptTree.empty()) {
			NotifyTipManager::instance()->addNotifyTip("没有部门数据~", NotifyTipBox::TypeSuccess);
			return;
		}
		for (auto& dept : deptTree) {
			auto items = GetDeptItems(dept);
			recursionFn(dept->children, items.first());
			m_model->appendRow(items);
		}

		ui->deptTreeView->setColumnWidth(0, 290);
		ui->deptTreeView->setColumnWidth(1, 220);
		ui->deptTreeView->setColumnWidth(2, 90);
		ui->deptTreeView->setColumnWidth(3, 250);
		ui->deptTreeView->setColumnWidth(4, 180);
		if (m_buttonDelegate->buttonCount() <= 0) {
			ui->deptTreeView->setColumnHidden(m_model->columnCount() - 1, true);
		}
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

		if (m_deptAddDlg) {
			m_deptAddDlg->setDeptTreeSelect(deptTreeSelect);
			auto dept = m_deptAddDlg->property("dept").value<std::shared_ptr<Dept>>();
			if (dept) {
				m_deptAddDlg->setDept(dept);
			}
		}

		if (m_deptEditDlg) {
			m_deptEditDlg->setDeptTreeSelect(deptTreeSelect);
			auto dept = m_deptEditDlg->property("dept").value<std::shared_ptr<Dept>>();
			m_deptEditDlg->setDept(dept);
		}

		ContextHolder::instance()->setDeptTreeSelect(deptTreeSelect);
	});


	HTTP_HANDER_INSERT(ReqId::dept_del) {
			if (json["code"] != 200) {
				NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
				return;
			}

			NotifyTipManager::instance()->addNotifyTip("删除成功~", NotifyTipBox::TypeSuccess);
			auto index = m_delMsgBox->property("index").value<QModelIndex>();
			m_model->removeRow(index.row(), index.parent());
			updateDeptTreeSelect();
		});

	HTTP_HANDER_INSERT(ReqId::dept_add) {
			if (json["code"] != 200) {
				NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
				return;
			}

			m_deptAddDlg->hide();
			updateDeptTreeSelect();
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		});

	HTTP_HANDER_INSERT(ReqId::dept_edit) {
			if (json["code"] != 200) {
				NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
				return;
			}

			m_deptEditDlg->hide();
			updateDeptTreeSelect();
			NotifyTipManager::instance()->addNotifyTip("修改成功~", NotifyTipBox::TypeSuccess);
		});

}

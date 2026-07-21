#include "MenuMgrPage.h"
#include "common/utils/OverlayWidget.h"
#include "common/delegate/MappingDelegate.h"
#include "common/delegate/ButtonDelegate.h"
#include "common/widgets/MessageBox.h"
#include "common/notify/NotifyTipManager.h"

#include "domain/entity/Menu.hpp"
#include "domain/vo/TreeSelect.hpp"

#include "ContextHolder.h"
#include "MenuAddDlg.h"
#include "MenuEditDlg.h"
#include <QTimer>

MenuMgrPage::MenuMgrPage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::MenuMgrPageClass())
	, m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	ui->statusCbx->setCurrentIndex(-1);

	ui->addBtn->setPermission("system:menu:add");

	ui->menuTreeView->setModel(m_model);
	ui->menuTreeView->setAnimated(true);
	ui->menuTreeView->setEditTriggers(QTreeView::NoEditTriggers);
	ui->menuTreeView->setSelectionMode(QTreeView::NoSelection);

	auto ctx = ContextHolder::instance();


	//映射委托
	auto mappingDelegate = new MappingDelegate(this);
	mappingDelegate->addMapping({ 0,"禁用",QColor(255, 237, 237),QColor(255, 219, 219),QColor(255, 73, 73) });
	mappingDelegate->addMapping({ 1,"正常",QColor(232, 244, 255),QColor(209, 233, 255),QColor(24, 144, 255) });
	//按钮委托
	m_buttonDelegate = new ButtonDelegate(ui->menuTreeView);
	auto buttonDelegate = m_buttonDelegate;
	if (ctx->hasPermission("system:menu:edit")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/pen.svg"), "修改");
	}
	if (ctx->hasPermission("system:menu:add")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/add.svg"), "新增");
	}
	if (ctx->hasPermission("system:menu:remove")) {
		buttonDelegate->addButton(QPixmap(":/Resource/icons/delegate/del.svg"), "删除");
	}

	ui->menuTreeView->setItemDelegateForColumn(5, mappingDelegate);
	ui->menuTreeView->setItemDelegateForColumn(7, buttonDelegate);

	connect(buttonDelegate, &ButtonDelegate::clicked, this, [this](int id,const QModelIndex& index,ButtonDelegate::Data* d) {
		auto menu = index.data(Qt::UserRole).value<std::shared_ptr<Menu>>();
		if (!menu) {
			return;
		}
		if (d->text == "修改") {
			qDebug() << "修改";
			slot_update_menu(menu);
		}
		else if (d->text == "新增") {
			on_addBtn_clicked(menu);
		}
		else if (d->text == "删除") {
			qDebug() << "删除";
			auto box = delMsgBox();
			box->setMessage(QString("是否确认删除名称为\"%1\"的数据项?").arg(menu->menu_name));
			box->setProperty("menu", index.data(Qt::UserRole));
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


	HTTP_HANDLER_INIT(menu_mgr);
	if (ContextHolder::instance()->menuTreeSelect().empty()) {
		HttpMgr::instance()->get(URL("/menu/treeselect"), ReqId::menu_treeselect, Modules::MenuMgr);
	}
}

MenuMgrPage::~MenuMgrPage()
{
	if (m_menuAddDlg)
		m_menuAddDlg->deleteLater();
	if (m_menuEditDlg)
		m_menuEditDlg->deleteLater();
	if (m_delMsgBox)
		m_delMsgBox->deleteLater();
	delete ui;
}

void MenuMgrPage::on_resetBtn_clicked()
{
	ui->menuNameEdit->clear();
	ui->statusCbx->setCurrentIndex(-1);
	on_searchBtn_clicked();
}

static QList<QStandardItem*> GetMenuItems(const std::shared_ptr<Menu>& menu)
{
	QList<QStandardItem*> items;
	auto nameItem = items.emplaceBack(new QStandardItem(menu->menu_name));
	auto iconItem = items.emplaceBack(new QStandardItem);
	auto orderItem = items.emplaceBack(new QStandardItem(QString::number(menu->order_num)));
	auto permsItem = items.emplaceBack(new QStandardItem(menu->perms));
	auto comItem = items.emplaceBack(new QStandardItem(menu->component));
	auto activeItem = items.emplaceBack(new QStandardItem(QString::number(menu->is_active)));
	auto timeItem = items.emplaceBack(new QStandardItem(menu->create_time));
		auto optionItem = items.emplaceBack(new QStandardItem);
		optionItem->setData(QVariant::fromValue(menu), Qt::UserRole);

	nameItem->setTextAlignment(Qt::AlignCenter);
	iconItem->setTextAlignment(Qt::AlignCenter);
	orderItem->setTextAlignment(Qt::AlignCenter);
	permsItem->setTextAlignment(Qt::AlignCenter);
	activeItem->setTextAlignment(Qt::AlignCenter);
	timeItem->setTextAlignment(Qt::AlignCenter);

	auto iconPath = QString(":/Resource/icons/svg/%1.svg").arg(menu->icon);
	iconItem->setIcon(QIcon(iconPath));
	return items;
}

static void recursionFn(const QList<std::shared_ptr<Menu>>& menus, QStandardItem* parentItem)
{
	for (auto& menu : menus) {
		auto items = GetMenuItems(menu);
		recursionFn(menu->children, items.first());
		parentItem->appendRow(items);
	}
}

void MenuMgrPage::on_searchBtn_clicked()
{
	//清空数据
	m_model->clear();

	//设置表头
	QStringList headers = { "菜单名称","图标","排序","权限标识","组件路径","状态","创建时间","操作"};
	m_model->setHorizontalHeaderLabels(headers);
	for (int i = 0; i < m_model->columnCount(); i++) {
		auto item = m_model->horizontalHeaderItem(i);
		if (item) {
			item->setTextAlignment(Qt::AlignCenter);
		}
	}
	//设置调整模式
	ui->menuTreeView->header()->setSectionResizeMode(3,QHeaderView::ResizeMode::Stretch);
	ui->menuTreeView->header()->setSectionResizeMode(4,QHeaderView::ResizeMode::Stretch);


	HttpMgr::instance()->get(URL("/menu/tree"), ReqId::menu_tree, Modules::MenuMgr, searchParams());
}

void MenuMgrPage::on_addBtn_clicked(const std::shared_ptr<Menu>& menu)
{
	if (!m_menuAddDlg) {
		m_menuAddDlg = new MenuAddDlg;
	}
	m_menuAddDlg->clear();
	m_menuAddDlg->setMenuTreeSelect(ContextHolder::instance()->menuTreeSelect());
	if (menu) {
		m_menuAddDlg->setMenu(menu);
	}
	OverlayWidget::instance()->popup(m_menuAddDlg.get());
}

void MenuMgrPage::on_expandBtn_clicked(bool checked)
{
	if (checked) {
		ui->menuTreeView->expandAll();
	}
	else {
		ui->menuTreeView->collapseAll();
	}
}

void MenuMgrPage::on_hideSearchBarBtn_clicked()
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

void MenuMgrPage::on_refreshBtn_clicked()
{
	on_searchBtn_clicked();
}

void MenuMgrPage::slot_update_menu(const std::shared_ptr<Menu>& menu)
{
	if (!m_menuEditDlg) {
		m_menuEditDlg = new MenuEditDlg;
	}
	m_menuEditDlg->clear();
	m_menuEditDlg->setMenuTreeSelect(ContextHolder::instance()->menuTreeSelect());
	m_menuEditDlg->setMenu(menu);
	OverlayWidget::instance()->popup(m_menuEditDlg.get());
}

QPointer<MessageBox> MenuMgrPage::delMsgBox()
{
	if (!m_delMsgBox) {
		m_delMsgBox = new MessageBox(MessageBox::Warning);
		connect(m_delMsgBox, &MessageBox::closed, this, [this](int ret) {
			if (ret == MessageBox::AcceptRole) {
				auto menu = m_delMsgBox->property("menu").value<std::shared_ptr<Menu>>();
				auto index = m_delMsgBox->property("index").value<QModelIndex>();
				HttpMgr::instance()->del(URL("/menu/" + QString::number(menu->menu_id)), ReqId::menu_del, Modules::MenuMgr);
			}
			m_delMsgBox->close();
			});
	}
	return m_delMsgBox;
}

QVariantMap MenuMgrPage::searchParams()
{
	QVariantMap params;
	//获取查询条件
	auto menuName = ui->menuNameEdit->text();
	if (!menuName.isEmpty()) {
		params["menu_name"] = menuName;
	}

	if (ui->statusCbx->currentIndex() !=-1) {
		params["is_active"] = ui->statusCbx->currentData().toInt();
	}

	return params;
}


HTTP_HANDLER_IMPL(MenuMgrPage, menu_mgr)
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

void MenuMgrPage::initHandlers()
{

	HTTP_HANDER_INSERT(ReqId::menu_tree) {
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
			return;
		}

		auto menuTree = json_t(json["data"]).cast<QList<std::shared_ptr<Menu>>>();
		if (menuTree.empty()) {
			NotifyTipManager::instance()->addNotifyTip("没有菜单数据~", NotifyTipBox::TypeSuccess);

			return;
		}
		for (auto& menu : menuTree) {
			auto items = GetMenuItems(menu);
			recursionFn(menu->children, items.first());
			m_model->appendRow(items);
		}

		ui->menuTreeView->setColumnWidth(0, 180);
		ui->menuTreeView->setColumnWidth(1, 30);
		ui->menuTreeView->setColumnWidth(2, 50);
		ui->menuTreeView->setColumnWidth(3, 120);
		ui->menuTreeView->setColumnWidth(4, 155);
		ui->menuTreeView->setColumnWidth(5, 155);
		ui->menuTreeView->setColumnWidth(6, 180);
		if (m_buttonDelegate->buttonCount() <= 0) {
			ui->menuTreeView->setColumnHidden(m_model->columnCount() - 1, true);
		}
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

		if (m_menuAddDlg) {
			m_menuAddDlg->setMenuTreeSelect(menuTreeSelect);
			auto menu= m_menuAddDlg->property("menu").value<std::shared_ptr<Menu>>();
			if (menu) {
				m_menuAddDlg->setMenu(menu);
			}
		}

		if (m_menuEditDlg) {
			m_menuEditDlg->setMenuTreeSelect(menuTreeSelect);
			auto menu = m_menuEditDlg->property("menu").value<std::shared_ptr<Menu>>();
			if (menu) {
				m_menuEditDlg->setMenu(menu);
			}
		}
		ContextHolder::instance()->setMenuTreeSelect(menuTreeSelect);
	});

	HTTP_HANDER_INSERT(ReqId::menu_del) {
			if (json["code"] != 200) {
				NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
				return;
			}

			NotifyTipManager::instance()->addNotifyTip("删除成功~", NotifyTipBox::TypeSuccess);
			auto index = m_delMsgBox->property("index").value<QModelIndex>();
			m_model->removeRow(index.row(), index.parent());
		});

	HTTP_HANDER_INSERT(ReqId::menu_add) {
			if (json["code"] != 200) {
				NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
				return;
			}

			m_menuAddDlg->hide();
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		});

	HTTP_HANDER_INSERT(ReqId::menu_edit) {
			if (json["code"] != 200) {
				NotifyTipManager::instance()->addNotifyTip(json["msg"].toString());
				return;
			}

			m_menuEditDlg->hide();
			NotifyTipManager::instance()->addNotifyTip("修改成功~", NotifyTipBox::TypeSuccess);
		});

}

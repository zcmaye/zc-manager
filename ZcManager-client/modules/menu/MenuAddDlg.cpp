#include "MenuAddDlg.h"
#include "common/constants/UserConstants.hpp"
#include "common/notify/NotifyTipManager.h"
#include "common/widgets/IconSelectView.h"
#include "common/network/HttpMgr.h"

#include "domain/entity/Menu.hpp"
#include "domain/vo/TreeSelect.hpp"

#include <QTreeView>
#include <QStandardItemModel>
#include <QHeaderView>


MenuAddDlg::MenuAddDlg(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::MenuAddDlgClass())
{
	ui->setupUi(this);

	connect(ui->dTypeRBtn, &QRadioButton::toggled, [this](bool checked) {if(checked)slot_cut_page(UserConstants::TYPE_DIR); });
	connect(ui->mTypeRBtn, &QRadioButton::toggled, [this](bool checked) {if(checked)slot_cut_page(UserConstants::TYPE_MENU); });
	connect(ui->bTypeRBtn, &QRadioButton::toggled, [this](bool checked) {if(checked)slot_cut_page(UserConstants::TYPE_BUTTON); });

	//图标选择
	auto iconView = new IconSelectView;
	iconView->addIconPath(":/Resource/icons/svg");
	connect(iconView, &IconSelectView::iconChanged, this, [this](const QString& name, const QIcon& icon) {
		ui->menuIconEdit->setText(name);
		ui->menuIconEdit->leadingAction()->setIcon(icon);
		ui->menuIconEdit->hidePopup();
		});

	ui->menuIconEdit->setReadOnly(false);
	ui->menuIconEdit->setWidget(iconView);
}

MenuAddDlg::~MenuAddDlg()
{
	delete ui;
}

static void recursionFn(const QList<std::shared_ptr<TreeSelect>>& treeSelect, QStandardItem* parentItem)
{
	for (auto& ts: treeSelect) {
		auto item = new QStandardItem(ts->label);
		item->setData(QVariant::fromValue(ts), Qt::UserRole);
		recursionFn(ts->children, item);
		parentItem->appendRow(item);
	}
}

void MenuAddDlg::setMenuTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect)
{
	auto m = dynamic_cast<QStandardItemModel*>(ui->parentMenuCbx->model());
	if (!m) {
		qWarning() << "model is nullptr!";
		return;
	}
	auto item = new QStandardItem("主目录");
	recursionFn(treeSelect, item);
	m->appendRow(item);
}

void MenuAddDlg::setMenu(const std::shared_ptr<Menu>& menu)
{
	if (!menu)
		return;

	auto w = ui->parentMenuCbx;

	auto index = w->findText(menu->menu_name,Qt::MatchFlag::MatchRecursive);
	if (!index.isValid()) {
		index = w->findData(QVariant::fromValue(menu), [](const QVariant& left, const QVariant& right) {
			auto ts = left.value<std::shared_ptr<TreeSelect>>();
			auto m = right.value<std::shared_ptr<Menu>>();
			if (ts && m) {
				return ts->id == m->menu_id;
			}
			return false;
			});
		if (index.isValid()) {
			qWarning() << menu->menu_name << "not found!";
			return;
		}
	}
	w->setCurrentIndex(index);
}

void MenuAddDlg::clear()
{
	ui->parentMenuCbx->clear();
	ui->mTypeRBtn->setChecked(true);
	ui->menuIconEdit->clear();
	ui->menuNameEdit->clear();
	ui->routerNameEdit->clear();
	ui->isFrameRBtn_0->setChecked(true);
	ui->routerUrlEdit->clear();
	ui->componentEdit->clear();
	ui->permsEdit->clear();
	ui->visibleRBtn_1->setChecked(true);
	ui->statusRBtn_1->setChecked(true);
}

void MenuAddDlg::on_okBtn_clicked()
{
	auto orderNum = ui->orderNumBox->value();
	auto menuName= ui->menuNameEdit->text();
	auto path = ui->routerUrlEdit->text();

	auto type = getMenuType();

	if (orderNum < 0) {
		NotifyTipManager::instance()->addNotifyTip("显示排序不能为负数");
		return;
	}
	if (menuName.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("菜单名不能为空");
		return;
	}
	if (path.isEmpty() && type != UserConstants::TYPE_BUTTON) {
		NotifyTipManager::instance()->addNotifyTip("路由地址不能为空");
		return;
	}

	auto menu = std::make_shared<Menu>();
	menu->menu_type = type;

	auto parent = ui->parentMenuCbx->currentData(Qt::UserRole).value<std::shared_ptr<TreeSelect>>();
	if (!parent)
		menu->parent_id = 0;
	else
		menu->parent_id = parent->id;

	if (type == UserConstants::TYPE_DIR) {
		menu->icon = ui->menuIconEdit->text();
		menu->order_num = ui->orderNumBox->value();
		menu->menu_name = ui->menuNameEdit->text();
		menu->path = ui->routerUrlEdit->text();
		menu->is_frame = ui->isFrameRBtn_0->isChecked() ? 0 : 1;
		menu->is_visible= ui->visibleRBtn_0->isChecked() ? 0 : 1;
		menu->is_active = ui->statusRBtn_0->isChecked() ? 0 : 1;
	}
	else if (type == UserConstants::TYPE_MENU) {
		menu->icon = ui->menuIconEdit->text();
		menu->order_num = ui->orderNumBox->value();
		menu->menu_name = ui->menuNameEdit->text();
		menu->path = ui->routerUrlEdit->text();
		menu->component = ui->componentEdit->text();
		menu->perms = ui->permsEdit->text();
		menu->is_frame = ui->isFrameRBtn_0->isChecked() ? 0 : 1;
		menu->is_visible= ui->visibleRBtn_0->isChecked() ? 0 : 1;
		menu->is_active = ui->statusRBtn_0->isChecked() ? 0 : 1;

	}
	else if (type == UserConstants::TYPE_BUTTON) {
		menu->order_num = ui->orderNumBox->value();
		menu->menu_name = ui->menuNameEdit->text();
		menu->perms = ui->permsEdit->text();
		menu->is_active = ui->statusRBtn_0->isChecked() ? 0 : 1;
	}

	HttpMgr::instance()->post(URL("/menu/add"), menu, ReqId::menu_add, Modules::MenuMgr);
}

void MenuAddDlg::slot_cut_page(const QString& type)
{
	qDebug() << type;
	//b -> d
	ui->menuIconWdgt->show();
	ui->orderNumWdgt->show();
	ui->menuNameEdit->show();
	ui->routerNameWdgt->show();
	ui->routerUrlWdgt->show();
	ui->isFrameWdgt->show();
	ui->componentWdgt->show();
	ui->permsWdgt->show();
	ui->visibleWdgt->show();
	ui->statusWdgt->show();

	if (type == UserConstants::TYPE_DIR) {
		//m -> d
		ui->routerNameWdgt->hide();
		ui->componentWdgt->hide();
		ui->permsWdgt->hide();

		//显示排序
		moveTo(ui->orderNumWdgt, 0, 1);
		//权限字符
		moveTo(ui->permsWdgt, 3, 1);
		//菜单状态
		moveTo(ui->statusWdgt, 4, 1);
	}
	else if (type == UserConstants::TYPE_MENU) {
		//d -> m
		//ui->routerNameWdgt->show();
		//ui->componentWdgt->show();
		//ui->permsWdgt->show();

		//显示排序
		moveTo(ui->orderNumWdgt, 0, 1);
		//权限字符
		moveTo(ui->permsWdgt, 3, 1);
		//菜单状态
		moveTo(ui->statusWdgt, 4, 1);
	}
	else if (type == UserConstants::TYPE_BUTTON) {
		ui->menuIconWdgt->hide();
		ui->routerNameWdgt->hide();
		ui->routerUrlWdgt->hide();
		ui->isFrameWdgt->hide();
		ui->componentWdgt->hide();
		ui->visibleWdgt->hide();

		//显示排序
		moveTo(ui->orderNumWdgt, 0, 0);
		//权限字符
		moveTo(ui->permsWdgt, 3, 0);
		//菜单状态
		moveTo(ui->statusWdgt, 4, 0);
		auto glayout = dynamic_cast<QGridLayout*>(ui->centeral->layout());
		glayout->addItem(new QSpacerItem(100, 20, QSizePolicy::Policy::Expanding),0,1);
	}
}

void MenuAddDlg::moveTo(QWidget* w, int row, int col)
{
	auto glayout = dynamic_cast<QGridLayout*>(ui->centeral->layout());
	//移动布局中的元素
	auto index = glayout->indexOf(w);
	auto item = glayout->takeAt(index);
	glayout->addItem(item, row, col);
}

const char* MenuAddDlg::getMenuType()
{
	if (ui->dTypeRBtn->isChecked()) return UserConstants::TYPE_DIR;
	if (ui->mTypeRBtn->isChecked()) return UserConstants::TYPE_MENU;
	if (ui->bTypeRBtn->isChecked()) return UserConstants::TYPE_BUTTON;
	return "M";
}


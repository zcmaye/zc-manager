#include "HomePage.h"
#include "ContextHolder.h"
#include "domain/entity/User.hpp"
#include "common/base/Event.hpp"
#include "common/utils/StringUtils.h"
#include "common/notify/NotifyTipManager.h"
#include "common/base/GlobalSignalHub.hpp"

#include "modules/personal-info/PersonalInfoPage.h"
#include "modules/menu/MenuMgrPage.h"
#include "modules/user/UserMgrPage.h"
#include "modules/role/RoleMgrPage.h"
#include "modules/role/AssignUserRolePage.h"
#include "modules/dept/DeptMgrPage.h"
#include "modules/post/PostMgrPage.h"
#include "Home.h"

#include <QMenu>
#include <QDesktopServices>
#include <QApplication>

HomePage::HomePage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::HomePageClass())
{
	ui->setupUi(this);
	setAttribute(Qt::WA_StyledBackground);

	initPersonalMenu();
	ui->tabWidget->installEventFilter(this);
	ui->tabWidget->setTabsClosable(true);

	connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index) {
		auto w = ui->tabWidget->widget(index);
		w->deleteLater();
		});
	connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
		auto name = ui->tabWidget->widget(index)->objectName();
		ui->navTreeView->setCurrentNavigation(name);
		auto list = ui->navTreeView->navigationNameList();
		if (list.isEmpty())
			ui->navLocationLab->clear();
		else
			ui->navLocationLab->setText("/ " + list.join(" / "));
		});

	//点击头像显示菜单
	//connect(ui->personalBtn, &HoverButton::entered, this, [this]() {
	//	if (m_personalMenu->isHidden()) {
	//		QPoint pos(ui->personalBtn->x() - (m_personalMenu->width() - ui->personalBtn->width()), ui->personalBtn->y() + ui->personalBtn->height() + 10);
	//		pos = ui->topBar->mapToGlobal(pos);
	//		m_personalMenu->popup(pos);
	//	}
	//	});
	//connect(ui->personalBtn, &HoverButton::exited, this, [this]() {
	//	m_personalMenu->hide();
	//	});

	connect(ui->goHomeBtn, &QPushButton::clicked, this, [this] {
		slot_cut_page(ui->navTreeView->model()->index(0, 0));
		});
	connect(ui->navTreeView, &QTreeView::clicked, this, &HomePage::slot_cut_page);
	connect(ui->navTreeView, &NavTreeView::navigationChanged, this, [this](const QStringList& list)
		{
			if (list.isEmpty())
				ui->navLocationLab->clear();
			else
				ui->navLocationLab->setText("/ " + list.join(" / "));
		});

	//:/Resource/icons/svg/tree-table.svg
	setCurrentPage("home", "首页");
	ui->tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);		//将首页tab的关闭按钮去掉

	//当头像变化时，进行更新
	connect(GlobalSignalHub::instance().get(), &GlobalSignalHub::avatarChanged, this, [this](QPixmap pix) {
		ui->personalBtn->setIcon(Utils::toRoundPixamp(pix));
		});
}

HomePage::~HomePage()
{
	delete ui;
}

void HomePage::setRouters(const QList<std::shared_ptr<RouterVo>>& routers)
{
	ui->navTreeView->setRouters(routers);
}

void HomePage::setCurrentPage(const QString& component, const QString& title)
{
	//if (component == "system/user/index") {
	//	qDebug() << title;
	//}
	//else if (component == "system/menu/index") {
	//	qDebug() << title;
	//}
	qDebug() << component;
	QWidget* page = nullptr;
	switch (Utils::OptionHash(qPrintable(component)))
	{
	case Utils::OptionHash("home"):
		page = addPage<Home>(title);
		break;
	case Utils::OptionHash("personal/info"):
		page = addPage<PersonalInfoPage>(title);
		break;
	case Utils::OptionHash("system/user/index"):
		page = addPage<UserMgrPage>(title);
		break;
	case Utils::OptionHash("system/menu/index"):
		page = addPage<MenuMgrPage>(title);
		break;
	case Utils::OptionHash("system/role/index"):
		page = addPage<RoleMgrPage>(title);
		break;
	case Utils::OptionHash("system/roleAssign/index"):
		page = addPage<AssignUserRolePage>(title);
		break;
	case Utils::OptionHash("system/dept/index"):
		page = addPage<DeptMgrPage>(title);
		break;
	case Utils::OptionHash("system/post/index"):
		page = addPage<PostMgrPage>(title);
		break;
	default:
		break;
	}
	if (page) {
		ui->tabWidget->setCurrentWidget(page);
		ui->tabWidget->update();
	}

}

void HomePage::showEvent(QShowEvent* ev)
{
	auto user = ContextHolder::instance()->self();
	ui->personalBtn->setIcon(user->avatarPixmap());
}

bool HomePage::eventFilter(QObject* watched, QEvent* ev)
{
	if (watched == ui->tabWidget) {
		if (ev->type() == StackedWidgetAddPageEvent::type) {
			auto sev = dynamic_cast<StackedWidgetAddPageEvent*>(ev);
			//先添加
			setCurrentPage(sev->component(),sev->title());
			//然后查找
			auto assingUserRolePage = ui->tabWidget->findChild<AssignUserRolePage*>();
			if (assingUserRolePage) {
				assingUserRolePage->setRole(sev->role());
			}
		}
	}
	return false;
}

void HomePage::slot_cut_page(const QModelIndex& index)
{
	if (!index.isValid()) {
		qWarning() << "index is invalid:" << index;
		return;
	}
	auto name = index.data(Qt::DisplayRole).toString();
	if (name == "首页") {
		setCurrentPage("home", "首页");
	}

	auto router = index.data(Qt::UserRole).value<std::shared_ptr<RouterVo>>();
	if (!router) {
		qWarning() << "router is nullptr" << name;
		return;
	}
	//如果是外链接，用浏览器打开
	if (StringUtils::isHttp(router->meta->link)) {
		if (QDesktopServices::openUrl(QUrl(router->meta->link))) {
			NotifyTipManager::instance()->addNotifyTip("打开成功~", NotifyTipBox::TypeSuccess);
		}
		else {
			NotifyTipManager::instance()->addNotifyTip(QString("%1 打开失败~").arg(router->meta->link));
		}
		return;
	}

	//qDebug() << router->meta->title << router->path << router->component << router->name;
	//根据component切换页面
	auto& component = router->component;
	setCurrentPage(component, router->meta->title);
}

void HomePage::initPersonalMenu()
{
	m_personalMenu = new QMenu(this);
	m_personalMenu->addAction("个人信息", [this] {setCurrentPage("personal/info", "个人信息"); });
	m_personalMenu->addSeparator();
	m_personalMenu->addAction("退出登录",this,&HomePage::sig_quit_login);

	ui->personalBtn->setMenu(m_personalMenu);
}


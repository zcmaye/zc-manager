#include "NavTreeView.h"
#include "common/utils/Utils.h"
#include "common/delegate/NavTreeDelegate.h"
#include "common/constants/UserConstants.hpp"
#include <QTimer>
#include <QUrl>

NavTreeView::NavTreeView(QWidget* parent)
	: QTreeView(parent)
	, m_model(new QStandardItemModel(this))
{
	setAttribute(Qt::WA_StyledBackground);
	setModel(m_model);
	setEditTriggers(QTreeView::NoEditTriggers);
	setAnimated(true);
	setFocusPolicy(Qt::NoFocus);
	setExpandsOnDoubleClick(false);

	connect(this, &QTreeView::clicked, this, &NavTreeView::slot_clicked);
	//通过委托来绘制导航树
	setItemDelegateForColumn(0, new NavTreeDelegate(this));
	setIndentation(0);



	//test();
}

NavTreeView::~NavTreeView()
{
}

QStringList NavTreeView::navigationNameList() const
{
	QStringList list;

	auto index = currentIndex();
	while (index.isValid()) {
		list.prepend(index.data(Qt::DisplayRole).toString());
		index = index.parent();
	}

	return list;
}

void NavTreeView::setCurrentNavigation(const QString& name)
{
	auto items =  m_model->findItems(name,Qt::MatchFlag::MatchRecursive);
	if (items.isEmpty()) {
		qWarning() << "name is " << name << " nav not foun!";
		return;
	}
	setCurrentIndex(items.first()->index());
}

void NavTreeView::test()
{
	//-- 头
	m_model->setHorizontalHeaderItem(0, new QStandardItem(QIcon(":/Resource/images/icon.png"), "学生管理系统"));

	//-- 首页
	auto item = new QStandardItem(Utils::SetIconColor(QIcon(":/Resource/icons/svg/dashboard.svg")), "首页");
	item->setSelectable(false);
	m_model->appendRow(item);

	//-- 系统管理
	item = new QStandardItem(Utils::SetIconColor(QIcon(":/Resource/icons/svg/system.svg")), "系统管理");
	item->setSelectable(false);
	item->appendRow(new QStandardItem(Utils::SetIconColor(QIcon(":/Resource/icons/svg/user.svg")), "用户管理"));
	item->appendRow(new QStandardItem(Utils::SetIconColor(QIcon(":/Resource/icons/svg/people.svg")), "学生管理管理"));

	auto logItem = new QStandardItem("日志管理");
	logItem->setSelectable(false);
	logItem->appendRow(new QStandardItem("操作日志"));
	logItem->appendRow(new QStandardItem("登录日志"));
	item->appendRow(logItem);
	m_model->appendRow(item);

}

QStandardItem* createItem(const std::shared_ptr<RouterVo>& router)
{
	auto item = new QStandardItem(router->meta->title);
	auto iconPath = QString(":/Resource/icons/svg/%1.svg").arg(router->meta->icon);
	qDebug() << iconPath;
	item->setIcon(Utils::SetIconColor(QIcon(iconPath)));
	item->setData(QVariant::fromValue(router), Qt::UserRole);
	if (router->redirect == UserConstants::NO_REDIRECT) {
		item->setSelectable(false);
	}
	return item;
}

template<typename T>
static void recursionFn(const QList<std::shared_ptr<RouterVo>>& routers, T* parent_item)
{
	if (routers.isEmpty()) {
		return;
	}

	for (auto& router : routers) {
		if (router->hidden) {
			continue;
		}
		auto item = createItem(router);
		recursionFn(router->children, item);
		parent_item->appendRow(item);
	}
}

void NavTreeView::setRouters(const QList<std::shared_ptr<RouterVo>>& routers)
{
	m_model->clear();

	//-- 头
	m_model->setHorizontalHeaderItem(0, new QStandardItem(QIcon(":/Resource/images/icon.png"), "学生管理系统"));

	//-- 首页
	auto item = new QStandardItem(Utils::SetIconColor(QIcon(":/Resource/icons/svg/dashboard.svg")),"首页");
	item->setSelectable(false);
	m_model->appendRow(item);

	if (routers.isEmpty()) {
		qWarning() << "routers is empty!";
		return;
	}
	recursionFn(routers, m_model);
}

void NavTreeView::autoExpanded(const QModelIndex& index)
{
	//展开或关闭	
	setExpanded(index, !isExpanded(index));

	//关闭其他的
	QTimer::singleShot(100, [this, index] {
		for (int i = 0; i < m_model->rowCount(); i++) {
			auto idx = m_model->index(i, 0);

			//idx是不是index的父级别
			bool isP = Utils::isParent(index, idx);
			if (isP) {
				continue;
			}

			//如果有效并且是展开的，而且不是当前操作的
			if (idx.isValid() && isExpanded(idx) && idx != index) {
				setExpanded(idx, false);
				break;
			}
		}
		});
}

void NavTreeView::slot_clicked(const QModelIndex& index)
{
	//如果是顶级Item
	if (!index.parent().isValid()) {
		//有子Item
		if (m_model->itemFromIndex(index)->hasChildren()) {
			autoExpanded(index);
		}
		//没有孩子
		else {
			auto text = index.data(Qt::DisplayRole).toString();
			if (text == "首页") {
				emit navigationChanged({});
			}
			//else if (text == "宏定义官网") {
			//	QDesktopServices::openUrl(QUrl("https://appmbamikht2838.h5.xiaoeknow.com"));
			//}
		}
	}
	//是子item
	else {
		//有子Item
		if (m_model->itemFromIndex(index)->hasChildren()) {
			autoExpanded(index);
		}
		else {
			emit navigationChanged(navigationNameList());
		}
	}
}


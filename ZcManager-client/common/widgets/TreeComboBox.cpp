#include "TreeComboBox.h"
#include <QHeaderView>
#include <QEvent>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QLineEdit>

TreeComboBox::TreeComboBox(QWidget *parent)
	: QComboBox(parent)
	, m_isAllowHidePopup(false)
{
	setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToContents);
	setEditable(true);
	lineEdit()->setReadOnly(true);
	//上级菜单选择
	auto view = new QTreeView;
	view->header()->hide();
	view->setContentsMargins(0, 10, 0, 10);
	setView(view);

	//查找容器
	auto frame = findChild<QFrame*>();
	if (frame) {
		frame->installEventFilter(this);
	}

	if(lineEdit())
		lineEdit()->installEventFilter(this);
}

TreeComboBox::~TreeComboBox()
{}

void TreeComboBox::setView(QTreeView * view)
{
	QComboBox::setView(view);
	if (view) {
		view->viewport()->installEventFilter(this);
	}
}

QTreeView* TreeComboBox::view() const
{
	return dynamic_cast<QTreeView*>(QComboBox::view());
}

QVariant TreeComboBox::currentData(int role) const
{
	return model()->data(view()->currentIndex(), role);
}

QModelIndex TreeComboBox::currentIndex() const
{
	return view()->currentIndex();
}

void TreeComboBox::setCurrentIndex(const QModelIndex& index) 
{
	view()->setCurrentIndex(index);
	setEditText(index.data(Qt::DisplayRole).toString());
}

static QModelIndex recursionFn(QStandardItem* item,const QVariant& data, const std::function<bool(QVariant, QVariant)>& fun, int role)
{
	for (int i = 0; i < item->rowCount(); i++) {
		auto child = item->child(i);
		if (fun(child->data(role), data)) {
			return child->index();
		}
		auto index = recursionFn(child, data, fun, role);
		if (index.isValid()) {
			return index;
		}
	}
	return QModelIndex();
}


QModelIndex TreeComboBox::findData(const QVariant& data, const std::function<bool(QVariant, QVariant)>& fun, int role) const
{
	auto m = dynamic_cast<QStandardItemModel*>(model());
	if (!m) {
		qWarning() << "model is nullptr";
		return {};
	}

	for (int i = 0; i < model()->rowCount(); i++) {
		auto index =  model()->index(i, 0);
		if (fun(index.data(role), data)) {
			return index;
		}
		index = recursionFn(m->itemFromIndex(index),data,fun,role);
		if (index.isValid()) {
			return index;
		}
	}
	return QModelIndex();
}

QModelIndex TreeComboBox::findText(const QString& text, Qt::MatchFlags flags) const
{
	auto m = dynamic_cast<QStandardItemModel*>(model());
	if (!m) {
		qWarning() << "model is nullptr";
		return {};
	}

	auto items = m->findItems(text, flags);
	if (items.isEmpty()) {
		return QModelIndex();
	}

	return items.first()->index();
}

bool TreeComboBox::eventFilter(QObject* watched, QEvent* ev)
{
	auto frame = findChild<QFrame*>();
	if (watched == view()->viewport()) {
		auto vport = view()->viewport();
		if (ev->type() == QEvent::MouseButtonRelease) {
			auto mouseEvent = dynamic_cast<QMouseEvent*>(ev);
			//qDebug() << vport->geometry() << mouseEvent->pos();
			//获取当前索引
			auto index = view()->currentIndex();
			//获取当前项的item区域
			auto vrect = view()->visualRect(index);	//(20,0,w,h)
			//如果点击的是展开/折叠区域
			auto erect = QRect(0, vrect.y(), vrect.x(), vrect.height());
			if (erect.contains(mouseEvent->pos()) && model()->hasChildren(index)) {
				auto h = hitHeight();
				frame->setFixedHeight(h);
				qDebug() << "expanded" << h;
				m_isAllowHidePopup = false;
				return true;
			}
			else {
				qDebug() << "item";
				m_isAllowHidePopup = true;
			}
		}
	}
	else if (watched == frame) {
		if (ev->type() == QEvent::MouseButtonRelease) {
			auto mouseEvent = dynamic_cast<QMouseEvent*>(ev);
			//qDebug() << frame->geometry() << mouseEvent->pos();
			if (!frame->geometry().contains(mouseEvent->pos())) {
				m_isAllowHidePopup = true;
			}
		}
	}
	else if (watched = lineEdit()) {
		static bool pressed = false;
		if (ev->type() == QEvent::MouseButtonRelease) {
			if (pressed) {
				showPopup();
				pressed = false;
			}
		}
		else if (ev->type() == QEvent::MouseButtonPress) {
			pressed = true;
		}
	}
	return false;
}

void TreeComboBox::showPopup()
{
	QComboBox::showPopup();
	m_isAllowHidePopup = false;
}

void TreeComboBox::hidePopup()
{
	if (m_isAllowHidePopup) {
		QComboBox::hidePopup();
	}
}

int TreeComboBox::hitHeight()
{
	auto m = dynamic_cast<QStandardItemModel*>(model());
	if (!m) {
		auto frame = findChild<QFrame*>();
		return frame->height();
	}

	int h = 0;
	for (int i = 0; i < m->rowCount(); i++) {
		auto item = m->item(i, 0);
		h+= recursionHeight(item);
	}

	return qMin(h, 400);
}

int TreeComboBox::recursionHeight(QStandardItem* parent)
{
	int h = 0;
	h += view()->visualRect(parent->index()).height();

	for (int i = 0; i < parent->rowCount(); i++) {
		auto item = parent->child(i, 0);
		h+= recursionHeight(item);
	}

	return h + 20;
}




#include "ButtonDelegate.h"
#include <QTreeView>
#include <QTableView>
#include <QListView>
#include <QHeaderView>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>

ButtonDelegate::ButtonDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
	, m_spacing(6)
	, m_iconSize(16)
	, m_font("微软雅黑",10)
	, m_margins(2,9,2,9)
	, m_isInit(false)
	, m_index(-1)
{
	if (parent && headerView()) {
		headerView()->installEventFilter(this);
		parent->installEventFilter(this);
	}
}

ButtonDelegate::~ButtonDelegate()
{}

void ButtonDelegate::setSpacing(int spacing)
{
	m_spacing = spacing;
}

void ButtonDelegate::setIconSize(int size)
{
	m_iconSize = size;
}

void ButtonDelegate::setFont(const QFont& font)
{
	m_font = font;
}

void ButtonDelegate::addButton(const Data & d)
{
	auto data = std::make_shared<Data>();
	*data = d;
	m_datas.append(data);
}

void ButtonDelegate::addButton(const QPixmap& icon, const QString& text, const QColor& fg, const QColor& pressed)
{
	auto data = std::make_shared<Data>();
	data->icon = icon;
	data->text = text;
	data->forgeground = fg;
	data->pressedColor = pressed;
	m_datas.append(data);
}

int ButtonDelegate::buttonCount() const
{
	return m_datas.size();
}

void ButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);
	if (m_datas.isEmpty()) {
		return;
	}

	//判断当前行是否隐藏所有按钮
	if (index.data(HideRowRole).toBool()) {
		return;
	}
	//获取当前行隐藏的按钮索引
	auto hideIndexes = index.data(HideButtonRole).value<QList<int>>();
	int xOffset = 0;
	if (!hideIndexes.isEmpty()) {
		//计算隐藏的按钮的总宽度
		int totalW = 0;
		for (int i = 0; i < m_datas.size(); i++) {
			if (hideIndexes.contains(i)) {
				totalW += m_datas[i]->rect.width();
			}
		}
		//
		xOffset = totalW / 2;
	}

	if (!m_isInit) {
		updateButtons(option.rect);
		m_isInit = true;
	}
	painter->save();
	painter->setFont(m_font);
	//设置裁剪矩形
	painter->setClipRect(option.rect);
	//绘制按钮
	for (int i = 0; i < m_datas.size(); i++) {
		auto& d = m_datas[i];
		if (hideIndexes.contains(i)) {
			continue;
		}
		//校准按钮矩形
		auto rect = d->rect;
		rect.moveLeft(d->rect.x() + option.rect.x() + xOffset);
		rect.moveTop(d->rect.y() + option.rect.y());
		//测试背景
		//painter->setPen(Qt::green);
		//painter->drawRoundedRect(rect, 5, 5);

		//绘制icon
		auto iconRect = QRect(rect.x(),rect.y() + (rect.height() - m_iconSize) / 2, m_iconSize, m_iconSize);
		painter->drawPixmap(iconRect, d->icon);
		//qDebug() << m_index;
		//绘制文本
		if (m_index == i && m_currentIndex == index) {
			painter->setPen(d->pressedColor);
		}
		else {
			painter->setPen(d->forgeground);
		}
		painter->drawText(rect.adjusted(m_iconSize, 0, 0, 0), d->text, QTextOption(Qt::AlignCenter));
	}
	painter->restore();
}

bool ButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (event->type() == QEvent::MouseButtonPress) {
		auto ev = dynamic_cast<QMouseEvent*>(event);
		if (ev->button() == Qt::LeftButton) {

			//判断当前行是否隐藏所有按钮
			if (index.data(HideRowRole).toBool()) {
				return true;
			}

			//获取当前行隐藏的按钮索引
			auto hideIndexes = index.data(HideButtonRole).value<QList<int>>();
			int xOffset = 0;
			if (!hideIndexes.isEmpty()) {
				//计算隐藏的按钮的总宽度
				int totalW = 0;
				for (int i = 0; i < m_datas.size(); i++) {
					if (hideIndexes.contains(i)) {
						totalW += m_datas[i]->rect.width();
					}
				}
				//
				xOffset = totalW / 2;
			}

			m_index = -1;
			for (int i = 0; i < m_datas.size(); i++) {
				if (hideIndexes.contains(i)) {
					continue;
				}
				auto& d = m_datas[i];
				auto rect = d->rect;
				rect.moveLeft(option.rect.x() + d->rect.x() + xOffset);
				rect.moveTop(option.rect.y() + d->rect.y());
				//qDebug() << rect << ev->pos() << m_index;
				if (rect.contains(ev->pos())) {
					m_index = i;
					m_currentIndex = index;
					break;
				}
			}
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		auto ev = dynamic_cast<QMouseEvent*>(event);
		if (ev->button() == Qt::LeftButton) {
			if (m_index != -1) {
				auto d = m_datas.at(m_index);
				emit clicked(m_index, index, d.get());
			}
			m_index = -1;
		}
	}
	return false;
}

bool ButtonDelegate::eventFilter(QObject* editor, QEvent* event)
{
	if (event->type() == QEvent::Resize) {
		m_isInit = false;
		auto header_view = headerView();
		if (!header_view) {
			qWarning() << "Header View is nullptr,maybe parent is not View?";
			return false;
		}
		auto index = header_view->count() - 1;
		auto w = header_view->sectionSize(index);
		auto h = header_view->sizeHint().height();
		auto len = header_view->height();
		auto x = header_view->sectionPosition(index);
		QRect rect(x, header_view->offset(), w, len);
		updateButtons(rect);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		m_index = -1;
	}
	return false;
}

QHeaderView* ButtonDelegate::headerView() const
{
	auto treeView = dynamic_cast<QTreeView*>(parent());
	if (treeView) {
		return treeView->header();
	}
	auto tableView = dynamic_cast<QTableView*>(parent());
	if (tableView) {
		return tableView->horizontalHeader();
	}
	return nullptr;
}

void ButtonDelegate::updateButtons(const QRect& rect)const
{
	if (m_datas.isEmpty())
		return;
	QFontMetrics metrics(m_font);
	//计算总按钮的宽度+间距
	int totalW = 0;
	for (int i = 0; i < m_datas.size(); i++) {
		auto& d = m_datas[i];
		auto w = m_iconSize + metrics.horizontalAdvance(d->text) + m_margins.left() + m_margins.right();
		totalW += w;
		d->rect.setWidth(w);
		d->rect.setHeight(metrics.height() + m_margins.top() + m_margins.bottom());
	}
	totalW += (m_datas.size() - 1) * m_spacing;


	//求开始坐标
	auto x = (rect.width() - totalW) / 2;
	auto y = (rect.height() - m_datas.at(0)->rect.height()) / 2;
	totalW = 0;
	//求每个按钮的坐标偏移量
	for (int i = 0; i < m_datas.size(); i++) {
		auto& d = m_datas[i];
		d->rect.moveLeft(x + totalW);
		d->rect.moveTop(y);
		totalW += m_iconSize + metrics.horizontalAdvance(d->text) + m_margins.left() + m_margins.right() + m_spacing;
	}
}


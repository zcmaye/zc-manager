#include "SwitchDelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStandardItemModel>

SwitchDelegate::SwitchDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
	, m_size(45,23)
{}

SwitchDelegate::~SwitchDelegate()
{}

void SwitchDelegate::setSize(const QSize& size)
{
	m_size = size;
}

QSize SwitchDelegate::size() const
{
	return m_size;
}

void SwitchDelegate::setBindValue(const QVariant& onValue, const QVariant& offValue)
{
	m_onValue = onValue;
	m_offValue = offValue;
}

void SwitchDelegate::setStatus(const QModelIndex& index, bool status) {
	const_cast<QAbstractItemModel*>(index.model())->setData(index, status, SwitchRole);
}

bool SwitchDelegate::status(const QModelIndex& index)
{
	return index.data(SwitchRole).toBool();
}

void SwitchDelegate::rollback(const QModelIndex& index)
{
	auto status = index.data(SwitchRole).toBool();
	setStatus(index, !status);
}

void SwitchDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	painter->setRenderHint(QPainter::Antialiasing);

	bool status = false;
	auto data = index.data(SwitchRole);
	if (!data.isValid()) {
		auto model = const_cast<QAbstractItemModel*>(index.model());
		status = m_onValue == index.data(Qt::DisplayRole);
		model->setData(index, status, SwitchRole);
	}
	else {
		status = data.toBool();
	}


	int x = option.rect.x();
	int y = option.rect.y();
	int w = option.rect.width();
	int h = option.rect.height();

	//计算按钮矩形
	QRect btnRect(QPoint(x + (w - m_size.width()) / 2, y + (h - m_size.height()) / 2), m_size);

	//绘制按钮
	painter->setPen(Qt::NoPen);
	if(status)
		painter->setBrush(QColor("#1890ff"));
	else
		painter->setBrush(QColor("#dcdfe6"));
	painter->drawRoundedRect(btnRect, m_size.height() / 2, m_size.height() / 2);

	//绘制把手
	int handleSize = btnRect.height() / 2 - 2;
	painter->setBrush(Qt::white);
	if (status) {
		painter->drawEllipse(QPoint(btnRect.right() - handleSize - 2, btnRect.top() + handleSize + 2), handleSize, handleSize);
	}
	else {
		painter->drawEllipse(QPoint(btnRect.left() + handleSize + 2, btnRect.top() + handleSize + 2), handleSize, handleSize);
	}
}

bool SwitchDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	int x = option.rect.x();
	int y = option.rect.y();
	int w = option.rect.width();
	int h = option.rect.height();

	//计算按钮矩形
	QRect btnRect(QPoint(x + (w - m_size.width()) / 2, y + (h - m_size.height()) / 2), m_size);

	if (event->type() == QEvent::MouseButtonRelease) {
		auto ev = dynamic_cast<QMouseEvent*>(event);
		if (btnRect.contains(ev->pos())) {
			//获取原来的状态
			auto status = index.data(SwitchRole).toBool();
			//设置新的状态
			model->setData(index, !status, SwitchRole);
			//发送信号
			emit stateChanged(!status, index);
		}
	}

	return false;
}


#include "CheckBoxDelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStandardItemModel>

CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
	, m_size(16,16)
{}

CheckBoxDelegate::~CheckBoxDelegate()
{}

void CheckBoxDelegate::setCheckState(const QModelIndex& index, Qt::CheckState state) {
	const_cast<QAbstractItemModel*>(index.model())->setData(index, state, CheckStateRole);
}

Qt::CheckState CheckBoxDelegate::checkState(const QModelIndex& index)
{
	return index.data(CheckStateRole).value<Qt::CheckState>();
}
void CheckBoxDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing);

	auto state = Qt::Unchecked;
	auto data = index.data(CheckStateRole);
	if (!data.isValid()) {
		auto model = const_cast<QAbstractItemModel*>(index.model());
		model->setData(index, state, CheckStateRole);
	}
	else {
		state = data.value<Qt::CheckState>();
	}

	//计算按钮矩形
	QRect rect(QPoint(option.rect.center().x() - m_size.width() / 2, option.rect.center().y() - m_size.height() / 2), m_size);
	//绘制
	if (state == Qt::Unchecked) {
		painter->setPen(QColor("#dcdfe6"));		//灰色
		painter->drawRoundedRect(rect, 2, 2);
	}
	else if (state == Qt::Checked) {
		painter->setPen(Qt::NoPen);
		painter->setBrush(QColor("#1890ff"));	//蓝色
		painter->drawRoundedRect(rect, 2, 2);

		//绘制√
		painter->setPen(QPen(Qt::white, 2));
		QPoint points[3] = {
			QPoint(rect.left() + 3,rect.center().y()),
			QPoint(rect.center().x() - 1,rect.bottom() - 3),
			QPoint(rect.right() - 3,rect.top() + 3),
		};
		painter->drawPolyline(points, 3);
	}
	else if (state == Qt::PartiallyChecked) {
		painter->setPen(Qt::NoPen);
		painter->setBrush(QColor("#1890ff"));	//蓝色
		painter->drawRoundedRect(rect, 2, 2);

		//绘制 -
		painter->setPen(QPen(Qt::white, 2));
		QPoint points[2] = {
			QPoint(rect.left() + 3,rect.center().y()),
			QPoint(rect.right() -  3,rect.center().y())
		};
		painter->drawPolyline(points, 2);
	}
	painter->restore();
}

bool CheckBoxDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	//计算按钮矩形
	QRect rect(QPoint(option.rect.center().x() - m_size.width() / 2, option.rect.center().y() - m_size.height() / 2), m_size);
	rect.adjust(-3, -2, 3, 2);

	if (event->type() == QEvent::MouseButtonRelease) {
		auto ev = dynamic_cast<QMouseEvent*>(event);
		if (rect.contains(ev->pos())) {
			//获取原来的状态
			auto checkState = index.data(CheckStateRole).value<Qt::CheckState>();
			//设置新的状态
			auto newCheckState = checkState == Qt::Unchecked ? Qt::Checked : Qt::Unchecked;
			model->setData(index, newCheckState, CheckStateRole);
			//发送信号
			emit checkStateChanged(newCheckState, index);
			return true;
		}
	}
	return false;
}


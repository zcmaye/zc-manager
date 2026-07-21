#include "CheckBoxHeaderView.h"
#include <QPainter>
#include <QMouseEvent>

CheckBoxHeaderView::CheckBoxHeaderView(QWidget *parent)
	: QHeaderView(Qt::Orientation::Horizontal,parent)
	, m_size(16,16)
	, m_checkState(Qt::Unchecked)
	, m_checkBoxIndex(0)
{}

CheckBoxHeaderView::~CheckBoxHeaderView()
{}

void CheckBoxHeaderView::setCheckState(Qt::CheckState checkState)
{
	m_checkState = checkState;
	viewport()->update();
}

Qt::CheckState CheckBoxHeaderView::checkState() const
{
	return m_checkState;
}

void CheckBoxHeaderView::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
	painter->save();
	QHeaderView::paintSection(painter, rect, logicalIndex);
	painter->restore();
	painter->setRenderHint(QPainter::Antialiasing);

	if (m_checkBoxIndex == logicalIndex)
	{
		//计算按钮矩形
		QRect boxRect(QPoint(rect.center().x() - m_size.width() / 2, rect.center().y() - m_size.height() / 2), m_size);
		//绘制
		if (m_checkState == Qt::Unchecked) {
			painter->setPen(QColor("#dcdfe6"));		//灰色
			painter->drawRoundedRect(boxRect, 2, 2);
		}
		else if (m_checkState == Qt::Checked) {
			painter->setPen(Qt::NoPen);
			painter->setBrush(QColor("#1890ff"));	//蓝色
			painter->drawRoundedRect(boxRect, 2, 2);

			//绘制√
			painter->setPen(QPen(Qt::white, 2));
			QPoint points[3] = {
				QPoint(boxRect.left() + 3,boxRect.center().y()),
				QPoint(boxRect.center().x() - 1,boxRect.bottom() - 3),
				QPoint(boxRect.right() - 3,boxRect.top() + 3),
			};
			painter->drawPolyline(points, 3);
		}
		else if (m_checkState == Qt::PartiallyChecked) {
			painter->setPen(Qt::NoPen);
			painter->setBrush(QColor("#1890ff"));	//蓝色
			painter->drawRoundedRect(boxRect, 2, 2);

			//绘制 -
			painter->setPen(QPen(Qt::white, 2));
			QPoint points[2] = {
				QPoint(boxRect.left() + 3,boxRect.center().y()),
				QPoint(boxRect.right() - 3,boxRect.center().y())
			};
			painter->drawPolyline(points, 2);
		}
	}
}

void CheckBoxHeaderView::mousePressEvent(QMouseEvent* ev)
{
	int column =  logicalIndexAt(ev->pos());
	if (column == m_checkBoxIndex) {
		m_checkState = m_checkState == Qt::Checked ? Qt::Unchecked : Qt::Checked;
		emit checkStateChanged(m_checkState);
		viewport()->update();
	}
}


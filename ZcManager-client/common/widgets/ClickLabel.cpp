#include "ClickLabel.h"
#include <QMouseEvent>
#include <QPainter>

ClickLabel::ClickLabel(QWidget *parent)
	: QLabel(parent)
	, m_maskLayerEnabled(false)
	, m_hovered(false)
{
	setAttribute(Qt::WA_StyledBackground);
}

ClickLabel::~ClickLabel()
{}

void ClickLabel::click()
{
	emit clicked();
}

void ClickLabel::setMaskLayerEnabled(bool enable)
{
	m_maskLayerEnabled = enable;
	update();
}

bool ClickLabel::maskLayerEnabled() const
{
	return m_maskLayerEnabled;
}

void ClickLabel::setMaskLayerPixmap(const QPixmap& pix)
{
	m_maskLayerPixmap = pix;
}

QPixmap ClickLabel::maskLayerPixmap() const
{
	return m_maskLayerPixmap;
}

void ClickLabel::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		m_pressed = true;
	}
	QLabel::mousePressEvent(ev);
}

void ClickLabel::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		if (m_pressed) {
			emit clicked();
			m_pressed = false;
		}
	}
	QLabel::mouseReleaseEvent(ev);
}

void ClickLabel::enterEvent(QEnterEvent* ev)
{
	setCursor(QCursor(Qt::PointingHandCursor));
	m_hovered = true;
	update();
}

void ClickLabel::leaveEvent(QEvent* ev)
{
	setCursor(QCursor(Qt::ArrowCursor));
	m_hovered = false;
	update();
}

void ClickLabel::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	//绘制图片
	if (hasScaledContents()) {
		painter.drawPixmap(rect(), pixmap());
	}
	else {
		painter.drawPixmap(0,0, pixmap());
	}

	//绘制蒙层
	if (maskLayerEnabled() && m_hovered) {
		painter.setBrush(QColor(0, 0, 0, 120));
		painter.drawEllipse(rect());
		if (!m_maskLayerPixmap.isNull()) {
			int x = (width() - m_maskLayerPixmap.width()) / 2;
			int y = (height() - m_maskLayerPixmap.height()) / 2;
			painter.drawPixmap(x, y, m_maskLayerPixmap);
		}
	}
}

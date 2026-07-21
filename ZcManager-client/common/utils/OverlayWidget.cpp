#include "OverlayWidget.h"
#include <QResizeEvent>
#include <QPainter>

OverlayWidget::OverlayWidget(QWidget *parent)
	: QWidget(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_StyledBackground);
}

OverlayWidget::~OverlayWidget()
{

}

void OverlayWidget::setViewPort(QWidget * w)
{
	if (!w) {
		qWarning() << "w is nullptr!";
		return;
	}

	if (m_viewPort) {
		qWarning() << "view port is exists!";
	}

	m_viewPort = w;
	this->setParent(m_viewPort);
	this->hide();
	m_viewPort->installEventFilter(this);
}

QWidget* OverlayWidget::viewPort() const
{
	return m_viewPort;
}

void OverlayWidget::setBackgroundColor(const QColor& color)
{
	m_backgroundColor = color;
}

QColor OverlayWidget::backgroundColor() const
{
	return m_backgroundColor;
}

void OverlayWidget::popup(QWidget* w, PopPosition position)
{
	if (!m_viewPort) {
		qWarning() << "[OverlayWidget] view port is nullptr";
		return;
	}
	if (!w) {
		qWarning() << "[OverlayWidget] w is nullptr";
		return;
	}

	w->installEventFilter(this);
	w->setParent(this);

	m_popWidget = w;
	m_position = position;
	
	this->setFocus();
	this->show();
	w->show();
}

bool OverlayWidget::eventFilter(QObject* watched, QEvent* ev)
{
	if (watched == m_viewPort) {
		//根据视口的大小调整当前覆盖窗口的大小
		if (ev->type() == QEvent::Resize) {
			auto resizeEvent = dynamic_cast<QResizeEvent*>(ev);
			this->setGeometry(QRect(QPoint( 0, 0 ), resizeEvent->size()));
		}
		else if(ev->type() == QEvent::Close) {
			this->setParent(nullptr);
		}

	}
	else if (watched == m_popWidget)
	{
		if (!m_popWidget)
			return false;
		if (ev->type() == QEvent::Close || ev->type() == QEvent::Hide) {
			m_popWidget->installEventFilter(nullptr);
			m_popWidget->setParent(nullptr);
			m_popWidget = nullptr;
			this->hide();
		}
		else if (ev->type() == QEvent::Show) {
			onResize();
		}
	}
	return false;
}

void OverlayWidget::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	painter.fillRect(rect(), m_backgroundColor);
}

void OverlayWidget::resizeEvent(QResizeEvent* ev)
{
	onResize();
}

void OverlayWidget::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key_Escape) {
		this->hide();
	}
}


void OverlayWidget::onResize()
{
	if (!m_popWidget)
		return;

	switch (m_position)
	{
	case OverlayWidget::LeadingPosition:
		m_popWidget->setGeometry(0, 0, m_popWidget->width(), this->height());
		break;
	case OverlayWidget::MiddlePosition:
		m_popWidget->move(
			(width() - m_popWidget->width()) / 2,
			(height() - m_popWidget->height()) / 2
		);
		break;
	case OverlayWidget::TrilingPosition:
		m_popWidget->setGeometry(width() - m_popWidget->width(), 0, m_popWidget->width(), this->height());
		break;
	default:
		break;
	}

}


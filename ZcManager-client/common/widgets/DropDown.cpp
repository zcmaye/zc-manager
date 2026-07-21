#include "DropDown.h"
#include <QMouseEvent>
#include <QVBoxLayout>

DropDown::DropDown(QWidget *parent)
	: QLineEdit(parent)
	, m_container(new QWidget(this, Qt::Popup))
{
	m_container->setObjectName("container");
}

DropDown::~DropDown()
{}

QAction* DropDown::leadingAction()
{
	if (!m_leadingAct) {
		m_leadingAct = addAction(QIcon(":/Resource/icons/search-gray.svg"), QLineEdit::LeadingPosition);
	}
	return m_leadingAct;
}

QAction* DropDown::trailingAction()
{
	if (!m_trailingAct) {
		m_trailingAct= addAction(QIcon(":/Resource/icons/close.svg"), QLineEdit::TrailingPosition);
	}
	return m_trailingAct;
}

void DropDown::setTrailingActionClear(bool enable)
{
	auto act =  trailingAction();
	if (enable) {
		act->setIcon(QIcon(":/Resource/icons/close.svg"));
		connect(act, &QAction::triggered, this, &QLineEdit::clear);
	}
	else{
		removeAction(act);
	}
}

void DropDown::setWidget(QWidget* w)
{
	auto layout = m_container->layout();
	if (!layout) {
		layout = new QVBoxLayout(m_container);
	}

	if (!m_widget) {
		layout->addWidget(w);
	}
	else {
		layout->replaceWidget(m_widget, w);
	}
	m_widget = w;
}

QWidget* DropDown::widget() const
{
	return m_widget;
}

QWidget* DropDown::container() const
{
	return m_container;
}

void DropDown::setWidthSync(bool sync)
{
	m_widthSync = sync;
}

void DropDown::showPopup()
{
	if (!isPop()) {
		//把this的0,0位置转成全局坐标
		auto globalPos = this->mapToGlobal(QPoint(0, 0));
		m_container->move(
			globalPos.x(),
			globalPos.y() + height() + 6
		);
		m_container->show();
	}
}

void DropDown::hidePopup()
{
	if(isPop()) {
		m_container->hide();
	}
}

bool DropDown::isPop()
{
	return m_container->isVisible();
}

void DropDown::mousePressEvent(QMouseEvent* ev)
{
}

void DropDown::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		if(isPop())
			hidePopup();
		else
			showPopup();
	}
}

void DropDown::resizeEvent(QResizeEvent* ev)
{
	if (m_widthSync) {
		container()->setFixedWidth(width());
		if (m_widget) {
			m_widget->setFixedWidth(width() - container()->layout()->contentsMargins().left() * 2);
		}
	}
	emit sig_resize(this->size());
}

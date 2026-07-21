#include "HoverButton.h"

HoverButton::HoverButton(QWidget *parent)
	: QPushButton(parent)
{
	setAttribute(Qt::WA_StyledBackground);
}

HoverButton::~HoverButton()
{}

void HoverButton::enterEvent(QEnterEvent * ev)
{
	emit entered();
	setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
}

void HoverButton::leaveEvent(QEvent* ev)
{
	emit exited();
	setCursor(QCursor(Qt::CursorShape::ArrowCursor));
}


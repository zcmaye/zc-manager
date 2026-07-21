#pragma once

#include <QPushButton>

class HoverButton  : public QPushButton
{
	Q_OBJECT

public:
	HoverButton(QWidget *parent = nullptr);
	~HoverButton();

protected:
	void enterEvent(QEnterEvent* ev)override;
	void leaveEvent(QEvent* ev)override;
signals:
	void entered();
	void exited();
};


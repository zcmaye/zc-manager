#pragma once

#include "CalendarWidget.h"
#include <QLineEdit>

class DateEdit  : public QLineEdit
{
	Q_OBJECT

public:
	DateEdit(QWidget *parent);
	~DateEdit();

	QDate date()const;
	void setDate(QDate date);
	void clear();
signals:
	void dateChanged(QDate date);
protected:
	void mousePressEvent(QMouseEvent* ev)override;

	void showPopup();
	void hidePopup();
private:
	CalendarWidget* m_calendarWidget;
	QDate m_date;
};


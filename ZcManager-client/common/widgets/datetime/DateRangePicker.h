#pragma once

#include "DualCalendarWidget.h"
#include <QDate>
#include <QWidget>

class QLineEdit;

class DateRangePicker  : public QWidget
{
	Q_OBJECT

public:
	DateRangePicker(QWidget *parent = nullptr);
	~DateRangePicker();

	void setStartDate(QDate date);
	QDate startDate()const;

	void setEndDate(QDate date);
	QDate endDate()const;

	void clear();	
signals:
	void dateRangeChanged();
protected:
	bool eventFilter(QObject* watched, QEvent* ev)override;
	bool isPop();
	void showPopup();
	void hidePopup();
private:
	void initUis();
	DualCalendarWidget* m_calendarWidget;
	QDate m_startDate;
	QDate m_endDate;

	QLineEdit* m_startEdit;
	QLineEdit* m_endEdit;
	QPushButton* m_clearBtn;
};


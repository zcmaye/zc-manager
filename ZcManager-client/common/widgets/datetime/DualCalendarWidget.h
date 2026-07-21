#pragma once

#include <QWidget>
#include "ui_DualCalendarWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DualCalendarWidgetClass; };
QT_END_NAMESPACE

class DualCalendarWidget : public QWidget
{
	Q_OBJECT

public:
	DualCalendarWidget(QWidget *parent = nullptr,Qt::WindowFlags f = Qt::WindowFlags());
	~DualCalendarWidget();

	QDate startDate()const;
	QDate endDate()const;
	QDate leftCurrentDate()const;
	QDate rightCurrentDate()const;

public slots:
	void setCurrentPage(int year, int month);
	void setStartDate(QDate date);
	void setEndDate(QDate date);
	void showNextMonth();
	void showNextYear();
	void showPreviousMonth();
	void showPreviousYear();
	void showSelectedDate();
	void showToday();
signals:
	void clicked(QDate date);
	void currentPageChanged(int year, int month);
	void dateRangeChanged();
private:
	Ui::DualCalendarWidgetClass *ui;

	void initUis();
};


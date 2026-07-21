#pragma once

#include <QWidget>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class CalendarWidgetClass; };
QT_END_NAMESPACE

class CalendarWidget : public QWidget
{
    Q_OBJECT

public:
    CalendarWidget(QWidget *parent = nullptr,Qt::WindowFlags f = Qt::WindowFlags());
    ~CalendarWidget();

	QDate selectedDate()const;

public slots:
	void setCurrentPage(int year, int month);
	void setSelectedDate(QDate date);
	void showNextMonth();
	void showNextYear();
	void showPreviousMonth();
	void showPreviousYear();
	void showSelectedDate();
	void showToday();
signals:
	void clicked(QDate date);
	void currentPageChanged(int year, int month);
	void selectionChanged();
private:
	Ui::CalendarWidgetClass* ui;
	void initUis();
};


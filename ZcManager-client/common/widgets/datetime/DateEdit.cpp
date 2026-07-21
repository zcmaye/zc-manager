#include "DateEdit.h"
#include <QMouseEvent>

DateEdit::DateEdit(QWidget *parent)
	: QLineEdit(parent)
	, m_calendarWidget(nullptr)
{
	addAction(QIcon(":/Resource/icons/calendar.svg"), QLineEdit::LeadingPosition);
	setPlaceholderText("选择日期");
	setClearButtonEnabled(true);
	connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
		if (text.isNull()) {
			clear();
		}});
	connect(this, &QLineEdit::editingFinished, this, [this] {
		auto date = QDate::fromString(text(),Qt::ISODate);
		if (date.isValid()) {
			setDate(date);
		}
		});
}

DateEdit::~DateEdit()
{
}

QDate DateEdit::date() const
{
	return m_date;
}

void DateEdit::setDate(QDate date)
{
	if (m_date != date) {
		m_date = date;
		setText(date.toString("yyyy-MM-dd"));
		if (m_calendarWidget) {
			m_calendarWidget->setCurrentPage(m_date.year(), m_date.month());
			m_calendarWidget->setSelectedDate(m_date);
		}
		emit dateChanged(m_date);
	}
}

void DateEdit::clear()
{
	m_date = QDate();
	QLineEdit::clear();
}

void DateEdit::mousePressEvent(QMouseEvent * ev)
{
	if (ev->button() == Qt::LeftButton) {
		showPopup();
	}
}

void DateEdit::showPopup()
{
	if (!m_calendarWidget) {
		m_calendarWidget = new CalendarWidget(this,Qt::Popup);
		m_calendarWidget->setFixedSize(324, 343);
		connect(m_calendarWidget, &CalendarWidget::selectionChanged, this, [this] {
			this->setDate(m_calendarWidget->selectedDate());
			hidePopup();
			});
	}
	auto globalPos = this->mapToGlobal(QPoint(0,0));

	m_calendarWidget->move(
		globalPos.x(),
		globalPos.y() + height() + 10
	);
	m_calendarWidget->show();
}

void DateEdit::hidePopup()
{
	if (m_calendarWidget) {
		m_calendarWidget->hide();
	}
}


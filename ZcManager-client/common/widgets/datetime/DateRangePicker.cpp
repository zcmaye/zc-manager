#include "DateRangePicker.h"
#include <QMouseEvent>
#include <QLineEdit>

DateRangePicker::DateRangePicker(QWidget *parent)
	: QWidget(parent)
	, m_calendarWidget(nullptr)
{
	setAttribute(Qt::WA_StyledBackground);
	initUis();
}

DateRangePicker::~DateRangePicker()
{}

void DateRangePicker::setStartDate(QDate date)
{
	if (m_startDate != date) {
		m_startDate = date;
		if (m_calendarWidget) {
			m_calendarWidget->setStartDate(date);
			if(date.isValid())
				m_startEdit->setText(date.toString("yyyy-MM-dd"));
		}
	}
}

QDate DateRangePicker::startDate() const
{
	return m_startDate;
}

void DateRangePicker::setEndDate(QDate date)
{
	if (m_endDate != date) {
		m_endDate = date;
		if (m_calendarWidget) {
			m_calendarWidget->setEndDate(date);
			if (date.isValid())
				m_endEdit->setText(date.toString("yyyy-MM-dd"));
		}
	}
}

QDate DateRangePicker::endDate() const
{
	return m_endDate;
}

void DateRangePicker::clear()
{
	setStartDate(QDate());
	setEndDate(QDate());
	m_startEdit->clear();
	m_endEdit->clear();
}

bool DateRangePicker::eventFilter(QObject* watched, QEvent* ev)
{
	if (ev->type() == QEvent::MouseButtonPress) {
		if(!isPop())
			showPopup();
		setFocus();
	}
	else if(ev->type() == QEvent::Enter) {
		m_clearBtn->show();
	}
	else if (ev->type() == QEvent::Leave) {
		m_clearBtn->hide();
	}
	return false;
}

bool DateRangePicker::isPop() {
	return m_calendarWidget && m_calendarWidget->isVisible();
}


void DateRangePicker::showPopup()
{
	if (!m_calendarWidget) {
		m_calendarWidget = new DualCalendarWidget(this,Qt::Popup);
		m_calendarWidget->setFixedSize(323*2, 329);
		connect(m_calendarWidget, &DualCalendarWidget::dateRangeChanged, this, [this] {
			setStartDate(m_calendarWidget->startDate());
			setEndDate(m_calendarWidget->endDate());
			hidePopup();
			emit dateRangeChanged();
			});
	}
	auto globalPos = this->mapToGlobal(QPoint(0,0));

	m_calendarWidget->move(
		globalPos.x(),
		globalPos.y() + height() + 10
	);
	m_calendarWidget->show();
}

void DateRangePicker::hidePopup()
{
	if (m_calendarWidget) {
		m_calendarWidget->hide();
	}
}

void DateRangePicker::initUis()
{
	auto iconBtn = new QPushButton(QIcon(":/Resource/icons/calendar.svg"), {});
	m_startEdit = new QLineEdit;
	auto lab = new QLabel("至");
	m_endEdit = new QLineEdit;

	auto w = new QWidget;
	w->setFixedWidth(30);
	w->setAttribute(Qt::WA_StyledBackground, false);
	w->setStyleSheet("background-color:transparent;");
	auto blayout = new QVBoxLayout(w);
	blayout->setContentsMargins(0, 0, 0, 0);
	auto clearBtn = new QPushButton(QIcon(":/Resource/icons/clear.svg"), {});
	clearBtn->setObjectName("clearBtn");
	blayout->addWidget(clearBtn);

	auto hlayot = new QHBoxLayout(this);
	hlayot->setContentsMargins(9, 0, 9, 0);
	hlayot->addWidget(iconBtn);
	hlayot->addWidget(m_startEdit);
	hlayot->addWidget(lab);
	hlayot->addWidget(m_endEdit);
	hlayot->addWidget(w);

	m_startEdit->setPlaceholderText("开始日期");
	m_endEdit->setPlaceholderText("结束日期");

	lab->setFixedSize(26, 26);
	iconBtn->setFixedSize(26, 26);
	iconBtn->setStyleSheet("border:none;");
	clearBtn->setFixedSize(26, 26);
	clearBtn->setStyleSheet("border:none;");
	m_startEdit->setObjectName("startEdit");
	m_startEdit->setStyleSheet("border:none;background-color:transparent;font-size:13px;");
	m_startEdit->setAlignment(Qt::AlignCenter);
	//m_startEdit->setMaximumWidth(100);
	//m_startEdit->setMinimumWidth(30);

	m_endEdit->setObjectName("endEdit");
	m_endEdit->setStyleSheet("border:none;background-color:transparent;font-size:13px;");
	m_endEdit->setAlignment(Qt::AlignCenter);
	//m_endEdit->setMaximumWidth(100);
	//m_endEdit->setMinimumWidth(30);

	this->installEventFilter(this);
	iconBtn->installEventFilter(this);
	m_startEdit->installEventFilter(this);
	lab->installEventFilter(this);
	m_endEdit->installEventFilter(this);
	//clearBtn->installEventFilter(this);

	m_clearBtn = clearBtn;
	m_clearBtn->hide();
	connect(clearBtn, &QPushButton::clicked, this, [this] {
		clear();
		});
}

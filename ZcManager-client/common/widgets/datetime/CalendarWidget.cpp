#include "CalendarWidget.h"
#include "ui_CalendarWidget.h"

#include <QWidget>
#include <QDate>
#include <QPainter>
#include <QMouseEvent>

#define ViewPtr() findChild<CalendarView*>("calendar_view")

/** 日历视图 */
class CalendarView : public QWidget 
{
    Q_OBJECT
public:
    CalendarView(QWidget* parent = nullptr) 
        :QWidget(parent)
        ,m_currentDate(QDate::currentDate()) 
        ,m_hoverPos(-1,-1)
    {
        setMouseTracking(true);
    }

	QDate selectedDate()const { return m_selectedDate; }
	QDate currentDate()const { return m_currentDate; }
public slots:
    void setSelectedDate(QDate date) {
        if (m_selectedDate != date) {
			m_selectedDate = date;
            emit selectionChanged();
			update();
        }
    }
    void setCurrentPage(int year, int month) {
        if (m_currentDate.year() != year || m_currentDate.month() != month) {
			m_currentDate.setDate(year, month, 1);
            emit currentPageChanged(year, month);
			update();
        }
    }
	void showNextMonth() {
		auto dt = m_currentDate.addMonths(1);
		setCurrentPage(dt.year(), dt.month());
	}
	void showNextYear() {
		auto dt = m_currentDate.addYears(1);
		setCurrentPage(dt.year(), dt.month());
	}
	void showPreviousMonth() {
		auto dt = m_currentDate.addMonths(-1);
		setCurrentPage(dt.year(), dt.month());
	}
	void showPreviousYear()
	{
		auto dt = m_currentDate.addYears(-1);
		setCurrentPage(dt.year(), dt.month());
	}
	void showSelectedDate() {
		setCurrentPage(m_selectedDate.year(),m_selectedDate.month());
	}
	void showToday() {
		auto dt = QDate::currentDate();
		setCurrentPage(dt.year(), dt.month());
	}
signals:
    void clicked(QDate date);
    void currentPageChanged(int year, int month);
    void selectionChanged();
protected:
    void paintEvent(QPaintEvent* ev)override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //绘制背景
		drawBackground(&painter, QRect(0, 0, width(), height()));

        //绘制日历
		drawMonthCalendar(&painter, QRect(0, 0, width(), height()));
        
    }
    void mousePressEvent(QMouseEvent* ev)override
    {
        if (ev->button() == Qt::LeftButton) {
            auto date = dateAtPosition(ev->pos());
            if (date.isValid()) {
                setSelectedDate(date);
                emit clicked(date);
            }
			//测试切页
			if (date.year() != m_currentDate.year() || date.month() != m_currentDate.month()) {
				setCurrentPage(date.year(), date.month());
			}
        }
    }
    void mouseMoveEvent(QMouseEvent* ev)override {
        m_hoverPos = ev->pos();
        update();
    }
    void leaveEvent(QEvent* ev)override {
        m_hoverPos = QPoint(-1, -1);
        update();
    }
private:
    void drawBackground(QPainter* painter, const QRect& area)
    {
		//整体背景
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::white);
        painter->drawRoundedRect(area, 5, 5);

		//周和日期分割线
		painter->setPen(QColor("#ebeef5"));
		painter->drawLine(area.left(), area.top() + 40, area.right(), area.top() + 40);
    }
	void drawDateBackground(QPainter* painter, const QRect& area, QDate date)
	{
		painter->setPen(Qt::NoPen);
		if (date == m_selectedDate) {
			painter->setBrush(QColor("#409eff"));
			auto radius = qMin(area.width(), area.height()) / 2 - 4;
			painter->drawEllipse(area.center(), radius, radius);
		}
		else {
			painter->setBrush(Qt::white);
			painter->drawRoundedRect(area.adjusted(2, 2, -2, -2), 5, 5);
		}

	}
	void drawDateText(QPainter* painter, const QRect& area, QDate date)
	{
		painter->save();
		QFont font = painter->font();
		//如果不是本月
		if (date.month() != m_currentDate.month()) {
			painter->setPen(QColor("#c0c4cc"));
		}
		//当前选择
		else if (date == m_selectedDate) {
			painter->setPen(QColor("#ffffff"));
			font.setBold(true);
		}
		//本月
		else if (date == m_currentDate) {
			painter->setPen(QColor("#409eff"));
			font.setBold(true);
		}
		//鼠标悬停
		else if (area.contains(m_hoverPos)) {
			painter->setPen(QColor("#58aaff"));
		}
		//其他正常
		else {
			painter->setPen(QColor("#787a7d"));
		}
		painter->setFont(font);
		painter->drawText(area, Qt::AlignCenter, QString::number(date.day()));
		painter->restore();
	}
	void drawMonthCalendar(QPainter* painter, const QRect& area) {

		//绘制星期标题(水平头)
		QStringList weekDays = { "一","二","三","四","五","六","日" };
		int cellWidth = area.width() / 7;
		int cellHeight = (area.height() - 40) / 6;

		painter->setPen(QColor("#606266"));
		for (int i = 0; i < 7; i++) {
			QRect dayRect(area.left() + i * cellWidth, area.top(), cellWidth, 40);
			painter->drawText(dayRect, Qt::AlignCenter, weekDays[i]);
		}

		//计算当月第一天
		QDate firstDayOfMonth(m_currentDate.year(), m_currentDate.month(), 1);
		//获取星期 1~7 转成 0~6
		int firstDayOfWeek = firstDayOfMonth.dayOfWeek() - 1;
		firstDayOfWeek = firstDayOfWeek == 0 ? 7 : firstDayOfWeek;

		//绘制日期
		QDate date = firstDayOfMonth.addDays(-firstDayOfWeek);
		for (int row = 0; row < 6; row++) {
			for (int col = 0; col < 7; col++) {
				QRect cellRect(area.left() + col * cellWidth,
					area.top() + 40 + row * cellHeight,
					cellWidth, cellHeight);
				//绘制日期背景
				drawDateBackground(painter, cellRect,date);

				//绘制日期文本
				drawDateText(painter, cellRect, date);

				date = date.addDays(1);
			}
		}
	}

	QDate dateAtPosition(const QPoint& pos)
	{
		//日历区域
		QRect area(0, 40, width(), height());
		//如果不再日历区域，直接返回
		if (!area.contains(pos)) {
			return QDate();
		}
		//单元格大小
		int cellWidth = area.width() / 7;
		int cellHeight = (area.height() - 40) / 6;
		//把坐标转下标
		int row = (pos.y() - area.top()) / cellHeight;
		int col = (pos.x() - area.left()) / cellWidth;
		if (row < 0 || row >= 6 || col < 0 || col >= 7) {
			return QDate();
		}
		//获取下标处的日期
		//计算当月第一天
		QDate firstDayOfMonth(m_currentDate.year(), m_currentDate.month(), 1);
		//获取星期 1~7 转成 0~6
		int firstDayOfWeek = firstDayOfMonth.dayOfWeek() - 1;
		firstDayOfWeek = firstDayOfWeek == 0 ? 7 : firstDayOfWeek;

		return firstDayOfMonth.addDays(-firstDayOfWeek + row * 7 + col);
	}
private:
	QDate m_currentDate;        /*!当前显示的日期*/
	QDate m_selectedDate;       /*!当前选择的日期*/
	QPoint m_hoverPos;
};

CalendarWidget::CalendarWidget(QWidget *parent,Qt::WindowFlags f)
    : QWidget(parent,f)
    , ui(new Ui::CalendarWidgetClass())
{
    ui->setupUi(this);
	initUis();
}

CalendarWidget::~CalendarWidget()
{
    delete ui;
}

QDate CalendarWidget::selectedDate() const
{
	return ViewPtr()->selectedDate();
}

void CalendarWidget::setSelectedDate(QDate date)
{
	ViewPtr()->setSelectedDate(date);
}

void CalendarWidget::showNextMonth()
{
	ViewPtr()->showNextMonth();
}

void CalendarWidget::showNextYear()
{
	ViewPtr()->showNextYear();
}

void CalendarWidget::showPreviousMonth()
{
	ViewPtr()->showPreviousMonth();
}

void CalendarWidget::showPreviousYear()
{
	ViewPtr()->showPreviousYear();
}

void CalendarWidget::showSelectedDate()
{
	ViewPtr()->showSelectedDate();
}

void CalendarWidget::showToday()
{
	ViewPtr()->showToday();
}

void CalendarWidget::setCurrentPage(int year, int month)
{
	ViewPtr()->setCurrentPage(year,month);
}

void CalendarWidget::initUis()
{
	//设置日历视图
	auto view = new CalendarView;
	view->setObjectName("calendar_view");
	auto layout = ui->container->layout();
	if (layout) {
		auto item = layout->takeAt(layout->count() - 1);
		delete item;
		layout->addWidget(view);
	}
	ui->yearLab->setText(QString("%1 年").arg(view->currentDate().year()));
	ui->monthLab->setText(QString("%1 月").arg(view->currentDate().month()));

	connect(view, &CalendarView::selectionChanged, this,&CalendarWidget::selectionChanged);
	connect(view, &CalendarView::currentPageChanged, [this](int year, int month) {
			ui->yearLab->setText(QString("%1 年").arg(year));
			ui->monthLab->setText(QString("%1 月").arg(month));
			emit currentPageChanged(year, month);
		});
	connect(view, &CalendarView::clicked, this, &CalendarWidget::clicked);

	connect(ui->prevYearBtn, &QPushButton::clicked, view, &CalendarView::showPreviousYear);
	connect(ui->prevMonthBtn, &QPushButton::clicked, view, &CalendarView::showPreviousMonth);
	connect(ui->nextMonthBtn, &QPushButton::clicked, view, &CalendarView::showNextMonth);
	connect(ui->nextYearBtn, &QPushButton::clicked, view, &CalendarView::showNextYear);
}

#include "CalendarWidget.moc"

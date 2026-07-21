#include "DualCalendarWidget.h"
#include <QWidget>
#include <QDate>
#include <QPainter>
#include <QMouseEvent>

#define ViewPtr() findChild<DualCalendarView*>("calendar_view")

/** 日历视图 */
class DualCalendarView : public QWidget 
{
    Q_OBJECT
public:
    DualCalendarView(QWidget* parent = nullptr) 
        :QWidget(parent)
        ,m_leftCurrentDate(QDate::currentDate()) 
        ,m_rightCurrentDate(QDate::currentDate().addMonths(1)) 
        ,m_hoverPos(-1,-1)
    {
        setMouseTracking(true);
    }

	QDate startDate()const { return m_startDate; }
	QDate endDate()const { return m_endDate; }
	QDate leftCurrentDate()const { return m_leftCurrentDate; } 
	QDate rightCurrentDate()const { return m_rightCurrentDate; }
public slots:
    void setStartDate(QDate date) {
        if (m_startDate != date) {
			m_startDate = date;
			update();
        }
    }
    void setEndDate(QDate date) {
        if (m_endDate != date) {
			m_endDate = date;
			update();
        }
    }

    void setCurrentPage(int year, int month) {
        if (m_leftCurrentDate.year() != year || m_leftCurrentDate.month() != month) {
			m_leftCurrentDate.setDate(year, month, 1);
			m_rightCurrentDate = m_leftCurrentDate.addMonths(1);
            emit currentPageChanged(year, month);
			update();
        }
    }
	void showNextMonth() {
		auto dt = m_leftCurrentDate.addMonths(1);
		setCurrentPage(dt.year(), dt.month());
	}
	void showNextYear() {
		auto dt = m_leftCurrentDate.addYears(1);
		setCurrentPage(dt.year(), dt.month());
	}
	void showPreviousMonth() {
		auto dt = m_leftCurrentDate.addMonths(-1); 
		setCurrentPage(dt.year(), dt.month());
	}
	void showPreviousYear()
	{
		auto dt = m_leftCurrentDate.addYears(-1);
		setCurrentPage(dt.year(), dt.month());
	}
	void showSelectedDate() {
		setCurrentPage(m_startDate.year(),m_startDate.month());
	}
	void showToday() {
		auto dt = QDate::currentDate();
		setCurrentPage(dt.year(), dt.month());
	}
signals:
    void clicked(QDate date);
    void currentPageChanged(int year, int month);
	void dateRangeChanged();
protected:
    void paintEvent(QPaintEvent* ev)override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

		//绘制窗口背景
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::white);
        painter.drawRoundedRect(rect(), 5, 5);

		//左右日历分割线
		painter.setPen(QColor("#e4e4e4"));
		painter.drawLine(width() / 2, 0, width() / 2, height());


		auto leftArea = QRect(0, 0, width() / 2 - 10, height());
		auto rightArea = QRect(width() / 2 + 10, 0, width() / 2 - 10, height());

        //绘制背景
		drawBackground(&painter, leftArea);
		drawBackground(&painter, rightArea);

        //绘制左右日历
		drawMonthCalendar(&painter, leftArea,m_leftCurrentDate);
		drawMonthCalendar(&painter, rightArea,m_rightCurrentDate);
        
    }
    void mousePressEvent(QMouseEvent* ev)override
    {
        if (ev->button() == Qt::LeftButton) {
            auto date = dateAtPosition(ev->pos());
            if (date.isValid()) {
				//点击在已选择范围内，重置选择
				if (m_startDate.isValid() && m_endDate.isValid() && date >= m_startDate && date <= m_endDate) {
					m_startDate = date;
					m_endDate = date;
				}
				//开始日期无效
				else if (!m_startDate.isValid() || date < m_startDate) {
					m_startDate = date;
					if (m_endDate.isValid() && m_startDate > m_endDate) {
						m_endDate = m_startDate;
					}
				}
				//开始日期有效并且点击的日期大于等于开始选择的日期
				else {
					m_endDate = date;
					//如果结束日期小于开始日期，交换
					if (m_startDate > m_endDate) {
						auto temp = m_startDate;
						m_startDate = m_endDate;
						m_endDate = temp;
					}
				}
                emit clicked(date);
				update();
				if (m_startDate.isValid() && m_endDate.isValid()) {
					emit dateRangeChanged();
				}

				//测试切页
				//if (date.year() != m_leftCurrentDate.year() || date.month() != m_leftCurrentDate.month()) {
				//	setCurrentPage(date.year(), date.month());
				//}
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
		//周和日期分割线
		painter->setPen(QColor("#ebeef5"));
		painter->drawLine(area.left(), area.top() + 40, area.right(), area.top() + 40);
	}
	void drawDateRangeBackground(QPainter* painter, const QRect& area, QDate date,QDate startDate,QDate endDate)
	{
		//开始日期和结束日期样式
		if (date == startDate) {
			auto radius = qMin(area.width(), area.height()) / 2 - 8;
			//1
			painter->setBrush(QColor("#f2f6fc"));
			painter->drawRect(area.adjusted(area.width() / 2, 5, 0, -5));
			painter->drawEllipse(area.center(), radius + 4, radius + 3);

			//2
			painter->setBrush(QColor("#409eff"));
			painter->drawEllipse(area.center(), radius, radius);
		}
		else if (date == endDate) {
			auto radius = qMin(area.width(), area.height()) / 2 - 8;
			//1
			painter->setBrush(QColor("#f2f6fc"));
			painter->drawRect(area.adjusted(0, 5, -area.width() / 2, -5));
			painter->drawEllipse(area.center(), radius + 4, radius + 3);

			//2
			painter->setBrush(QColor("#409eff"));
			painter->drawEllipse(area.center(), radius, radius);

		}
		//中间日期样式
		else {
			painter->setBrush(QColor("#f2f6fc"));
			painter->drawRect(area.adjusted(0, 5, 0, -5));
		}
	}
	void drawDateBackground(QPainter* painter, const QRect& area, QDate date)
	{
		bool isCurrentMonth = (date.month() == m_leftCurrentDate.month() || date.month() == m_rightCurrentDate.month());
		auto clickDate = dateAtPosition(m_hoverPos);
		painter->setPen(Qt::NoPen);
		//高亮选择范围
		if (m_startDate != m_endDate && m_startDate.isValid() && m_endDate.isValid() && date >= m_startDate && date <= m_endDate) {
			drawDateRangeBackground(painter, area, date, m_startDate, m_endDate);
		}
		else if (date == m_startDate && clickDate == m_startDate) {
			auto radius = qMin(area.width(), area.height()) / 2 - 8;
			//1
			painter->setBrush(QColor("#f2f6fc"));
			painter->drawEllipse(area.center(), radius + 3, radius + 3);

			//2
			painter->setBrush(QColor("#409eff"));
			painter->drawEllipse(area.center(), radius, radius);
		}
		else if (date == m_endDate) {
			auto radius = qMin(area.width(), area.height()) / 2 - 8;
			//1
			painter->setBrush(QColor("#f2f6fc"));
			painter->drawEllipse(area.center(), radius + 3, radius + 3);

			//2
			painter->setBrush(QColor("#409eff"));
			painter->drawEllipse(area.center(), radius, radius);
		}
		else {
			if (clickDate.isValid() && m_startDate.isValid() && (date >= clickDate && date <= m_startDate || date >= m_startDate && date <= clickDate)) {
				//if (date >= m_startDate && date <= clickDate)
				if (clickDate >= m_startDate) {
					drawDateRangeBackground(painter, area, date, m_startDate, clickDate);
				}
				else {
					drawDateRangeBackground(painter, area, date, clickDate, m_startDate);
				}
			}
			else {
				painter->setBrush(Qt::white);
				painter->drawRoundedRect(area.adjusted(2, 2, -2, -2), 5, 5);
			}
		}

	}
	void drawDateText(QPainter* painter, const QRect& area, QDate date)
	{
		bool isCurrentMonth = (date.month() == m_leftCurrentDate.month() || date.month() == m_rightCurrentDate.month());

		painter->save();
		QFont font = painter->font();
		//高亮选择范围
		if (m_startDate.isValid() && m_endDate.isValid() && date >= m_startDate && date <= m_endDate) {
			//开始日期和结束日期样式
			if (date == m_startDate || date == m_endDate) {
				painter->setPen(QColor("#ffffff"));		//白色
				font.setBold(true);
			}
			//中间日期样式
			else {
				painter->setPen(QColor("#787a7d"));		//黑色
			}
		}
		else if (date == m_startDate || date == m_endDate) {
			painter->setPen(QColor("#ffffff"));		//白色
			font.setBold(true);
		}
		//鼠标悬停
		else if (area.contains(m_hoverPos)) {
			if (m_startDate.isValid()) {
				painter->setPen(QColor("#ffffff"));		//白色
				font.setBold(true);
			}
			else {
				painter->setPen(QColor("#58aaff"));		//蓝色
			}
		}
		//如果不是本月
		else if (!isCurrentMonth) {
			painter->setPen(QColor("#c0c4cc"));		//灰色
		}
		//其他正常
		else {
			painter->setPen(QColor("#787a7d"));		//黑色
		}
		painter->setFont(font);
		painter->drawText(area, Qt::AlignCenter, QString::number(date.day()));
		painter->restore();
	}
	void drawMonthCalendar(QPainter* painter, const QRect& area, QDate month) {

		//绘制星期标题(水平头)
		QStringList weekDays = { "一","二","三","四","五","六","日" };
		int cellWidth = area.width() / 7;
		int cellHeight = 38; // (area.height() - 40) / 6;

		painter->setPen(QColor("#606266"));
		for (int i = 0; i < 7; i++) {
			QRect dayRect(area.left() + i * cellWidth, area.top(), cellWidth, 40);
			painter->drawText(dayRect, Qt::AlignCenter, weekDays[i]);
		}

		//计算当月第一天
		QDate firstDayOfMonth(month.year(), month.month(), 1);
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
				drawDateBackground(painter, cellRect, date);

				//绘制日期文本
				drawDateText(painter, cellRect, date);

				date = date.addDays(1);
			}
		}
	}

	QDate dateAtPosition(const QPoint& pos)
	{
		auto leftArea = QRect(0, 40, width() / 2 - 10, height());
		auto rightArea = QRect(width() / 2 + 10, 40, width() / 2 - 10, height());

		//如果不再日历区域，直接返回
		if (!leftArea.contains(pos) && !rightArea.contains(pos)) {
			return QDate();
		}

		auto area = leftArea.contains(pos) ? leftArea : rightArea;
		auto month = leftArea.contains(pos) ? m_leftCurrentDate : m_rightCurrentDate;

		//单元格大小
		int cellWidth = area.width() / 7;
		int cellHeight = 38;// (area.height() - 40) / 6;
		//把坐标转下标
		int row = (pos.y() - area.top()) / cellHeight;
		int col = (pos.x() - area.left()) / cellWidth;
		if (row < 0 || row >= 6 || col < 0 || col >= 7) {
			return QDate();
		}
		//获取下标处的日期
		//计算当月第一天
		QDate firstDayOfMonth(month.year(), month.month(), 1);
		//获取星期 1~7 转成 0~6
		int firstDayOfWeek = firstDayOfMonth.dayOfWeek() - 1;
		firstDayOfWeek = firstDayOfWeek == 0 ? 7 : firstDayOfWeek;

		return firstDayOfMonth.addDays(-firstDayOfWeek + row * 7 + col);
	}
private:
	QDate m_leftCurrentDate;        /*!当前显示的日期*/
	QDate m_rightCurrentDate;        /*!当前显示的日期*/
	QDate m_startDate;			/*!当前选择开始的日期*/
	QDate m_endDate;			/*!当前选择结束的日期*/
	QPoint m_hoverPos;
};


DualCalendarWidget::DualCalendarWidget(QWidget *parent,Qt::WindowFlags f)
	: QWidget(parent,f)
	, ui(new Ui::DualCalendarWidgetClass())
{
	ui->setupUi(this);
	initUis();
}

DualCalendarWidget::~DualCalendarWidget()
{
	delete ui;
}

QDate DualCalendarWidget::startDate() const
{
	return ViewPtr()->startDate();
}

QDate DualCalendarWidget::endDate() const
{
	return ViewPtr()->endDate();
}

QDate DualCalendarWidget::leftCurrentDate() const
{
	return ViewPtr()->leftCurrentDate();
}

QDate DualCalendarWidget::rightCurrentDate() const
{
	return ViewPtr()->rightCurrentDate();
}

void DualCalendarWidget::setCurrentPage(int year, int month)
{
	ViewPtr()->setCurrentPage(year, month);
}

void DualCalendarWidget::setStartDate(QDate date)
{
	ViewPtr()->setStartDate(date);
}

void DualCalendarWidget::setEndDate(QDate date)
{
	ViewPtr()->setEndDate(date);
}

void DualCalendarWidget::showNextMonth()
{
	ViewPtr()->showNextMonth();
}

void DualCalendarWidget::showNextYear()
{
	ViewPtr()->showNextYear();
}

void DualCalendarWidget::showPreviousMonth()
{
	ViewPtr()->showPreviousMonth();
}

void DualCalendarWidget::showPreviousYear()
{
	ViewPtr()->showPreviousYear();
}

void DualCalendarWidget::showSelectedDate()
{
	ViewPtr()->showSelectedDate();
}

void DualCalendarWidget::showToday()
{
	ViewPtr()->showToday();
}


void DualCalendarWidget::initUis()
{
	//设置日历视图
	auto view = new DualCalendarView;
	view->setObjectName("calendar_view");
	auto layout = ui->container->layout();
	if (layout) {
		auto item = layout->takeAt(layout->count() - 1);
		delete item;
		layout->addWidget(view);
	}
	ui->yearLab->setText(QString("%1 年").arg(view->leftCurrentDate().year()));
	ui->monthLab->setText(QString("%1 月").arg(view->leftCurrentDate().month()));
	ui->yearLab_2->setText(QString("%1 年").arg(view->rightCurrentDate().year()));
	ui->monthLab_2->setText(QString("%1 月").arg(view->rightCurrentDate().month()));

	connect(view, &DualCalendarView::dateRangeChanged, this,&DualCalendarWidget::dateRangeChanged);
	connect(view, &DualCalendarView::currentPageChanged, [this,view](int year, int month) {
		ui->yearLab->setText(QString("%1 年").arg(view->leftCurrentDate().year()));
		ui->monthLab->setText(QString("%1 月").arg(view->leftCurrentDate().month()));
		ui->yearLab_2->setText(QString("%1 年").arg(view->rightCurrentDate().year()));
		ui->monthLab_2->setText(QString("%1 月").arg(view->rightCurrentDate().month()));
		emit currentPageChanged(year, month);
		});
	connect(view, &DualCalendarView::clicked, this, &DualCalendarWidget::clicked);

	connect(ui->prevYearBtn, &QPushButton::clicked, view, &DualCalendarView::showPreviousYear);
	connect(ui->prevMonthBtn, &QPushButton::clicked, view, &DualCalendarView::showPreviousMonth);
	connect(ui->nextMonthBtn, &QPushButton::clicked, view, &DualCalendarView::showNextMonth);
	connect(ui->nextYearBtn, &QPushButton::clicked, view, &DualCalendarView::showNextYear);

}

#include "DualCalendarWidget.moc"


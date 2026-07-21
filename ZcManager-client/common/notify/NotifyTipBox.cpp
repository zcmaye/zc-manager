#include "NotifyTipBox.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimerEvent>
#include <QCloseEvent>
#include <QTimer>

NotifyTipBox::NotifyTipBox(QWidget *parent)
	:NotifyTipBox(TypeError, "错误", parent)
{
}

NotifyTipBox::NotifyTipBox(TipType typ,const QString& msg, QWidget* parent)
	:NotifyTipBox(typ,msg,3000,parent)
{ 
}

NotifyTipBox::NotifyTipBox(TipType type, const QString& msg, qint32 delay, QWidget* parent)
	: QWidget(parent)
	, m_timerId(-1)
	, m_animation(new QPropertyAnimation(this, "pos", this))
{
	setFixedSize(418,53);
	setAttribute(Qt::WA_StyledBackground);

	if (PresetTipDatas.isEmpty()) {
		PresetTipDatas.append({ QPixmap(":/Resource/icons/tip-close.svg"), QString("错误"),3000
				, QColor(255, 237, 237)
				, QColor(255, 219, 219)
				, QColor(255, 98, 98) });
		PresetTipDatas.append({ QPixmap(":/Resource/icons/tip-success.svg"), QString("成功"),3000
				, QColor(119, 230, 66)
				, QColor(119, 230, 66)
				, QColor(255, 255, 255) });
		PresetTipDatas.append({ QPixmap(":/Resource/icons/tip-warning.svg"), QString("警告"),3000
				, QColor(251, 189, 62)
				, QColor(251, 189, 62)
				, QColor(250, 255, 255) });
	}

	setTipType(type);
	m_tipData.tipMsg = msg;
	m_tipData.holderDelay = delay;

	//开启定时器
	if (m_timerId == -1)
		m_timerId = startTimer(m_tipData.holderDelay);
}


NotifyTipBox::~NotifyTipBox()
{
	qDebug() << __FUNCSIG__;
}

void NotifyTipBox::setTipType(TipType type)
{
	 m_tipData = PresetTipDatas[type];
}

NotifyTipBox::TipData& NotifyTipBox::tipData()
{
	return m_tipData;
}

void NotifyTipBox::setTipData(const TipData& d)
{
	m_tipData = d;
}


void NotifyTipBox::animate(const QPoint& start, const QPoint& end, bool runStopOnDel)
{
	if (start == end) {
		return;
	}
	m_animation->stop();

	m_animation->setStartValue(start);
	m_animation->setEndValue(end);
	m_animation->setDuration(200);
	m_animation->start();


	//如果是结束动画
	if (runStopOnDel) {
		QTimer::singleShot(m_animation->duration(),this, [this] {
			emit del(this);
			});
	}

	//connect(m_animation, &QPropertyAnimation::finished, this, [this, runStopOnDel] {
	//		//如果是结束动画
	//		if (runStopOnDel) {
	//			emit del(this);
	//		}
	//	});
}

void NotifyTipBox::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	//背景
	painter.setBrush(m_tipData.backgroundColor);
	painter.setPen(m_tipData.borderColor);
	painter.drawRoundedRect(rect(), 5, 5);
	//图片
	painter.drawPixmap(QRect(25, 19, 16, 16), m_tipData.tipIcon);
	//文本
	auto font = painter.font();
	font.setPixelSize(14);
	font.setFamilies({ "微软雅黑" });
	font.setBold(true);
	painter.setFont(font);
	painter.setPen(m_tipData.foregroundColor);
	painter.drawText(QPoint(50, 32), m_tipData.tipMsg);
}

void NotifyTipBox::timerEvent(QTimerEvent* ev)
{
	if (ev->timerId() == m_timerId) {
		qDebug() << "timeout!" << this;
		emit disappear(this);
		m_timerId = -1;
	}
}

void NotifyTipBox::enterEvent(QEnterEvent* ev)
{
	killTimer(m_timerId);
	m_timerId = -1;
}

void NotifyTipBox::leaveEvent(QEvent* ev)
{
	//开启定时器
	if (m_timerId == -1)
		m_timerId = startTimer(m_tipData.holderDelay);
}

void NotifyTipBox::closeEvent(QCloseEvent* ev)
{
	//获取动画剩余时间
	auto t = m_animation->duration() - m_animation->currentTime();
	//当动画执行完200毫秒后自动消失
	QTimer::singleShot(t + 200, this, [this] {
		emit disappear(this);
		});
	ev->ignore();
}


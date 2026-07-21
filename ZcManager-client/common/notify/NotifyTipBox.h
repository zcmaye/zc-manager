#pragma once

#include <QWidget>

class QPropertyAnimation;

class NotifyTipBox  : public QWidget
{
	Q_OBJECT

public:
	enum TipType{
		TypeError,
		TypeSuccess,
		TypeWarning,
	};
	struct TipData {
		QPixmap tipIcon;
		QString tipMsg;
		qint32 holderDelay;			/*!驻留时长*/
		QColor backgroundColor;		/*!背景颜色*/
		QColor borderColor;			/*!边框颜色*/
		QColor foregroundColor;		/*!文本颜色*/

	};
public:
	NotifyTipBox(QWidget *parent = nullptr);
	NotifyTipBox(TipType type,const QString& msg,QWidget *parent = nullptr);
	NotifyTipBox(TipType type,const QString& msg,qint32 delay,QWidget *parent = nullptr);
	~NotifyTipBox();

	void setTipType(TipType type);

	TipData& tipData();
	void setTipData(const TipData& d);

	void animate(const QPoint& start, const QPoint& end,bool runStopOnDel = false);
protected:
	void paintEvent(QPaintEvent* ev)override;
	void timerEvent(QTimerEvent* ev) override;
	void enterEvent(QEnterEvent* ev)override;
	void leaveEvent(QEvent* ev)override;
	void closeEvent(QCloseEvent* ev)override;
signals:
	void disappear(NotifyTipBox* self);
	void del(NotifyTipBox* self);
private:
	int m_timerId;
	TipData m_tipData;			/*!当前提示数据*/
	QPropertyAnimation* m_animation{};
	inline static QList<TipData> PresetTipDatas;	/*!预设提示数据*/
};


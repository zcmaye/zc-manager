#pragma once

#include "NotifyTipBox.h"
#include "common/base/Singleton.hpp"
#include <QObject>

class NotifyTipManager  : public QObject,public Singleton<NotifyTipManager>
{
	Q_OBJECT
	friend class Singleton<NotifyTipManager>;
public:
	NotifyTipManager(QObject *parent = nullptr);
	~NotifyTipManager();

	inline static NotifyTipManager* instance() {
		static NotifyTipManager ins;
		return &ins;
	}

	/**
	 * 获取父窗口.
	 */
	void setViewPort(QWidget* w);
	QWidget* viewPort()const;

	/**
	 * 添加通知.
	 */
	NotifyTipBox* addNotifyTip(const QString& msg,NotifyTipBox::TipType typ = NotifyTipBox::TypeError);
	NotifyTipBox* addNotifyTip(const QString& msg, qint32 delay, NotifyTipBox::TipType typ = NotifyTipBox::TypeError);
	NotifyTipBox* addNotifyTip(NotifyTipBox* tip);

	/**
	 * 提示框停留时长ms.
	 */
	void setHolderDelay(qint32 ms);
	qint32 holderDelay()const;
protected:
	bool eventFilter(QObject* watched, QEvent* ev)override;
	void updateNotifyPosition();
public slots:
	void slot_newNofity(NotifyTipBox* tip, int index);
	void slot_disappear(NotifyTipBox* tip);
	void slot_del(NotifyTipBox* tip);
signals:
	void newNotify(NotifyTipBox* tip, int index);
private:
	QList<NotifyTipBox*> m_notifyTips;
	qint32 m_holderDelay;
	QSize  m_notifyTipSize;
	QPoint m_notifyTipPos;
	QPoint notifyPos(int index)const;
	NotifyTipBox* prevNotify(int index)const;
	int m_spacing;

	QWidget* m_viewPort{};
};


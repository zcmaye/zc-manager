#include "NotifyTipManager.h"
#include <QEvent>
#include <QResizeEvent>

NotifyTipManager::NotifyTipManager(QObject *parent)
	: QObject(parent)
	, m_holderDelay(3000)
	, m_notifyTipSize(418,53)
	, m_spacing(18)
{
	connect(this, &NotifyTipManager::newNotify, this, &NotifyTipManager::slot_newNofity);
	startTimer(500);
}

NotifyTipManager::~NotifyTipManager()
{
	//for (auto n : m_notifyTips) {
	//	delete n;
	//}
}

void NotifyTipManager::setViewPort(QWidget* w)
{
	if (!w) {
		return;
	}

	if (m_viewPort != w) {
		m_viewPort = w;
		m_viewPort->installEventFilter(this);
	}
}

QWidget* NotifyTipManager::viewPort() const
{
	return m_viewPort;
}

NotifyTipBox* NotifyTipManager::addNotifyTip(const QString& msg,NotifyTipBox::TipType typ)
{
	auto n = m_notifyTips.emplaceBack(new NotifyTipBox(typ,msg));
	emit newNotify(n, m_notifyTips.size() - 1);
	return n;
}

NotifyTipBox* NotifyTipManager::addNotifyTip(const QString& msg, qint32 delay, NotifyTipBox::TipType typ)
{
	auto n = m_notifyTips.emplaceBack(new NotifyTipBox(typ,msg,delay));
	emit newNotify(n, m_notifyTips.size() - 1);
	return n;
}

NotifyTipBox* NotifyTipManager::addNotifyTip(NotifyTipBox* tip)
{
	auto n = m_notifyTips.emplaceBack(tip);
	emit newNotify(n, m_notifyTips.size() - 1);
	return n;
}

void NotifyTipManager::setHolderDelay(qint32 ms)
{
	m_holderDelay = ms;
}

qint32 NotifyTipManager::holderDelay() const
{
	return m_holderDelay;
}

bool NotifyTipManager::eventFilter(QObject* watched, QEvent* ev)
{
	if (watched == m_viewPort) {
		//视口大小改变
		if (ev->type() == QEvent::Resize) {
			m_notifyTipPos.rx() = (m_viewPort->width() - m_notifyTipSize.width()) / 2;
			m_notifyTipPos.ry() = 18;//(m_viewPort->height() - m_notifyTipSize.height()) / 2;

			for (auto& n : m_notifyTips) {
				//n->animate(n->pos(), {m_notifyTipPos.x(),n->y()});
				n->move({m_notifyTipPos.x(),n->y()});
			}
		}
	}
	return false;
}

void NotifyTipManager::updateNotifyPosition() {
	//检测提示是否在自己应该待的位置，不在就移动
	for (int i = 0 ; i < m_notifyTips.size(); i++) {
		auto t = m_notifyTips[i];
		t->animate(t->pos(), notifyPos(i));
	}
}

void NotifyTipManager::slot_disappear(NotifyTipBox* tip)
{
	//把这个后面的所有的tip后往前挪动
	auto idx =  m_notifyTips.indexOf(tip);
	if (idx == -1) {
		qWarning() << "idx is -1";
		return;
	}


	//遍历次tip后面的所有的tip，运行动画
	//for (int i = idx + 1; i < m_notifyTips.size(); i++) {
	//	auto t = m_notifyTips[i];
	//	auto prevn = prevNotify(i);
	//	if (prevn) {
	//		t->animate(t->pos(), prevn->pos());
	//	}
	//	else {
	//		t->animate(t->pos(), QPoint(t->x(), -t->height()));
	//	}
	//}

	//for (int i = m_notifyTips.size() - 1; i > idx; i--) {
	//	auto t = m_notifyTips[i];
	//	auto prevn = prevNotify(i);
	//	if (prevn) {
	//		t->animate(t->pos(),notifyPos(i-1));
	//	}
	//	else {
	//		t->animate(t->pos(), QPoint(t->x(), -t->height()));
	//	}
	//}


	//移动自己
	auto prevn = prevNotify(idx);
	if (prevn) {
		tip->animate(tip->pos(), prevn->pos(),true);
	}
	else {
		tip->animate(tip->pos(), QPoint(tip->x(), -tip->height()), true);
	}

	//从列表中移除
	//m_notifyTips.removeOne(tip);

	//updateNotifyPosition();
}

void NotifyTipManager::slot_del(NotifyTipBox* tip)
{
	//从列表中移除
	m_notifyTips.removeOne(tip);

	//释放掉
	tip->deleteLater();

	//更新其他的提示的位置
	updateNotifyPosition();
}

QPoint NotifyTipManager::notifyPos(int index) const
{
	//如果index大于1，则跟随则放在最后一个的后面
	if (index >= 1) {
		return QPoint(
			m_notifyTipPos.x(),
			m_notifyTipPos.y() +
			(index * (m_spacing + m_notifyTipSize.height()))
		);
	}
	//否则放在第一个的位置
	else {
		return m_notifyTipPos;
	}
}

NotifyTipBox* NotifyTipManager::prevNotify(int index) const
{
	if (index < 1 || index >= m_notifyTips.size()) {
		qWarning() << "prevNotify:out of range!";
		return nullptr;
	}
	return m_notifyTips[index - 1];
}

void  NotifyTipManager::slot_newNofity(NotifyTipBox* tip, int index)
{
	if (!viewPort()) {
		qWarning() << "NotifyTipManager must set view port!";
		return;
	}

	updateNotifyPosition();

	connect(tip, &NotifyTipBox::disappear, this, &NotifyTipManager::slot_disappear);
	connect(tip, &NotifyTipBox::del, this, &NotifyTipManager::slot_del);

	qDebug() << tip << index;
	//设置父对象
	tip->setParent(m_viewPort);
	//设置坐标
	tip->move(notifyPos(index));
	//设置大小
	tip->setFixedSize(m_notifyTipSize);
	//设置驻留时长
	auto delay = tip->tipData().holderDelay;
	tip->tipData().holderDelay = delay == 3000 ? m_holderDelay : delay;

	//显示
	tip->show();
	//运行动画
	//auto n = prevNotify(index);
	//如果有上一个，移动到上一个的后面
	if (index) {
		//tip->animate(n->pos(), tip->pos());
		tip->animate(notifyPos(index-1), tip->pos());
	}
	else {
		tip->animate(QPoint(tip->x(), 0 - tip->height()),tip->pos());
	}
}

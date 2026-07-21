#pragma once

#include <QStyledItemDelegate>
#include <QVariant>

class SwitchDelegate  : public QStyledItemDelegate
{
	Q_OBJECT
public:
	inline static int SwitchRole = Qt::UserRole + 100;
public:
	SwitchDelegate(QObject *parent);
	~SwitchDelegate();

	void setSize(const QSize& size);
	QSize size()const;
	void setBindValue(const QVariant& onValue,const QVariant& offValue);

	void setStatus(const QModelIndex& index, bool status);
	bool status(const QModelIndex& index);
	void rollback(const QModelIndex& index);
signals:
	void stateChanged(bool state,const QModelIndex& index);
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;
private:
	QSize m_size;
	QVariant m_onValue;
	QVariant m_offValue;
};


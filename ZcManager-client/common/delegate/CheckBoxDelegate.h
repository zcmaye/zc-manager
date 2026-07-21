#pragma once

#include <QStyledItemDelegate>
#include <QVariant>

class CheckBoxDelegate  : public QStyledItemDelegate
{
	Q_OBJECT
public:
	inline static int CheckStateRole = Qt::UserRole + 101;
public:
	CheckBoxDelegate(QObject *parent);
	~CheckBoxDelegate();

	void setCheckState(const QModelIndex& index, Qt::CheckState checkState);
	Qt::CheckState checkState(const QModelIndex& index);
signals:
	void checkStateChanged(Qt::CheckState state, const QModelIndex& index);
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;
private:
	QSize m_size;
};


#pragma once

#include <QStyledItemDelegate>
#include <QModelIndex>

class NavTreeDelegate  : public QStyledItemDelegate
{
	Q_OBJECT

public:
	NavTreeDelegate(QObject *parent);
	~NavTreeDelegate();
protected:
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
	struct Data {
		QColor childBgColor{ 31, 45, 61 };		/*!子item背景颜色*/
		QColor childHoverColor{ 0, 21, 40 };	/*!子item悬停颜色*/
		QColor childFgColor{ 191, 203, 217 };		/*!子item文本颜色*/
		QColor childSelectFgColor{ 64, 158, 255 };	/*!子item选中后文本颜色*/

		QColor bgColor{ 48, 65, 86 };			/*!顶级item背景颜色*/
		QColor hoverColor{ 45, 61, 81 };		/*!顶级item悬停颜色*/
		QColor fgColor{ 191, 203, 217 };		/*!顶级item文本颜色*/
		QColor selectFgColor{ 244, 244, 245 };	/*!顶级item选中后文本颜色*/

		int iconSize{ 16 };
		int xOffset{ 44 };			/*!默认开始的位置*/
		int spacing{ 10 };			/*!图标和文本之间的间距*/
	}d;

	mutable QModelIndex m_currentIndex;
};


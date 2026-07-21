#pragma once

#include <QStyledItemDelegate>
#include <QList>

class QHeaderView;

class ButtonDelegate  : public QStyledItemDelegate
{
	Q_OBJECT
public:
	inline static int HideRowRole = Qt::UserRole + 102;		/*!bool true-隐藏 false-显示(默认)*/
	inline static int HideButtonRole = Qt::UserRole + 103;	/*!QList<int> 隐藏的按钮的索引*/
	struct Data {
		QPixmap icon;
		QString text;
		QColor forgeground;
		QColor pressedColor;
		QRect rect;
	};
public:
	ButtonDelegate(QObject *parent);
	~ButtonDelegate();

	void setSpacing(int spacing);
	void setIconSize(int size);
	void setFont(const QFont& font);
	void addButton(const Data& d);
	void addButton(const QPixmap& icon, const QString& text, const QColor& fg = QColor(0x2e99ff), const QColor& pressed = QColor(0x2b8be7));
	int buttonCount()const;
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;
	bool eventFilter(QObject* editor, QEvent* event) override;
signals:
	void clicked(int id, const QModelIndex& index, Data* buttonData);
private:
	mutable QList<std::shared_ptr<Data>> m_datas;
	int m_spacing;
	int m_iconSize;
	QFont  m_font;
	QMargins m_margins;
	mutable bool m_isInit;
	int m_index;
	QModelIndex m_currentIndex;

	QHeaderView* headerView()const;
	void updateButtons(const QRect& size)const;
};


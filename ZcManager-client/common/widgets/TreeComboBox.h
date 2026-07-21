#pragma once

#include <QComboBox>
#include <QTreeView>

class QStandardItem;

class TreeComboBox  : public QComboBox
{
	Q_OBJECT
public:
	TreeComboBox(QWidget *parent = nullptr);
	~TreeComboBox();

	void setView(QTreeView* view);
	QTreeView* view()const;

	QVariant currentData(int role = Qt::UserRole) const;
	QModelIndex currentIndex() const;
	void setCurrentIndex(const QModelIndex& index);

	QModelIndex findData(const QVariant& data, const std::function<bool(QVariant,QVariant)>& fun 
		= [](const QVariant& left, const QVariant& right) { return left == right; },
		int role = Qt::UserRole) const;
	QModelIndex findText(const QString& text, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive) const;
protected:
	bool eventFilter(QObject* watched, QEvent* ev)override;
	void showPopup()override;
	void hidePopup()override;
private:
	bool m_isAllowHidePopup;		/*!是否允许隐藏弹出框*/

	int hitHeight();
	int recursionHeight(QStandardItem* item);
};


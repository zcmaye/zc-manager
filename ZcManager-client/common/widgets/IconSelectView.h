#pragma once

#include <QWidget>

class QListView;
class QLineEdit;
class QStandardItemModel;
class QSortFilterProxyModel;
class QStandardItem;

class IconSelectView  : public QWidget
{
	Q_OBJECT

public:
	IconSelectView(QWidget *parent = nullptr);
	~IconSelectView();

	void addIcon(const QString& name, const QIcon& icon);
	void addIconPath(const QString& path);

	QVariant currentData(int role = Qt::UserRole) const;
	QModelIndex currentIndex() const;
	void setCurrentIndex(const QModelIndex& index);

	QModelIndex findData(const QVariant& data, const std::function<bool(QVariant,QVariant)>& fun 
		= [](const QVariant& left, const QVariant& right) { return left == right; },
		int role = Qt::UserRole) const;
	QModelIndex findText(const QString& text, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive) const;

public:
	void slot_clicked(const QModelIndex& index);
signals:
	void clicked(const QModelIndex& index);
	void iconChanged(const QString& name,const QIcon& icon);
private:
	QLineEdit* m_searchEdit;
	QListView* m_view;
	QAction* m_iconAct;
	QStandardItemModel* m_model;
	QSortFilterProxyModel* m_proxyModel;
};


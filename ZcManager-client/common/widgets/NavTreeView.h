#pragma once

#include "domain/vo/RouterVo.hpp"
#include <QTreeView>
#include <QStandardItemModel>

class NavTreeView  : public QTreeView
{
	Q_OBJECT

public:
	NavTreeView(QWidget *parent);
	~NavTreeView();

	/**
	 * 获取当前导航位置列表.
	 */
	QStringList navigationNameList()const;

	/**
	 * 选择导航菜单.
	 */
	void setCurrentNavigation(const QString& name);

	void test();

	void setRouters(const QList<std::shared_ptr<RouterVo>>& routers);
public slots:
	void slot_clicked(const QModelIndex& index);
signals:
	void navigationChanged(const QStringList& list);
private:
	QStandardItemModel* m_model{};

	void autoExpanded(const QModelIndex& index);
};


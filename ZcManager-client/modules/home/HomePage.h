#pragma once

#include <QWidget>
#include "ui_HomePage.h"
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class HomePageClass; };
QT_END_NAMESPACE

struct RouterVo;

class HomePage : public QWidget
{
	Q_OBJECT

public:
	HomePage(QWidget *parent = nullptr);
	~HomePage();


	void setRouters(const QList<std::shared_ptr<RouterVo>>& routers);
	void setCurrentPage(const QString& component, const QString& title);
protected:
	void showEvent(QShowEvent* ev)override;
	bool eventFilter(QObject* watched, QEvent* ev)override;
public slots:
	void slot_cut_page(const QModelIndex& index);
signals:
	void sig_quit_login();
private:
	Ui::HomePageClass *ui;

	template<typename T>
	inline QWidget* addPage(const QString& name);

	QMenu* m_personalMenu{};
	void initPersonalMenu();
};

template<typename T>
inline QWidget* HomePage::addPage(const QString& name)
{
	auto w = ui->tabWidget->findChild<QWidget*>(name);
	if (w) {
		return w;
	}
	else {
		qDebug() << name << "not found\n";
	}
	auto page = new T;
	page->setObjectName(name);
	//ui->stackedWidget->addWidget(page);
	ui->tabWidget->addTab(page,name);
	return page;
}


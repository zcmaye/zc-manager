#pragma once

#include <QWidget>
#include "ui_MainWindow.h"
#include <QPointer>
#include "common/network/HttpMgr.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class HomePage;

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void loadAvatar();
private:
	Ui::MainWindowClass *ui;
	QPointer<HomePage> m_homePage;

	HTTP_HANDLER_DECL(mainwindow);
};


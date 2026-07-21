#pragma once

#include <QWidget>
#include "ui_PostAddDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PostAddDlgClass; };
QT_END_NAMESPACE

class PostAddDlg : public QWidget
{
	Q_OBJECT

public:
	PostAddDlg(QWidget *parent = nullptr);
	~PostAddDlg();

	void clear();
public slots:
	void on_okBtn_clicked();
private:
	Ui::PostAddDlgClass *ui;
};


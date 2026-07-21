#pragma once

#include <QWidget>
#include "ui_DeptAddDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DeptAddDlgClass; };
QT_END_NAMESPACE

struct Dept;
struct TreeSelect;

class DeptAddDlg : public QWidget
{
	Q_OBJECT

public:
	DeptAddDlg(QWidget *parent = nullptr);
	~DeptAddDlg();

	void setDeptTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect);
	void setDept(const std::shared_ptr<Dept>& dept);
	void clear();
public slots:
	void on_okBtn_clicked();
private:
	Ui::DeptAddDlgClass *ui;
};


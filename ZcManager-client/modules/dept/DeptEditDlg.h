#pragma once

#include <QWidget>
#include "ui_DeptEditDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DeptEditDlgClass; };
QT_END_NAMESPACE

struct TreeSelect;
struct Dept;

class DeptEditDlg : public QWidget
{
	Q_OBJECT

public:
	DeptEditDlg(QWidget *parent = nullptr);
	~DeptEditDlg();

	void setDeptTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect);
	void setDept(const std::shared_ptr<Dept>& dept);
	void clear();
public slots:
	void on_okBtn_clicked();
private:
	Ui::DeptEditDlgClass *ui;
	std::shared_ptr<Dept> m_dept;
};


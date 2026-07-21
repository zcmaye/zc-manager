#pragma once

#include <QWidget>
#include "ui_MenuAddDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MenuAddDlgClass; };
QT_END_NAMESPACE

struct Menu;
struct TreeSelect;

class MenuAddDlg : public QWidget
{
	Q_OBJECT

public:
	MenuAddDlg(QWidget *parent = nullptr);
	~MenuAddDlg();

	void setMenuTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect);
	void setMenu(const std::shared_ptr<Menu>& menu);
	void clear();
public slots:
	void slot_cut_page(const QString& type);
	void on_okBtn_clicked();
private:
	Ui::MenuAddDlgClass *ui;

	void moveTo(QWidget* w,int row,int col);

	const char* getMenuType();
};


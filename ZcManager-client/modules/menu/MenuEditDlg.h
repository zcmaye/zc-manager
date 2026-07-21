#pragma once

#include <QWidget>
#include "ui_MenuEditDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MenuEditDlgClass; };
QT_END_NAMESPACE

struct Menu;
struct TreeSelect;

class MenuEditDlg : public QWidget
{
	Q_OBJECT

public:
	MenuEditDlg(QWidget *parent = nullptr);
	~MenuEditDlg();

	void setMenuTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect);
	void setMenu(const std::shared_ptr<Menu>& menu);
	void clear();
public slots:
	void slot_cut_page(const QString& type);
	void on_okBtn_clicked();
private:
	Ui::MenuEditDlgClass *ui;

	void moveTo(QWidget* w,int row,int col);

	const char* getMenuType();

	std::shared_ptr<Menu> m_menu;
};


#include "Home.h"
/*首页设计参考 https://www.woshipm.com/pd/4304980.html*/
Home::Home(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::HomeClass())
{
	ui->setupUi(this);
}

Home::~Home()
{
	delete ui;
}


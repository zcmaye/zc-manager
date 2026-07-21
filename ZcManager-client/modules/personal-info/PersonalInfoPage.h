#pragma once

#include <QWidget>
#include "ui_PersonalInfoPage.h"
#include "common/network/HttpMgr.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PersonalInfoPageClass; };
QT_END_NAMESPACE

class PersonalInfoPage : public QWidget
{
	Q_OBJECT

public:
	PersonalInfoPage(QWidget *parent = nullptr);
	~PersonalInfoPage();

protected:
	void showEvent(QShowEvent* ev)override;
private:
	Ui::PersonalInfoPageClass *ui;

	HTTP_HANDLER_DECL(personal_info);
};


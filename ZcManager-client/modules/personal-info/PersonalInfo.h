#pragma once

#include "common/widgets/AvatarChoose.h"
#include <QWidget>
#include "ui_PersonalInfo.h"
#include <QPointer>


QT_BEGIN_NAMESPACE
namespace Ui { class PersonalInfoClass; };
QT_END_NAMESPACE

struct User;

class PersonalInfo : public QWidget
{
	Q_OBJECT

public:
	PersonalInfo(QWidget *parent = nullptr);
	~PersonalInfo();

	void setUser(std::shared_ptr<User> user);
public slots:
	void on_avatarLab_clicked();
private:
	Ui::PersonalInfoClass *ui;
	AvatarChoose* m_avatarChoose{};
};


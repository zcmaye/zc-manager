#pragma once

#include <QWidget>
#include "ui_BaseProfile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BaseProfileClass; };
QT_END_NAMESPACE

struct User;

class BaseProfile : public QWidget
{
	Q_OBJECT

public:
	BaseProfile(QWidget *parent = nullptr);
	~BaseProfile();

	void setUser(std::shared_ptr<User> user);
	void updateUi();

	void clearPassword();
	void updateUser();
	void updatePassword();
public slots:
	void on_saveBtn_clicked();
	void on_cancelBtn_clicked();

private:
	Ui::BaseProfileClass *ui;
	std::shared_ptr<User> m_user;
};


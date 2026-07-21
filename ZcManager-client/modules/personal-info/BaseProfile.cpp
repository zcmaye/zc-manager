#include "BaseProfile.h"
#include "domain/entity/User.hpp"
#include "common/notify/NotifyTipManager.h"
#include "common/network/HttpMgr.h"

BaseProfile::BaseProfile(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::BaseProfileClass())
{
	ui->setupUi(this);
	setAttribute(Qt::WA_StyledBackground);

	connect(ui->baseProfileBtn, &QPushButton::clicked, this, [this] {
			ui->stackedWidget->setCurrentIndex(0);
		});
	connect(ui->alterPwdBtn, &QPushButton::clicked, this, [this] {
			ui->stackedWidget->setCurrentIndex(1);
		});
}

BaseProfile::~BaseProfile()
{
	delete ui;
}

void BaseProfile::setUser(std::shared_ptr<User> user)
{
	m_user = user;
	updateUi();
}

void BaseProfile::updateUi()
{
	ui->nickNameEdit->setText(m_user->nick_name);
	ui->phoneNumberEdit->setText(m_user->phone_number);
	ui->emailEdit->setText(m_user->email);

	if (m_user->sex == 0)
		ui->femaleRBtn->setChecked(true);
	else if (m_user->sex == 1)
		ui->maileRBtn->setChecked(true);
	else
		ui->unknownRBtn->setChecked(true);
}

void BaseProfile::clearPassword()
{
	ui->oldPwdEdit->clear();
	ui->newPwdEdit->clear();
	ui->cofirmPwdEdit->clear();
}

void BaseProfile::updateUser()
{
	auto nick_name = ui->nickNameEdit->text().trimmed();
	auto phone_number = ui->phoneNumberEdit->text().trimmed();
	auto email = ui->emailEdit->text().trimmed();
	auto sex = ui->femaleRBtn->isChecked() ? 0 : (ui->maileRBtn->isChecked() ? 1 : 2);

	m_user->nick_name = nick_name;
	m_user->phone_number = phone_number;
	m_user->email = email;
	m_user->sex = sex;
}

void BaseProfile::updatePassword()
{
	m_user->password = ui->newPwdEdit->text().trimmed();
	clearPassword();
}

void BaseProfile::on_saveBtn_clicked()
{
	QJsonObject juser;

	juser["user_id"] = m_user->user_id;

	//修改基本资料
	if (ui->baseProfileBtn->isChecked()) {
		juser["nick_name"] = ui->nickNameEdit->text().trimmed();
		juser["phone_number"]= ui->phoneNumberEdit->text().trimmed();
		juser["email"] =  ui->emailEdit->text().trimmed();
		juser["sex"] = ui->femaleRBtn->isChecked() ? 0 : (ui->maileRBtn->isChecked() ? 1 : 2);

		//发送修改资料请求
		HttpMgr::instance()->put(URL("/profile"), juser, ReqId::profile, Modules::PersonalInfo);
	}
	//修改密码
	else {
		auto oldPwd = ui->oldPwdEdit->text().trimmed();
		auto newPwd = ui->newPwdEdit->text().trimmed();
		auto confirmPwd = ui->cofirmPwdEdit->text().trimmed();

		if (newPwd != confirmPwd) {
			NotifyTipManager::instance()->addNotifyTip("确认密码和新密码不一致~");
			return;
		}

		juser["newPassword"] = ui->newPwdEdit->text().trimmed();
		juser["oldPassword"] = ui->oldPwdEdit->text().trimmed();


		HttpMgr::instance()->put(URL("/profile/updatePwd"), juser, ReqId::profile_reset_pwd, Modules::PersonalInfo);

		//m_user->password= newUser->password;
		//clearPassword();
	}
}

void BaseProfile::on_cancelBtn_clicked()
{
	if (ui->baseProfileBtn->isChecked())
		updateUi();
	else
		clearPassword();
}


#include "PersonalInfoPage.h"
#include "common/utils/Utils.h"
#include "ContextHolder.h"
#include "common/notify/NotifyTipManager.h"
#include "common/base/GlobalSignalHub.hpp"
#include "domain/entity/User.hpp"

PersonalInfoPage::PersonalInfoPage(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PersonalInfoPageClass())
{
	ui->setupUi(this);
	setAttribute(Qt::WA_StyledBackground);

	Utils::SetDropShadow(ui->baseProfile);
	Utils::SetDropShadow(ui->personalInfo);
	qDebug() << __FUNCTION__;


	HTTP_HANDLER_INIT(personal_info);
}

PersonalInfoPage::~PersonalInfoPage()
{
	delete ui;
}

void PersonalInfoPage::showEvent(QShowEvent* ev)
{
	ui->personalInfo->setUser(ContextHolder::instance()->self());
	ui->baseProfile->setUser(ContextHolder::instance()->self());
}

HTTP_HANDLER_IMPL(PersonalInfoPage, personal_info)
{
	if (errc != ErrorCode::Success) {
		NotifyTipManager::instance()->addNotifyTip(msg, NotifyTipBox::TypeError);
		return;	
	}

	try
	{
		auto json = json_t::parse(msg);
		if (json["code"] != 200) {
			NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
			return;
		}
		_handlers[req_id](json);
	}
	catch (const std::exception& e) {
		NotifyTipManager::instance()->addNotifyTip(e.what(), NotifyTipBox::TypeError);
	}
}

void PersonalInfoPage::initHandlers()
{
	HTTP_HANDER_INSERT(ReqId::profile) {
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		ui->baseProfile->updateUser();
	});

	HTTP_HANDER_INSERT(ReqId::profile_reset_pwd) {
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		ui->baseProfile->updatePassword();
	});

	HTTP_HANDER_INSERT(ReqId::profile_avatar_upload) {
		NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeSuccess);
		//获取缓存的头像
		auto pix = ContextHolder::instance()->self()->_tmp_pixmap;
		//发送信号
		emit GlobalSignalHub::instance()->avatarChanged(pix);
		//将头像保存到本地
		pix.save(Config::instance()->avatarPath() + "/" + ContextHolder::instance()->self()->user_name + ".png", "png");
	});
}

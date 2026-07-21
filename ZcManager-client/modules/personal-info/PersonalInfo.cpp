#include "PersonalInfo.h"
#include "domain/entity/User.hpp"
#include "common/utils/OverlayWidget.h"
#include "common/notify/NotifyTipManager.h"
#include "ContextHolder.h"
#include <QBuffer>
#include "common/network/HttpMgr.h"
#include "common/base/GlobalSignalHub.hpp"

PersonalInfo::PersonalInfo(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PersonalInfoClass())
{
	ui->setupUi(this);
	setAttribute(Qt::WA_StyledBackground);
	ui->avatarLab->setMaskLayerEnabled(true);
	ui->avatarLab->setMaskLayerPixmap(QPixmap(":/Resource/icons/plus.svg"));

	connect(GlobalSignalHub::instance().get(), &GlobalSignalHub::avatarChanged, this, [this](QPixmap pix) {
		ui->avatarLab->setPixmap(Utils::toRoundPixamp(pix));
		m_avatarChoose->hide();
		});
}

PersonalInfo::~PersonalInfo()
{
	delete ui;
}

void PersonalInfo::setUser(std::shared_ptr<User> user)
{
	//QPixmap pix(":/Resource/images/avatar.jpg");
	//ui->avatarlab->setPixmap(Utils::toRoundPixamp(pix));
	ui->avatarLab->setPixmap(user->avatarPixmap());
	ui->userNameLab->setText(user->user_name);
	ui->phoneNumberLab->setText(user->phone_number);
	ui->emailLab->setText(user->email);
	//ui->majorLab->setText();
	ui->birthdayLab->setText(user->birthday);
	ui->createTimeLab->setText(user->create_time);
}

void PersonalInfo::on_avatarLab_clicked()
{
	qDebug() << __FUNCTION__;
	if (!m_avatarChoose) {
		m_avatarChoose = new AvatarChoose;
		auto user = ContextHolder::instance()->self();
		if(user) {
			m_avatarChoose->setPixmap(user->avatarPixmap(true));
		}
		connect(m_avatarChoose, &AvatarChoose::sig_avatar, this, [this,user](QPixmap pix) {
			//上传头像
			QBuffer buffer;
			//把头像保存到buffer中
			if (!pix.save(&buffer, "png")) {
				qWarning() << "avatar save failed";
				return;
			}
			HttpMgr::instance()->upload(URL("/profile/avatar"), ReqId::profile_avatar_upload, buffer.data(), Modules::PersonalInfo);
			//缓存头像
			ContextHolder::instance()->self()->_tmp_pixmap = pix;
			});
	}

	OverlayWidget::instance()->popup(m_avatarChoose);
}




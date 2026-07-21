#include "LoginPage.h"
#include "common/notify/NotifyTipManager.h"
#include "common/widgets/MessageBox.h"
#include "common/utils/OverlayWidget.h"
#include "common/base/Defer.hpp"
#include "common/config/Config.h"
#include "ContextHolder.h"

#include "domain/JsonMapper.h"
#include "domain/entity/User.hpp"
#include "domain/vo/RouterVo.hpp"

#include <QDate>
#include <QTimer>
#include <QPainter>

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginPageClass())
    , m_copyright(QString("Copyright © 2024-%1 hdy All Rights Reserved. 湘ICP备180468xx号").arg(QDate::currentDate().year()))
    , m_regEnabled(true)
{
    ui->setupUi(this);
	setAttribute(Qt::WA_StyledBackground);


    ui->userNameEdit->addAction(QIcon(":/Resource/icons/user.svg"), QLineEdit::ActionPosition::LeadingPosition);
    ui->pwdEdit->addAction(QIcon(":/Resource/icons/pwd.svg"), QLineEdit::ActionPosition::LeadingPosition);
    ui->captchaEdit->addAction(QIcon(":/Resource/icons/captcha.svg"), QLineEdit::ActionPosition::LeadingPosition);

    ui->reg_userNameEdit->addAction(QIcon(":/Resource/icons/user.svg"), QLineEdit::ActionPosition::LeadingPosition);
    ui->reg_pwdEdit->addAction(QIcon(":/Resource/icons/pwd.svg"), QLineEdit::ActionPosition::LeadingPosition);
    ui->reg_confirmPwdEdit->addAction(QIcon(":/Resource/icons/pwd.svg"), QLineEdit::ActionPosition::LeadingPosition);
    ui->reg_captchaEdit->addAction(QIcon(":/Resource/icons/captcha.svg"), QLineEdit::ActionPosition::LeadingPosition);

    connect(ui->captchaLab, &ClickLabel::clicked, this, [this] {
		m_loginCaptcha =  m_maker.createText();
        auto pix =  m_maker.createImage(m_loginCaptcha.first);
        ui->captchaLab->setPixmap(pix);
        });

    connect(ui->reg_captchaLab, &ClickLabel::clicked, this, [this] {
		m_regCaptcha =  m_maker.createText();
        auto pix =  m_maker.createImage(m_regCaptcha.first);
        ui->reg_captchaLab->setPixmap(pix);
        });

    connect(ui->goRegBtn, &QPushButton::clicked, this, [this] {
        setCurrentWidget(ui->regWdgt);
        });

    connect(ui->goLoginBtn, &QPushButton::clicked, this, [this] {
        setCurrentWidget(ui->loginWdgt);
        });
    setCurrentWidget(ui->loginWdgt);

    ui->captchaLab->click();
    ui->reg_captchaLab->click();


    auto config = Config::instance();
    ui->userNameEdit->setText(config->value("account/userName").toString());
    ui->pwdEdit->setText(config->value("account/pwd").toString());
    ui->rememberPwdChx->setChecked(config->value("account/rememberpwd").toBool());


	//网络请求初始化
    HTTP_HANDLER_INIT(login_reg);
}

LoginPage::~LoginPage()
{
    delete ui;
}

void LoginPage::setCurrentWidget(QWidget* w)
{
    if (w == ui->loginWdgt) {
        ui->regWdgt->hide();
        ui->loginWdgt->show();
    }
    else if(w == ui->regWdgt) {
        ui->loginWdgt->hide();
        ui->regWdgt->show();
    }
}

void LoginPage::setRegisterEnabled(bool enable)
{
    if (m_regEnabled != enable) {
		m_regEnabled = enable;
        ui->goRegWdgt->setHidden(!enable);
    }
}

bool LoginPage::registerEnabled() const
{
    return m_regEnabled;
}

void LoginPage::clear()
{
	ui->captchaEdit->clear();
	ui->reg_captchaEdit->clear();
	ui->reg_userNameEdit->clear();
	ui->reg_pwdEdit->clear();
	ui->reg_confirmPwdEdit->clear();
}

void LoginPage::saveConfig()
{
	clear();
    auto userName = ui->userNameEdit->text().trimmed();
    auto pwd = ui->pwdEdit->text().trimmed();

    //保存用户名和密码
    auto config = Config::instance();
    config->setValue("account/userName", userName);
	if (ui->rememberPwdChx->isChecked()) {
		config->setValue("account/pwd", pwd);
	}
	else {
		//config->setValue("account/pwd", "");
		config->remove("account/pwd");
	}
	config->setValue("account/rememberPwd", ui->rememberPwdChx->isChecked());
}

void LoginPage::paintEvent(QPaintEvent* ev)
{
    QPainter painter(this);
    //背景图片
    painter.drawPixmap(rect(), QPixmap(":/Resource/images/login-background.jpg"));
    //版权信息
    painter.setPen(Qt::GlobalColor::white);
    auto tw = painter.fontMetrics().horizontalAdvance(m_copyright);
	painter.drawText(QPoint((width() - tw) / 2, height() - 30), m_copyright);
}

void LoginPage::resizeEvent(QResizeEvent* ev)
{
    //登录
    int x = (width() - ui->loginWdgt->width())/2;
    int y = (height() - ui->loginWdgt->height())/2;
    ui->loginWdgt->move(x,y);

    //注册
    if (registerEnabled()) {
        x = (width() - ui->regWdgt->width()) / 2;
        y = (height() - ui->regWdgt->height()) / 2;
        ui->regWdgt->move(x, y);
    }
}

void LoginPage::on_loginBtn_clicked()
{
    //static int i = 100;
	//NotifyTipManager::instance()->addNotifyTip(QString("正在登录中...%1").arg(i), NotifyTipBox::TipType(i % 3));
    //i++;

    auto userName = ui->userNameEdit->text().trimmed();
    auto pwd = ui->pwdEdit->text().trimmed();
    auto captcha = ui->captchaEdit->text().trimmed();

    if (userName.isEmpty()) {
	    NotifyTipManager::instance()->addNotifyTip("用户名不能为空", NotifyTipBox::TypeError);
        return;
    }

	if (pwd.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("密码不能为空", NotifyTipBox::TypeError);
		return;
	}
	if (captcha.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("验证码不能为空", NotifyTipBox::TypeError);
		return;
	}

    if (captcha != m_loginCaptcha.second) {
		NotifyTipManager::instance()->addNotifyTip("验证码不正确", NotifyTipBox::TypeError);
		return;
    }

	auto ing = NotifyTipManager::instance()->addNotifyTip("登录中...",99999999, NotifyTipBox::TypeSuccess);
    Defer defer([ing] {
         if(ing)ing->close();
        });

	QJson body{
		{"username",userName},
		{"password",pwd}
	};


	HttpMgr::instance()->post(URL("/login"), body, ReqId::Login, Modules::LoginReg);
}

void LoginPage::on_reg_regBtn_clicked()
{
	auto userName = ui->reg_userNameEdit->text().trimmed();
	auto pwd = ui->reg_pwdEdit->text().trimmed();
	auto confirmPwd = ui->reg_confirmPwdEdit->text().trimmed();
	auto captcha = ui->reg_captchaEdit->text().trimmed();

	if (userName.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("用户名不能为空", NotifyTipBox::TypeError);
		return;
	}

	if (pwd.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("密码不能为空", NotifyTipBox::TypeError);
		return;
	}

	if (confirmPwd.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("确认密码不能为空", NotifyTipBox::TypeError);
		return;
	}

	if (pwd != confirmPwd) {
		NotifyTipManager::instance()->addNotifyTip("确认密码和密码不一致", NotifyTipBox::TypeError);
		return;
	}

	if (captcha.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("验证码不能为空", NotifyTipBox::TypeError);
		return;
	}

	if (captcha != m_regCaptcha.second) {
		NotifyTipManager::instance()->addNotifyTip("验证码不正确", NotifyTipBox::TypeError);
		return;
	}

	auto ing = NotifyTipManager::instance()->addNotifyTip("注册中...", 99999999, NotifyTipBox::TypeSuccess);
	Defer defer([ing] {
		if (ing) ing->close();
		});

	QJson body{
		{"username",userName},
		{"password",pwd}
	};

	HttpMgr::instance()->post(URL("/register"), body, ReqId::Register, Modules::LoginReg);


}

//实现模块处理
HTTP_HANDLER_IMPL(LoginPage,login_reg) {
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

void LoginPage::initHandlers()
{
	HTTP_HANDER_INSERT(ReqId::Login) {
		//缓存token到本地
		auto token = json["token"].toString();
		ContextHolder::instance()->setToken(token.toUtf8());

		//获取用户信息
		HttpMgr::instance()->get(URL("/getInfo"), ReqId::GetInfo, Modules::LoginReg);

		//登录成功提示
		NotifyTipManager::instance()->addNotifyTip("登录成功~", NotifyTipBox::TypeSuccess);
		NotifyTipManager::instance()->addNotifyTip("正在获取用户信息~", NotifyTipBox::TypeSuccess);
	});

	HTTP_HANDER_INSERT(ReqId::GetInfo) {

		//缓存用户信息
		ContextHolder::instance()->setSelf(QJson(json["user"]));
		//缓存用户对应的所有菜单的权限字符
		ContextHolder::instance()->setPermission(QJson(json["perms"]));

		//获取路由信息
		HttpMgr::instance()->get(URL("/getRouters"), ReqId::GetRouters, Modules::LoginReg);

		//成功提示
		NotifyTipManager::instance()->addNotifyTip("获取用户信息成功~", NotifyTipBox::TypeSuccess);
	});

	HTTP_HANDER_INSERT(ReqId::GetRouters) {

		//缓存路由信息
		ContextHolder::instance()->setRouters(QJson(json["data"]));

		//成功提示
		NotifyTipManager::instance()->addNotifyTip("获取路由成功~", NotifyTipBox::TypeSuccess);

		saveConfig();
		//发射信号
		emit sig_login_finished();
	});

	HTTP_HANDER_INSERT(ReqId::Register) {
		//成功提示
		NotifyTipManager::instance()->addNotifyTip("注册成功~", NotifyTipBox::TypeSuccess);

		static MessageBox* tipBox{};
		if (!tipBox) {
			tipBox = new MessageBox(MessageBox::Success);
			tipBox->setMessage(QString("<font color='red'>恭喜你,您的账号 %1 注册成功！</font>").arg(ui->reg_userNameEdit->text()));
			tipBox->setCancelButtonHidden(true);
			connect(tipBox, &MessageBox::closed, this, [](int ret) {
				qDebug() << "reg success ";
				});
		}
		OverlayWidget::instance()->popup(tipBox);
		clear();
	});

}

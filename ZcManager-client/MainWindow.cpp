#include "MainWindow.h"
#include "common/notify/NotifyTipManager.h"
#include "common/utils/OverlayWidget.h"
#include "common/base/GlobalSignalHub.hpp"
#include "modules/home/HomePage.h"
#include "domain/entity/User.hpp"
#include "ContextHolder.h"

#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::MainWindowClass())
{
	ui->setupUi(this);
	setAttribute(Qt::WA_StyledBackground);

    NotifyTipManager::instance()->setViewPort(this);
    OverlayWidget::instance()->setViewPort(this);


	//连接登录成功信号
	connect(ui->loginPage, &LoginPage::sig_login_finished, this, [this] {
		if (!m_homePage) {
			m_homePage = new HomePage;
			connect(m_homePage, &HomePage::sig_quit_login, this, [this] {
				m_homePage->deleteLater();
				ui->stackedWidget->setCurrentWidget(ui->loginPage);
				});
		}
		//设置路由
		m_homePage->setRouters(ContextHolder::instance()->routers());
		//添加家界面
		ui->stackedWidget->addWidget(m_homePage);
		ui->stackedWidget->setCurrentWidget(m_homePage);

		//加载头像
		loadAvatar();
		});


	HTTP_HANDLER_INIT(mainwindow);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::loadAvatar()
{
	auto self = ContextHolder::instance()->self();
	//获取本地头像路径
	auto path = Config::instance()->avatarPath() + "/" + self->user_name + ".png";
	//如果本地头像存在
	if (QFile::exists(path)) {
		qDebug()<<"找到了本地头像";

		//加载本地头像
		self->_pixmap.load(path);
		//通知头像改变
		emit GlobalSignalHub::instance()->avatarChanged(self->_pixmap);
		//如果本地头像存在时间超过一周，那么从服务器重新拉取
		QFileInfo fileinfo(path);
		if (fileinfo.lastModified().daysTo(QDateTime::currentDateTime()) > 7) {
			qDebug() << "本地头像缓存超过一周，重新拉取";
			QFile::remove(path);
		}
	}
	else {
		qDebug()<<"未找到了本地头像，从服务器加载";
		HttpMgr::instance()->get(URL("/profile/avatar"), ReqId::avatar_get, Modules::MainWindow);
	}
}


HTTP_HANDLER_IMPL(MainWindow, mainwindow)
{
	if (errc != ErrorCode::Success) {
		NotifyTipManager::instance()->addNotifyTip(msg, NotifyTipBox::TypeError);
		return;	
	}

	try
	{
		//有可能是二进制数据
		if (req_id == ReqId::avatar_get) {
			auto self = ContextHolder::instance()->self();
			//将数据加载成Pixmap
			if (self->_pixmap.loadFromData(msg)) {
				//通知头像改变
				emit GlobalSignalHub::instance()->avatarChanged(self->_pixmap);
				//将头像保存到本地
				self->_pixmap.save(Config::instance()->avatarPath() + "/" + ContextHolder::instance()->self()->user_name + ".png", "png");
				NotifyTipManager::instance()->addNotifyTip("头像加载成功", NotifyTipBox::TypeSuccess);
			}
			else {
				try{
					auto json = json_t::parse(msg);
					NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
				}
				catch (const std::exception &e) {
					NotifyTipManager::instance()->addNotifyTip(QString("头像加载失败") + e.what(), NotifyTipBox::TypeError);
				}
			}
		}
		else {
			auto json = json_t::parse(msg);
			if (json["code"] != 200) {
				NotifyTipManager::instance()->addNotifyTip(json["msg"].toString(), NotifyTipBox::TypeError);
				return;
			}
			_handlers[req_id](json);
		}
	}
	catch (const std::exception& e) {
		NotifyTipManager::instance()->addNotifyTip(e.what(), NotifyTipBox::TypeError);
	}

}


void MainWindow::initHandlers() {
}

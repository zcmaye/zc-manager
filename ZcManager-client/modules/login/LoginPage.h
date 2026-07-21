#pragma once

#include <QtWidgets/QWidget>
#include "ui_LoginPage.h"
#include "common/utils/CaptchaMaker.h"
#include "common/network/HttpMgr.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginPageClass; };
QT_END_NAMESPACE

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

    void setCurrentWidget(QWidget* w);

    /** 是否启用注册功能  */
    void setRegisterEnabled(bool enable);
    bool registerEnabled()const;

    void clear();
    void saveConfig();
protected:
    void paintEvent(QPaintEvent* ev)override;
    void resizeEvent(QResizeEvent* ev)override;
public slots:
    void on_loginBtn_clicked();
    void on_reg_regBtn_clicked();
signals:
    void sig_login_finished();
private:
    Ui::LoginPageClass *ui;
    QString m_copyright;
    bool m_regEnabled;
    CaptchaMaker m_maker;
    QPair<QString, QString> m_loginCaptcha;
    QPair<QString, QString> m_regCaptcha;

    //声明网络请求
    HTTP_HANDLER_DECL(login_reg);
};


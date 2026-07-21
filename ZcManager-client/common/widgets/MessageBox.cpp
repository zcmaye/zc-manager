#include "MessageBox.h"
#include <QCloseEvent>

MessageBox::MessageBox(QWidget *parent)
	:MessageBox(Success,parent)
{
}

MessageBox::MessageBox(MessageType type, QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::MessageBoxClass())
	, m_type(type)
{
	ui->setupUi(this);
	//去标边框
	setWindowFlag(Qt::FramelessWindowHint);
	//背景透明
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_StyledBackground);
	//信号
	connect(ui->cancelBtn, &QPushButton::clicked, this, [this] {
		emit closed(Role::RejectRole);
		});

	connect(ui->okBtn, &QPushButton::clicked, this, [this] {
		emit closed(Role::AcceptRole);
		});
	setType(type);
}

MessageBox::~MessageBox()
{
	delete ui;
}

void MessageBox::setTitle(const QString& title)
{
	ui->titleLab->setText(title);
}

QString MessageBox::title() const
{
	return ui->titleLab->text();
}

void MessageBox::setIcon(const QPixmap& icon)
{
	ui->iconLab->setPixmap(icon);
}

QPixmap MessageBox::icon() const
{
	return ui->iconLab->pixmap();
}

void MessageBox::setMessage(const QString& msg)
{
	ui->tipMsgLab->setText(msg);
}

QString MessageBox::message() const
{
	return ui->tipMsgLab->text();
}

void MessageBox::setType(MessageType type)
{
	switch (m_type)
	{
	case MessageBox::Warning:
		setIcon(QPixmap(":/Resource/icons/tip-warning.svg"));
		break;
	case MessageBox::Error:
		setIcon(QPixmap(":/Resource/icons/tip-close.svg"));
		break;
	case MessageBox::Success:
		setIcon(QPixmap(":/Resource/icons/tip-success.svg"));
		break;
	default:
		break;
	}
}

MessageBox::MessageType MessageBox::type() const
{
	return m_type;
}

void MessageBox::setCancelButtonHidden(bool hidden)
{
	ui->cancelBtn->setHidden(hidden);
}

void MessageBox::setOkButtonHidden(bool hidden)
{
	ui->okBtn->setHidden(hidden);
}

void MessageBox::closeEvent(QCloseEvent* ev)
{
	this->hide();
	ev->ignore();
	emit closed(Role::RejectRole);
}


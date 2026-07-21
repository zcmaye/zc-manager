#pragma once

#include <QWidget>
#include "ui_MessageBox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MessageBoxClass; };
QT_END_NAMESPACE

class MessageBox : public QWidget
{
	Q_OBJECT

public:
	enum Role {
		AcceptRole,
		RejectRole,
	};
	enum MessageType {
		Warning,
		Error,
		Success
	};
public:
	MessageBox(QWidget *parent = nullptr);
	MessageBox(MessageType type,QWidget *parent = nullptr);
	~MessageBox();

	void setTitle(const QString& title);
	QString title()const;

	void setIcon(const QPixmap& icon);
	QPixmap icon()const;

	void setMessage(const QString& msg);
	QString message()const;

	void setType(MessageType type);
	MessageType type()const;

	void setCancelButtonHidden(bool hidden);
	void setOkButtonHidden(bool hidden);
protected:
	void closeEvent(QCloseEvent* ev)override;
signals:
	void closed(int ret);
private:
	Ui::MessageBoxClass *ui;
	MessageType m_type;
};


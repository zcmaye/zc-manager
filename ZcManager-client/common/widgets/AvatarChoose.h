#pragma once

#include <QWidget>
#include "ui_AvatarChoose.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AvatarChooseClass; };
QT_END_NAMESPACE

class AvatarChoose : public QWidget
{
	Q_OBJECT

public:
	AvatarChoose(QWidget *parent = nullptr);
	~AvatarChoose();

	void setPixmap(const QPixmap& pix);
public slots:
	void on_chooseBtn_clicked();
	void on_commitBtn_clicked();
signals:
	void sig_avatar(QPixmap);
private:
	Ui::AvatarChooseClass *ui;
	float m_rotate;
};


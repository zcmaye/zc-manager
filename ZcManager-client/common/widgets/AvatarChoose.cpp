#include "AvatarChoose.h"
#include "common/utils/Utils.h"
#include <QFileDialog>

AvatarChoose::AvatarChoose(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AvatarChooseClass())
	, m_rotate(0)
{
	ui->setupUi(this);
	Utils::SetDropShadow(ui->avatarLab);

	connect(ui->zoomInBtn, &QPushButton::clicked, ui->avatarCropperbox, &AvatarCropperBox::zoomIn);
	connect(ui->zoomOutBtn, &QPushButton::clicked, ui->avatarCropperbox, &AvatarCropperBox::zoomOut);
	connect(ui->leftRotateBtn, &QPushButton::clicked, ui->avatarCropperbox, [this] {
		ui->avatarCropperbox->rotate(m_rotate += 90);
		});
	connect(ui->rightRotateBtn, &QPushButton::clicked, ui->avatarCropperbox, [this] {
		ui->avatarCropperbox->rotate(m_rotate -= 90);
		});

	connect(ui->avatarCropperbox, &AvatarCropperBox::imageChanged, this, [this](const QPixmap& pix)
		{
			ui->avatarLab->setPixmap(Utils::toRoundPixamp(pix));
		});
}

AvatarChoose::~AvatarChoose()
{
	delete ui;
}

void AvatarChoose::setPixmap(const QPixmap& pix)
{
	ui->avatarCropperbox->setPixmap(pix);
}

void AvatarChoose::on_commitBtn_clicked()
{
	emit sig_avatar(ui->avatarCropperbox->cropperImage());
}


void AvatarChoose::on_chooseBtn_clicked()
{
	auto filename  =  QFileDialog::getOpenFileName(this, "选择图片", "./", "Images (*.jpg;*.png,*.gif,*.bmp);;All (*.*)");
	if (filename.isEmpty()) {
		return;
	}
	ui->avatarCropperbox->setPixmap(QPixmap(filename));
}


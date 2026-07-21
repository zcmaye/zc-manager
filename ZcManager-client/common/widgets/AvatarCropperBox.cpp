#include "AvatarCropperBox.h"
#include <QPainter>
#include <QWheelEvent>

AvatarCropperBox::AvatarCropperBox(QWidget *parent)
	: QWidget(parent)
{}

AvatarCropperBox::~AvatarCropperBox()
{}

void AvatarCropperBox::setPixmap(const QPixmap & pixmap)
{
	m_cropperImage.image = pixmap;
	m_cropperImage.viewImage = pixmap;
	m_cropperImage.position = {
		-pixmap.width()/2,
		-pixmap.height()/2
	};
	update();
}

QPixmap AvatarCropperBox::pixmap() const
{
	return m_cropperImage.image;
}

void AvatarCropperBox::zoomIn()
{
	m_cropperImage.scale *= 1.1;
	update();
}

void AvatarCropperBox::zoomOut()
{
	m_cropperImage.scale *= 0.9;
	update();
}

void AvatarCropperBox::rotate(float otate)
{
	m_cropperImage.rotate = otate;
	update();
}

void AvatarCropperBox::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);

	auto bg = backgroundLayer();

	//绘制背景
	painter.drawPixmap(0, 0, bg);

	//绘制前景颜色
	painter.fillRect(rect(), QColor(102,102,102,120));

	//绘制当前选区的内容
	painter.drawPixmap(m_faceRect, bg, m_faceRect);

	//绘制裁剪矩形
	painter.setPen(QColor(93, 138, 182,230));
	painter.drawRect(m_faceRect);

	//效果差一点，效率高
	//emit imageChanged(bg.copy(m_faceRect));
	//效果好，效率低
	emit imageChanged(cropperImage());
}

void AvatarCropperBox::wheelEvent(QWheelEvent* ev)
{
	if (ev->angleDelta().y() > 0) {
		zoomIn();
	}
	else {
		zoomOut();
	}
}

void AvatarCropperBox::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton ) {
		if (m_faceRect.contains(ev->pos())) {
			m_facePressed = true;
			m_dragImagePressed = false;
			m_faceOffset= ev->pos() - m_faceRect.topLeft();
		}
		else {
			m_dragImagePressed = true;
			m_facePressed = false;
			m_dragImageOffset = ev->pos() - m_cropperImage.position;
		}
	}
}

void AvatarCropperBox::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		m_dragImagePressed = false;
		m_facePressed = false;
	}
}

void AvatarCropperBox::mouseMoveEvent(QMouseEvent* ev)
{
	if (m_dragImagePressed) {
		m_cropperImage.position = ev->pos() - m_dragImageOffset;
		update();
	}
	else if(m_facePressed) {
		m_faceRect.moveTopLeft(ev->pos() - m_faceOffset);
		//限制x
		if (m_faceRect.left() < 0) {
			m_faceRect.moveLeft(0);
		}
		else if (m_faceRect.right() > width()) {
			m_faceRect.moveRight(width() - 2);
		}
		//限制y
		if (m_faceRect.top() < 0) {
			m_faceRect.moveTop(0);
		}
		else if (m_faceRect.bottom() > height()) {
			m_faceRect.moveBottom(height() - 2);
		}

		update();
	}
}

void AvatarCropperBox::resizeEvent(QResizeEvent* ev)
{
	m_faceRect = {(width() - 200) / 2,(height() - 200) / 2, 200, 200 };
}

QPixmap AvatarCropperBox::backgroundLayer()
{
	if (_backgroundImg.isNull()) {
		_backgroundImg = QPixmap(size());
	}
	QPainter painter(&_backgroundImg);
	painter.save();
	//平移坐标轴
	auto r = rect();
	painter.translate(rect().center());

	//绘制背景
	painter.setPen(Qt::NoPen);
	painter.setBrush(QPixmap(":/Resource/images/transparent.png"));
	painter.drawRect(QRect{ -r.width() / 2,-r.height()/2,r.width(),r.height()});

	//缩放处理
	if (m_cropperImage.scale != 1) {
		//计算缩放后的大小		
		int w = m_cropperImage.image.size().width() * m_cropperImage.scale;
		int h = m_cropperImage.image.size().height() * m_cropperImage.scale;
		//判断是否已经缩放了
		auto size = m_cropperImage.image.size().scaled(w, h, Qt::AspectRatioMode::KeepAspectRatio);
		if (m_cropperImage.viewImage.size() != size) {
			//缩放时，保持缩放中心在图片中心
			m_cropperImage.position = {
				m_cropperImage.position.x() - (w - m_cropperImage.viewImage.width())/2,
				m_cropperImage.position.y() - (h - m_cropperImage.viewImage.height())/2
			};
			m_cropperImage.viewImage = m_cropperImage.image.scaled(size,Qt::AspectRatioMode::KeepAspectRatio);
		}
	}

	//旋转坐标轴
	painter.rotate(m_cropperImage.rotate);
	//绘制图片
	painter.drawPixmap(m_cropperImage.position, m_cropperImage.viewImage);
	painter.restore();
	return _backgroundImg;
}

QPixmap AvatarCropperBox::cropperImage()
{
	QPixmap pix(size());
	QPainter painter(&pix);
	//平移坐标轴
	auto r = rect();
	painter.translate(rect().center());

	//绘制背景
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawRect(QRect{ -r.width() / 2,-r.height()/2,r.width(),r.height()});

	//旋转坐标轴
	painter.rotate(m_cropperImage.rotate);
	//绘制图片
	painter.drawPixmap(m_cropperImage.position, m_cropperImage.viewImage);
	return pix.copy(m_faceRect);
}


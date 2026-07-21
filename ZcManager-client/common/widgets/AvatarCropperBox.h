#pragma once

#include <QWidget>

class AvatarCropperBox  : public QWidget
{
	Q_OBJECT

public:
	AvatarCropperBox(QWidget *parent);
	~AvatarCropperBox();

	void setPixmap(const QPixmap& pixmap);
	QPixmap pixmap()const;

	QPixmap cropperImage();

	void zoomIn();
	void zoomOut();
	void rotate(float otate);
protected:
	void paintEvent(QPaintEvent* ev)override;
	void wheelEvent(QWheelEvent* ev)override;
	void mousePressEvent(QMouseEvent* ev)override;
	void mouseReleaseEvent(QMouseEvent* ev)override;
	void mouseMoveEvent(QMouseEvent* ev)override;
	void resizeEvent(QResizeEvent* ev)override;
signals:
	void imageChanged(QPixmap);
private	:
	struct CropperImage {
		QPixmap image;		/*!原图*/
		QPixmap viewImage;	/*!用于显示的图片(缩放之后的)*/
		QPoint position;	/*!图片位置*/
		float scale{1};		/*!缩放因子*/
		float rotate{0};	/*!旋转角度*/
	}m_cropperImage;

	bool m_dragImagePressed;	/*!是否是拖动图片按下*/
	QPoint m_dragImageOffset;	/*!图片偏移量*/

	bool m_facePressed;			/*!是否移动裁剪面*/
	QPoint m_faceOffset;		/*!裁剪框偏移量*/
	QRect m_faceRect;			/*!裁剪矩形*/


	QPixmap _backgroundImg;
	QPixmap backgroundLayer();
};


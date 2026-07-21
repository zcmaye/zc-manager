#pragma once

#include <QLabel>

class ClickLabel  : public QLabel
{
	Q_OBJECT

public:
	ClickLabel(QWidget *parent);
	~ClickLabel();

	void click();

	void setMaskLayerEnabled(bool enable);
	bool maskLayerEnabled()const;

	void setMaskLayerPixmap(const QPixmap& pix);
	QPixmap maskLayerPixmap()const;
protected:
	void mousePressEvent(QMouseEvent* ev)override;
	void mouseReleaseEvent(QMouseEvent* ev)override;
	void enterEvent(QEnterEvent* ev)override;
	void leaveEvent(QEvent* ev)override;
	void paintEvent(QPaintEvent* ev)override;
signals:
	void clicked();
private:
	bool m_pressed;
	bool m_hovered;
	bool m_maskLayerEnabled;
	QPixmap m_maskLayerPixmap;
};


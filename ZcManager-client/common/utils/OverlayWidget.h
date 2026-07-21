#pragma once

#include "common/base/Singleton.hpp"
#include <QWidget>

class OverlayWidget  : public QWidget ,public Singleton<OverlayWidget>
{
	Q_OBJECT
	friend class Singleton<OverlayWidget>;
public:
	enum PopPosition {
		LeadingPosition,	/*!前边*/
		MiddlePosition,		/*!中间*/
		TrilingPosition,	/*!后边*/
	};
public:
	OverlayWidget(QWidget *parent = nullptr);
	~OverlayWidget();

	/**
	 * 设置视口.
	 */
	void setViewPort(QWidget* w);
	QWidget* viewPort()const;

	void setBackgroundColor(const QColor& color);
	QColor backgroundColor()const;

	/**
	 * 弹出窗口.
	 */
	void popup(QWidget* w,PopPosition position = MiddlePosition);

protected:
	bool eventFilter(QObject* watched, QEvent* ev)override;
	void paintEvent(QPaintEvent* ev)override;
	void resizeEvent(QResizeEvent* ev)override;
	void keyPressEvent(QKeyEvent* ev)override;
private:
	QWidget* m_viewPort{};
	QWidget* m_popWidget{};
	QColor m_backgroundColor{104, 104, 104,200};
	PopPosition m_position;

	void onResize();
};


#pragma once

#include <QLineEdit>

class DropDown  : public QLineEdit
{
	Q_OBJECT

public:
	DropDown(QWidget *parent);
	~DropDown();

	/**
	 * 获取前面的ACtion.
	 */
	QAction* leadingAction();
	/**
	 * 获取后面的ACtion.
	 */
	QAction* trailingAction();
	/**
	 * 设置后面的Action是一个清除按钮.
	 */
	void setTrailingActionClear(bool enable = true);

	void setWidget(QWidget* w);
	QWidget* widget()const;

	QWidget* container()const;
	void setWidthSync(bool sync = true);
public:
	void showPopup();
	void hidePopup();
	bool isPop();
protected:
	void mousePressEvent(QMouseEvent* ev)override;
	void mouseReleaseEvent(QMouseEvent* ev)override;
	void resizeEvent(QResizeEvent* ev)override;
signals:
	void sig_resize(const QSize& size);
private:
	QAction* m_leadingAct{};
	QAction* m_trailingAct{};
	QWidget* m_container{};
	QWidget* m_widget{};
	bool m_widthSync{ false };	/*!容器的宽度是否同步DropDown的宽度*/
};


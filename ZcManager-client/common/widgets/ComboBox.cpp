#include "ComboBox.h"
#include <QLineEdit>
#include <QAbstractItemView>
#include <QEvent>
#include <QMouseEvent>

ComboBox::ComboBox(QWidget *parent)
	: QComboBox(parent)
	, m_button(new QPushButton(this))
{
	//setObjectName("ComboBox");
	setAttribute(Qt::WA_StyledBackground);
	connect(this, &QComboBox::currentIndexChanged, [this](int index) {
		//qDebug() << index;
		});
	/*m_button->setStyleSheet(R"(QPushButton{
	border:none;
})");*/
	view()->installEventFilter(this);
	m_button->installEventFilter(this);
	connect(m_button, &QPushButton::clicked, this, [this] {updateState(); });
	connect(this, &QComboBox::currentIndexChanged, this, [this](int index) {
		//else {
		//	setState("on", "off");
		//}
		});


	setState("state", "off");
}

ComboBox::~ComboBox()
{}

void ComboBox::setState(const char* key, const QVariant & v)
{
	m_button->setProperty(key, v);
	m_button->style()->unpolish(m_button);
	m_button->style()->polish(m_button);
}

void ComboBox::updateState(bool clear)
{
	//qDebug() << __FUNCTION__;
	//如果是弹出下拉框
	if (currentIndex() == -1) {
		showPopup();
		setState("state", "on");
	}
	//如果选择了数据，则清空
	else {
		if (clear)
			setCurrentIndex(-1);
		else
			showPopup();
		setState("on", "off");
	}
}


void ComboBox::resizeEvent(QResizeEvent * ev) {
	m_button->resize(height(), height());
	m_button->move(width() - m_button->width(),0);
	if (lineEdit() && !lineEdit()->isReadOnly()) {
		lineEdit()->setReadOnly(true);
		lineEdit()->installEventFilter(this);
	}
}

void ComboBox::paintEvent(QPaintEvent* ev)
{
	if (this->lineEdit()) {
		lineEdit()->setPlaceholderText(placeholderText());
	}
}

bool ComboBox::eventFilter(QObject* watched, QEvent* ev)
{
	if (watched == view()) {
		if (ev->type() == QEvent::Show) {
			setState("state", "on");
		}
		else if(ev->type() == QEvent::Hide) {
			setState("state", "off");
		}
		//qDebug() << view()->isHidden() << view()->isVisible();
		//qDebug() << ev->type();
	}
	else if (watched == m_button)
	{
		if (ev->type() == QEvent::HoverEnter) {
			if (currentIndex() != -1) {
				setState("state", "clear");
			}
		}
		else if (ev->type() == QEvent::HoverLeave) {
			if (view()->isVisible())
				setState("state", "on");
			else
				setState("state", "off");
		}
	}
	else if (watched == lineEdit()) {
		if (ev->type() == QEvent::MouseButtonPress) {
			auto mev = dynamic_cast<QMouseEvent*>(ev);
			if (mev->button() == Qt::LeftButton) {
				//qDebug() << "xxxxxxxxxxxx";
				//qDebug() << view()->isHidden() << view()->isVisible();
				updateState(false);
			}
		}
		else if (ev->type() == QEvent::HoverEnter) {
			if (currentIndex() != -1) {
				setState("state", "clear");
			}
		}
		else if (ev->type() == QEvent::HoverLeave) {
			if (view()->isVisible())
				setState("state", "on");
			else
				setState("state", "off");
		}
	}
	//qDebug() << m_button->property("state");
	return false;
}


#pragma once

#include <QComboBox>
#include <QPushButton>

class ComboBox  : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(QString hello MEMBER m_hello)
public:
	ComboBox(QWidget *parent);
	~ComboBox();


	void setState(const char* key, const QVariant& v);
	void updateState(bool clear = true);
protected:
	void resizeEvent(QResizeEvent* ev)override;
	void paintEvent(QPaintEvent*ev)override;
	bool eventFilter(QObject* watched, QEvent* ev)override;
private:
	QPushButton* m_button{};
	QString m_hello;
};


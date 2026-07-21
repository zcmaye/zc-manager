#include "CaptchaMaker.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QPainter>

#define RAND(min,max) QRandomGenerator::global()->bounded(min,max)
#define RAND_COLOR()  QColor(RAND(0,256),RAND(0,256),RAND(0,256))

CaptchaMaker::CaptchaMaker()
	:m_type(Math)
{
}

CaptchaMaker::~CaptchaMaker()
{}

void CaptchaMaker::setType(Type type)
{
	m_type = type;
}

CaptchaMaker::Type CaptchaMaker::type() const
{
	return m_type;
}

QPair<QString, QString> CaptchaMaker::createText()
{
	switch (m_type)
	{
	case CaptchaMaker::Char:
		return createCharText();
	case CaptchaMaker::Math:
		return createMathText();
	default:
		break;
	}
	qWarning() << "type error!";
	return {};
}

QPixmap CaptchaMaker::createImage(const QString& text)
{
	switch (m_type)
	{
	case CaptchaMaker::Char:
		return createCharImage(text);
	case CaptchaMaker::Math:
		return createMathImage(text);
	default:
		break;
	}
	qWarning() << "type error!";
	return QPixmap();
}

QPair<QString, QString> CaptchaMaker::createCharText()
{
	QString text;
	for (int i = 0; i < charConfig.length; i++) {
		if (RAND(0, 2) == 0) {
			text.append(QChar('0' + RAND(0, 10)));
		}
		else {
			if (RAND(0, 2) == 0) {
				text.append(QChar('A' + RAND(0, 26)));
			}
			else {
				text.append(QChar('a' + RAND(0, 26)));
			}
		}
	}
	return { text,text };
}

QPair<QString, QString> CaptchaMaker::createMathText()
{
	QString text, answer;
	//生成数据
	auto it = mathConfig.ops.begin();
	std::advance(it, RAND(0, mathConfig.ops.size()));		//0-加法 1-减法 2-乘法 3-除法
	int op = 0;
	if (it != mathConfig.ops.end()) {
		op = *it;
	}
	int a = RAND(mathConfig.limits.first, mathConfig.limits.second);
	int b = RAND(mathConfig.limits.first, mathConfig.limits.second);

	switch (op)
	{
	case 0:	//加法
		answer = QString::number(a + b);
		text = QString("%1+%2=?").arg(a).arg(b);
		break;
	case 1:	//减法
		a = qMax(a, b);
		b = qMin(a, b);
		answer = QString::number(a - b);
		text = QString("%1-%2=?").arg(a).arg(b);
		break;
	case 2:	//乘法
		a = a % 10;
		b = b % 10;
		answer = QString::number(a * b);
		text = QString("%1×%2=?").arg(a).arg(b);
		break;
	case 3:	//除法
		b = b % 9 + 1;	//[0,9]
		answer = QString::number(RAND(mathConfig.limits.first, mathConfig.limits.second) % 10 + 1);
		a = b * answer.toInt();
		text = QString("%1÷%2=?").arg(a).arg(b);
		break;
	default:
		text = "1+1=?";
		answer = "2";
		break;
	}
	return { text,answer };
}

QPixmap CaptchaMaker::createCharImage(const QString& text)
{
	QPixmap img(charConfig.imageSize);

	//渐变背景
	QLinearGradient gradient(0, 0, img.width(), img.height());
	gradient.setColorAt(0, charConfig.startColor);
	gradient.setColorAt(1, charConfig.endColor);

	//画家
	QPainter painter(&img);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(gradient);
	painter.setPen(charConfig.borderColor);

	//画背景
	painter.drawRect(img.rect().adjusted(0,0,-1,-1));
	//设置字体
	QFont font("黑体", 24, QFont::Bold);
	painter.setFont(font);
	//绘制验证码文本
	//painter.drawText(img.rect(), text, QTextOption(Qt::AlignCenter));
	//一个一个字符绘制
	auto fontMetri = painter.fontMetrics();
	int x = (img.width() - (fontMetri.horizontalAdvance(text) + (text.size() - 1) * charConfig.spacing)) / 2;
	int y = (img.height() - fontMetri.height()) / 2 + fontMetri.height() - 5;
	int totalW = 0;
	for (int i = 0; i < text.size(); i++) {
		painter.save();
		painter.setPen(RAND_COLOR());
		//扭曲
		if (RAND(0, 2) == 0) {
			painter.shear(RAND(0, 31) / 100.0, 0);
		}
		//旋转
		else {
			painter.rotate(RAND(0, 10));
		}
		painter.drawText(QPoint(x + totalW, y), text[i]);
		totalW += fontMetri.horizontalAdvance(text[i]) + charConfig.spacing;
		painter.restore();
	}

	//绘制噪点
	for (int i = 0; i < charConfig.noiseCount; i++) {
		painter.setPen(QPen(RAND_COLOR(),RAND(1,3)));
		painter.drawPoint(QPoint(RAND(0,img.width()),RAND(0,img.height())));
	}

	//绘制干扰线
	for (int i = 0; i < charConfig.interferingLineCount; i++) {
		painter.setPen(QPen(RAND_COLOR(),RAND(1,3)));
		QList<QPoint> points{
			{RAND(0,img.width()/3),RAND(0,img.height())},
			{RAND((int)(img.width()*0.3),(int)(img.width()*0.6)),RAND(0,img.height())},
			{RAND((int)(img.width() * 0.6),img.width()),RAND(0,img.height())},
		};
		painter.drawPolyline(points);
	}
	return img;
}

QPixmap CaptchaMaker::createMathImage(const QString& text)
{
	QPixmap img(mathConfig.imageSize);

	//渐变背景
	QLinearGradient gradient(0, 0, img.width(), img.height());
	gradient.setColorAt(0, mathConfig.startColor);
	gradient.setColorAt(1, mathConfig.endColor);

	//画家
	QPainter painter(&img);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(gradient);
	painter.setPen(mathConfig.borderColor);

	//画背景
	painter.drawRect(img.rect().adjusted(0,0,-1,-1));
	//设置字体
	QFont font("Lobster", 20);
	painter.setFont(font);

	//绘制验证码
	painter.setPen(mathConfig.textColor);
	if (RAND(0, 2) == 0) {
		painter.shear(RAND(0, 50) / 100.0, 0);
	}
	else {
		painter.shear(-RAND(0, 50) / 100.0, 0);
	}
	painter.drawText(img.rect(), text, QTextOption(Qt::AlignCenter));

	return img;
}


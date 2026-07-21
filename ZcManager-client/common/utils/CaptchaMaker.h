#pragma once

#include <QObject>
#include <QPixmap>
#include <QSet>

class CaptchaMaker{
public:
	enum Type {
		Char,	/*!字符验证码*/
		Math	/*!数学验证码*/
	};
public:
	CaptchaMaker();
	~CaptchaMaker();

	void setType(Type type);
	Type type()const;

	/**
	 * 创建验证码文本.
	 */
	QPair<QString, QString> createText();

	/**
	 * 创建验证码图片.
	 */
	QPixmap createImage(const QString& text);
private:
	Type m_type;


	QPair<QString, QString> createCharText();
	QPair<QString, QString> createMathText();
	QPixmap createCharImage(const QString& text);
	QPixmap createMathImage(const QString& text);
public:
	struct Config {
		QSize imageSize{112,41};		/*!验证码图片大小*/
		QColor startColor{198,195,198};	/*!渐变开始颜色*/
		QColor endColor{247,247,247};	/*!渐变结束颜色*/
		QColor borderColor{156, 184, 135};/*!边框颜色*/
	};
	struct CharConfig:public Config {
		int length{4};					/*!字符个数*/
		int spacing{5};					/*!字符之间的间距*/
		int noiseCount{ 200 };			/*!干扰噪点数量*/
		int interferingLineCount{ 3};	/*!干扰线数量*/
	}charConfig;

	struct MathConfig:public Config {
		QSet<int> ops{0,1,2,3};				/*!符号：0-加法 1-减法 2-乘法 3-除法*/
		QPair<int, int> limits{1,21};		/*!数据范围*/
		QColor textColor{ 2, 2, 239 };		/*!文本颜色*/
	}mathConfig;
};


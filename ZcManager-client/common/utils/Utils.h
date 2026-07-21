#pragma once

#include <QPixmap>
#include <QIcon>

class QWidget;
class QModelIndex;

class Utils 
{
public:
	/**
	 * 设置窗口阴影.
	 */
	static void SetDropShadow(QWidget* w);

	/**
	 * 把图片转成圆角图片.
	 */
	static QPixmap toRoundPixamp(const QPixmap& pix, int radius = -1);

	/**
	 * 设置图标的颜色.
	 */
	static QIcon SetIconColor(const QIcon& icon,const QColor& color = Qt::white);

	/**
	 * 判断parent是不是child的父项(直接或间接父项).
	 */
	static bool isParent(const QModelIndex& child, const QModelIndex& parent);

	/**
	 * 获取index的父项数量.
	 */
	static int  parentCount(const QModelIndex& index);

	/**
	 * 把字符串转成唯一的哈希值.
	 */
	static constexpr unsigned int OptionHash(const char* str, int h = 0) {
		return !str[h] ? 5381 : (OptionHash(str, h + 1) * 33) ^ str[h];
	}

	/**
	 * 把搜索栏的布局修改为浮动布局.
	 */
	static void SetSearchBarFlowLayout(QWidget* searchBar);
};


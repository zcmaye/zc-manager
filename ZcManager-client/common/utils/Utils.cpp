#include "Utils.h"
#include "../widgets/FlowLayout.h"

#include <QWidget>
#include <QGraphicsDropShadowEffect>

#include <QPainter>
#include <QPainterPath>
#include <QModelIndex>


void Utils::SetDropShadow(QWidget* w)
{
	if (!w)
		return;
	w->setAttribute(Qt::WA_TranslucentBackground);

	auto shadow = new QGraphicsDropShadowEffect;
	shadow->setColor(Qt::black);
	shadow->setBlurRadius(9);
	shadow->setOffset(0);
	w->setGraphicsEffect(shadow);
}

QPixmap Utils::toRoundPixamp(const QPixmap& pix, int radius)
{
	//校准圆角半径
	if (radius == -1) {
		radius = qMin(pix.width(), pix.height());
	}

	//构建透明图片
	QPixmap dst(pix.width(), pix.height());
	dst.fill(Qt::transparent);

	//创建画家
	QPainter painter(&dst);
	painter.setRenderHint(QPainter::Antialiasing);
	
	//设置裁剪区域
	QPainterPath clipPath;
	clipPath.addRoundedRect(dst.rect(), radius, radius);
	painter.setClipPath(clipPath);

	painter.drawPixmap(dst.rect(), pix);
	return dst;
}

QIcon Utils::SetIconColor(const QIcon& icon, const QColor& color)
{
	auto pixmap = icon.pixmap(16, 16);
	QPainter painter(&pixmap);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.fillRect(pixmap.rect(), color);
	return QIcon(pixmap);
}

bool Utils::isParent(const QModelIndex& child, const QModelIndex& parent)
{
	if (child.isValid())
		return false;
	auto parentIndex = child.parent();
	while (parentIndex.isValid()) {
		if (parent == parentIndex) {
			return true;
		}
		parentIndex = parentIndex.parent();
	}
	return false;
}

int Utils::parentCount(const QModelIndex& index)
{
	int count = 0;
	auto parentIndex = index.parent();
	while (parentIndex.isValid()) {
		++count;
		parentIndex = parentIndex.parent();
	}
	return count;
}

void Utils::SetSearchBarFlowLayout(QWidget* searchBar)
{
	auto layout = searchBar->layout();
	if (!layout)
		return;

	auto flayout = new FlowLayout;
	flayout->setVerticalCenter();

	QLayoutItem* item = nullptr;
	while (item = layout->takeAt(0)) {
		if (item->widget())
			flayout->addItem(item);
		else
			delete item;
	}
	delete layout;

	searchBar->setLayout(flayout);
}


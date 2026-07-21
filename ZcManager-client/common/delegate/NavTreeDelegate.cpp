#include "NavTreeDelegate.h"
#include "common/utils/Utils.h"
#include <QPainter>
#include <QWidget>

NavTreeDelegate::NavTreeDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{}

NavTreeDelegate::~NavTreeDelegate()
{}

QSize NavTreeDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if (!index.parent().isValid()) {
		return QSize(option.rect.width(), 62);
	}
	return QSize(option.rect.width(), 56);
}

void NavTreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto& rect = option.rect;
	auto text = index.data(Qt::DisplayRole).toString();
	auto icon = index.data(Qt::DecorationRole).value<QIcon>();
	auto treeView = dynamic_cast<QWidget*>(parent());

	auto font = painter->font();
	font.setBold(true);
	font.setPixelSize(15);
	painter->setFont(font);

	//顶级item
	if (!index.parent().isValid()) {
		QColor currentColor = d.bgColor;
		//鼠标悬停了
		if (option.state.testFlag(QStyle::State_MouseOver)) {
			currentColor = d.hoverColor;
		}
		painter->fillRect(rect, currentColor);

		//绘制图标
		painter->drawPixmap(40, rect.y() + (rect.height() - d.iconSize) / 2, icon.pixmap(d.iconSize, d.iconSize));

		//绘制文本
		currentColor = d.fgColor;
		//让所有父级Item都变色
		if (Utils::isParent(m_currentIndex, index)) {
			currentColor = d.selectFgColor;
		}
		//auto parentIndex = m_currentIndex.parent();
		//while (parentIndex.isValid()) {
		//	if (parentIndex == index) {
		//		currentColor = d.selectFgColor;
		//		break;
		//	}
		//	parentIndex = parentIndex.parent();
		//}
		//if (m_currentIndex.parent() == index) {
		//	currentColor = d.selectFgColor;
		//}
		painter->setPen(currentColor);
		//painter->drawText(rect, text, QTextOption(Qt::AlignCenter));
		auto th = painter->fontMetrics().height();
		painter->drawText(40 + d.iconSize + 10, rect.y() + (rect.height() - th) / 2 + th - 3, text);
	}
	//子级item
	else {
		int parentCount = Utils::parentCount(index);
		QColor currentColor = d.childBgColor;
		//鼠标悬停了
		if (option.state.testFlag(QStyle::State_MouseOver)) {
			currentColor = d.childHoverColor;
		}
		painter->fillRect(rect, currentColor);


		auto th = painter->fontMetrics().height();
		QPoint textPos;
		//绘制图标
		if (!icon.isNull()) {
			painter->drawPixmap(d.xOffset + parentCount * d.iconSize, rect.y() + (rect.height() - d.iconSize) / 2, icon.pixmap(d.iconSize, d.iconSize));
			textPos  = {d.xOffset + d.iconSize + d.spacing + parentCount * d.iconSize, rect.y() + (rect.height() - th) / 2 + th - 3 };
		}
		else {
			textPos = { d.xOffset + parentCount * d.iconSize ,rect.y() + (rect.height() - th) / 2 + th - 3 };
		}
		//qDebug() << parentCount;

		//绘制文本
		currentColor = d.childFgColor;
		//如果是子级的父级别
		if (Utils::isParent(m_currentIndex, index)) {
			currentColor = d.selectFgColor;
		}
		else {
			//如果被选中
			if (option.state.testFlag(QStyle::State_Selected)) {
				currentColor = d.childSelectFgColor;
				if (m_currentIndex != index) {
					m_currentIndex = index;
					if (treeView) {
						treeView->update();
					}
				}
			}
		}
		painter->setPen(currentColor);
		//painter->drawText(rect, text, QTextOption(Qt::AlignCenter));
		painter->drawText(textPos, text);
	}

	//绘制展开/折叠标记
	if (option.state.testFlag(QStyle::State_Children)) {
		//展开的
		if (option.state.testFlag(QStyle::State_Open)) {
			painter->drawPixmap(QRect(rect.width() - 32,rect.y() + (rect.height() - d.iconSize)/2,d.iconSize,d.iconSize), QPixmap(":/Resource/icons/arrow-up.svg"));
		}
		//折叠的
		else {
			painter->drawPixmap(QRect(rect.width() - 32,rect.y() + (rect.height() - d.iconSize)/2,d.iconSize,d.iconSize), QPixmap(":/Resource/icons/arrow-down.svg"));
		}
	}
}


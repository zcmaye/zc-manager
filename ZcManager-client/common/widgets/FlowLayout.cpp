#include "FlowLayout.h"
#include <QStyle>
#include <QApplication>
#include <QWidget>

FlowLayout::FlowLayout(QWidget* parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpacing(hSpacing), m_vSpacing(vSpacing), m_alignment(Qt::AlignLeft),m_verticalCenter(false) {
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
    : m_hSpacing(hSpacing), m_vSpacing(vSpacing), m_alignment(Qt::AlignLeft),m_verticalCenter(false) {
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout() {
    QLayoutItem* item;
    while ((item = takeAt(0)) != nullptr) {
        delete item;
    }
}

void FlowLayout::addLayout(QLayout* layout)
{
    if(!layout) { 
        return;
    }
    auto container = new QWidget;
    container->setLayout(layout);
    addWidget(container);
}

void FlowLayout::addItem(QLayoutItem* item) {
    m_itemList.append(item);
}

void FlowLayout::insertItem(int index, QLayoutItem* item)
{
	m_itemList.insert(index, item);
}

int FlowLayout::horizontalSpacing() const {
    if (m_hSpacing >= 0) {
        return m_hSpacing;
    }
    else {
        return qApp->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
    }
}

int FlowLayout::verticalSpacing() const {
    if (m_vSpacing >= 0) {
        return m_vSpacing;
    }
    else {
        return qApp->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
    }
}

void FlowLayout::setHorizontalSpacing(int spacing) {
    m_hSpacing = spacing;
}

void FlowLayout::setVerticalSpacing(int spacing) {
    m_vSpacing = spacing;
}

void FlowLayout::setLayoutAlignment(Qt::Alignment alignment) {
    if (m_alignment != alignment) {
        this->m_alignment = alignment;
        update();
    }
}

Qt::Alignment FlowLayout::layoutAlignment() const
{
    return this->m_alignment;
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return { };
}

void FlowLayout::setVerticalCenter(bool center)
{
    m_verticalCenter = center;
}

bool FlowLayout::verticalCenter() const
{
    return m_verticalCenter;
}

int FlowLayout::count() const {
    return m_itemList.size();
}

QLayoutItem* FlowLayout::itemAt(int index) const {
    return m_itemList.value(index);
}

QLayoutItem* FlowLayout::takeAt(int index) {
    if (index >= 0 && index < m_itemList.size()) {
        return m_itemList.takeAt(index);
    }
    return nullptr;
}

QSize FlowLayout::minimumSize() const {
    QSize size;
    for (QLayoutItem* item : m_itemList) {
        size = size.expandedTo(item->minimumSize());
    }

    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

QSize FlowLayout::sizeHint() const {
    return minimumSize();
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}

/*
void FlowLayout::setGeometry(const QRect& rect) {
    QLayout::setGeometry(rect);

    if (m_itemList.isEmpty()) {
        return;
    }

    int hSpacing = horizontalSpacing();
    int vSpacing = verticalSpacing();
    auto margins = contentsMargins();

    int x = rect.x() + margins.left();
    int y = rect.y() + margins.top();
    int lineHeight = 0;

    for (QLayoutItem* item : m_itemList) {
        QWidget* widget = item->widget();
        int w = widget->sizeHint().width();
        int h = widget->sizeHint().height();

        //如果需要换行
        if (x + w + hSpacing + margins.left() + margins.right() > rect.right()) {
            x = rect.x() + margins.left();
			y += lineHeight;
            lineHeight = 0;
        }

        // 根据对齐方式调整水平位置
        auto realX = x;
        auto realY = y;
        if (m_alignment == Center) {
            //int lineWidth = y == rect.y() ? rect.width() : (rect.right() - x - hSpacing);
            //x = rect.x() + (lineWidth - w) / 2;
			if (x < rect.width() / 2) {
				realX = rect.right() / 2 - x;
            }
            else {
				realX = rect.right() / 2 + x;
            }
        }
        else if (m_alignment == Right) {
            realX = rect.right() - x - w;
        }
        widget->setGeometry(QRect(QPoint(realX, realY), widget->sizeHint()));

        //widget->setGeometry(QRect(QPoint(x, y), widget->sizeHint()));
        x += w + hSpacing;
		lineHeight = qMax(lineHeight, h + vSpacing);
    }
}
*/

void FlowLayout::setGeometry(const QRect& rect) {
    QLayout::setGeometry(rect);
    doLayout(rect);
}

int FlowLayout::doLayout(const QRect& rect, bool isTest)const
{
	if (m_itemList.isEmpty() || rect.isNull()) {
		return 0;
	}

	int hSpacing = horizontalSpacing();
	int vSpacing = verticalSpacing();
	auto margins = contentsMargins();

	int x = rect.x() + margins.left();
	int y = rect.y() + margins.top();
	int lineHeight = 0;

	//std::tuple<QList<QLayoutItem*>, int,int> 元组中三个元素分别是（当前行所有item、当前行剩余宽度、行高）
	QMap<int, std::tuple<QList<QLayoutItem*>, int, int>> items;
	int row = 0;

	for (QLayoutItem* item : m_itemList) {
		QWidget* widget = item->widget();
        auto size_hint = widget->sizeHint();
		int w = size_hint.width() == -1 ? widget->width():size_hint.width();
		int h = size_hint.height() == -1 ? widget->height() : size_hint.height();

		//如果需要换行
		if (x + w + margins.right() > rect.right()) {
			x = rect.x() + margins.left();
			y += lineHeight;
			lineHeight = 0;
			row++;
		}

		// 根据对齐方式调整水平位置
        if(!isTest)
			widget->setGeometry(QRect(QPoint(x, y), widget->sizeHint()));

		//如果是居中对齐
		//if (m_alignment != Left) {
		std::get<0>(items[row]).append(item);
		std::get<1>(items[row]) = rect.width() - (x + w + margins.right());
		//}

		x += w + hSpacing;
		lineHeight = qMax(lineHeight, h + vSpacing);
		//保存行高
		std::get<2>(items[row]) = lineHeight;
	}

    if (!isTest) {
        //如果是居中对齐或者右对齐
        //if (m_alignment != Left) {
        int realX = 0;
        int realY = 0;
        for (int i = 0; i < items.size(); i++) {
            //剩下的宽度
            auto remain_width = std::get<1>(items[i]);
            //遍历当前行所有item
            for (auto& item : std::get<0>(items[i])) {
                QWidget* widget = item->widget();
                if (m_alignment == Qt::AlignCenter) {
                    realX = widget->x() + remain_width / 2;
                }
                else if (m_alignment == Qt::AlignRight) {
                    realX = widget->x() + remain_width;
                }
                else {
                    realX = widget->x();
                }
                if (verticalCenter()) {
                    realY = widget->y() + (std::get<2>(items[i]) - widget->height()) / 2;
                }
                else {
                    realY = widget->y();
                }
                item->setGeometry(QRect(QPoint(realX, realY), widget->sizeHint()));
            }
        }
        //}
    }

	return y + lineHeight + margins.bottom() - vSpacing - rect.y();
}


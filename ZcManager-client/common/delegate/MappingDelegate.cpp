#include "MappingDelegate.h"
#include <QPainter>

MappingDelegate::MappingDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
	, m_radius(5)
	, m_size(50,32)
{}

MappingDelegate::~MappingDelegate()
{}

void MappingDelegate::addMapping(const Data & d)
{
	auto data = std::make_shared<Data>();
	*data = d;
	m_datas.append(data);
}

std::shared_ptr<MappingDelegate::Data> MappingDelegate::mapping(const QVariant& value)const
{
	for (auto& d : m_datas) {
		if ( d->value == value) {
			return d;
		}
	}
	qWarning() << value.toString() << "not found!";
	return {};
}

void MappingDelegate::setRadius(int radius)
{
	m_radius = radius;
}

void MappingDelegate::setSize(const QSize& size)
{
	m_size = size;
}

void MappingDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	auto value = index.data(Qt::DisplayRole);
	auto map =  mapping(value);
	if(!map) {
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}
	int x = option.rect.x() + (option.rect.width() - m_size.width()) / 2;
	int y = option.rect.y() + (option.rect.height() - m_size.height()) / 2;
	auto rect = QRect(QPoint(x,y),m_size);

	//抗锯齿
	//painter->setRenderHint(QPainter::Antialiasing);
	painter->save();

	//绘制背景
	painter->setBrush(map->background);
	painter->setPen(map->border);
	painter->drawRoundedRect(rect, m_radius, m_radius);

	//绘制文本
	painter->setPen(map->foreground);
	painter->drawText(rect, map->mapValue.toString(), QTextOption(Qt::AlignCenter));

	painter->restore();
}


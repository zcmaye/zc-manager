#pragma once

#include <QStyledItemDelegate>
#include <QVariant>

class MappingDelegate  : public QStyledItemDelegate
{
	Q_OBJECT
public:
	struct Data {
		QVariant value;		/*!原值*/
		QVariant mapValue;	/*!映射的新值*/
		QColor background;	/*!背景颜色*/
		QColor border;		/*!边框颜色*/
		QColor foreground;	/*!前景(文本)颜色*/
	};
public:
	MappingDelegate(QObject *parent);
	~MappingDelegate();

	void addMapping(const Data& d);
	std::shared_ptr<Data> mapping(const QVariant& value)const;

	void setRadius(int radius);
	void setSize(const QSize& size);
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
	QList<std::shared_ptr<Data>> m_datas;
	int m_radius;
	QSize m_size;
};


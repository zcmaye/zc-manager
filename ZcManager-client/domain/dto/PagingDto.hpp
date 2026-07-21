#pragma once


#include <qglobal.h>
#include <QList>

template<typename T>
struct PagingDto{
	qint32 page;
	qint32 pageSize;
	qint32 totalCount;
	QList<std::shared_ptr<T>> items;
};

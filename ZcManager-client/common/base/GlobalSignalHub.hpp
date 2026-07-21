#pragma once

#include "Singleton.hpp"
#include <QObject>
#include <QPixmap>

class GlobalSignalHub : public QObject, public Singleton<GlobalSignalHub>
{
	Q_OBJECT
	friend class Singleton<GlobalSignalHub>;
public:

signals:
	void avatarChanged(QPixmap avatar);
};

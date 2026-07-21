#pragma once

#include <QEvent>
struct Role;


class StackedWidgetAddPageEvent : public QEvent
{
public:
	inline static QEvent::Type type = QEvent::Type(QEvent::User + 1);

	StackedWidgetAddPageEvent(const QString& component,const QString& title,const 	std::shared_ptr<Role>& role)
		: QEvent(type)
		,m_component(component)
		,m_title(title) 
		,m_role(role)
	{
	}
	
	const QString& component()const { return m_component; }
	const QString& title()const { return m_title; }
	const std::shared_ptr<Role>& role()const { return m_role; }
private:
	QString m_component;
	QString m_title;
	std::shared_ptr<Role> m_role;
};

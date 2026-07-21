#include "PermButton.h"
#include <QEvent>

PermButton::PermButton(QWidget *parent)
	: HoverButton(parent)
{}

PermButton::~PermButton()
{}

void PermButton::setPermission(const QString & perm)
{
	m_perm = perm;
}

QString PermButton::permsission() const
{
	return m_perm;
}

void PermButton::showEvent(QShowEvent * ev)
{
	if (permEnabled()) {
		//没有权限就隐藏
		if (!m_perm.isEmpty() && (!s_permissions.contains(m_perm) && !s_permissions.contains("*:*:*"))) {
			hide();
		}
	}
}

bool PermButton::event(QEvent* e)
{
	//qDebug() << e->type();
	return QPushButton::event(e);
}

void PermButton::setPermissions(const QSet<QString>& perms)
{
	s_permissions = perms;
}

const QSet<QString>& PermButton::permissions()
{
	return s_permissions;
}

void PermButton::setPermEnabled(bool enabled)
{
	s_permEnabled = enabled;
}

const bool PermButton::permEnabled()
{
	return s_permEnabled;
}


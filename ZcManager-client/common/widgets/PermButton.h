#pragma once

#include "HoverButton.h"

class PermButton  : public HoverButton
{
	Q_OBJECT
public:
	PermButton(QWidget *parent);
	~PermButton();

	void setPermission(const QString& perm);
	QString permsission()const;

protected:
	void showEvent(QShowEvent* ev)override;
	bool event(QEvent*)override;
private:
	QString m_perm;

	/**
	 * 所有权限.
	 */
	inline static QSet<QString> s_permissions;

	/**
	 * 是否开启权限验证.
	 */
	inline static bool s_permEnabled{true};
public:
	static void setPermissions(const QSet<QString>& perms);
	static const QSet<QString>& permissions();

	static void setPermEnabled(bool enabled);
	static const bool permEnabled();
};


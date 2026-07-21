#pragma once
#include "common/base/Singleton.hpp"
#include <memory>
#include <QSet>
#include <QString>

struct User;
struct Menu;
struct RouterVo;
struct TreeSelect;

class ContextHolder  : public Singleton<ContextHolder>
{
	friend class Singleton<ContextHolder>;
	ContextHolder();
public:
	~ContextHolder();

	std::shared_ptr<User> self()const;
	void setSelf(std::shared_ptr<User> user);

	const QSet<QString>& permission()const;
	void setPermission(const QSet<QString>& perms);
	bool hasPermission(const QString& perm)const;

	void setToken(const QByteArray& token);
	QByteArray token()const;

	void setRouters(const QList<std::shared_ptr<RouterVo>>& routers);
	QList<std::shared_ptr<RouterVo>> routers()const;

	void setDeptTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect);
	QList<std::shared_ptr<TreeSelect>> deptTreeSelect()const;
	void clearDeptTreeSelect()const;

	void setMenuTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect);
	QList<std::shared_ptr<TreeSelect>> menuTreeSelect()const;
private:
	std::shared_ptr<User> m_slef;
	QSet<QString> m_perms;
};


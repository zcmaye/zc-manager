#include "ContextHolder.h"
#include "domain/entity/User.hpp"
#include "domain/vo/RouterVo.hpp"
#include "common/widgets/PermButton.h"
#include <QDebug>
#include <QApplication>

ContextHolder::ContextHolder()
{}

ContextHolder::~ContextHolder()
{}

std::shared_ptr<User> ContextHolder::self() const
{
    if (!m_slef) {
        qWarning() << "self is nullptr";
    }
    return m_slef;
}

void ContextHolder::setSelf(std::shared_ptr<User> user)
{
    m_slef = user;
}

const QSet<QString>& ContextHolder::permission() const
{
    return m_perms;
}

void ContextHolder::setPermission(const QSet<QString>& perms)
{
    m_perms = perms;
    PermButton::setPermissions(perms);
}

bool ContextHolder::hasPermission(const QString& perm) const
{
	return permission().contains(perm) || permission().contains("*:*:*");
}

void ContextHolder::setToken(const QByteArray& token)
{
    qApp->setProperty("token", token);
}

QByteArray ContextHolder::token() const
{
    auto v = qApp->property("token");
    if (!v.isValid()) {
        qWarning()<< "token is nullptr";
        return {};
    }
    return v.toByteArray();
}

void ContextHolder::setRouters(const QList<std::shared_ptr<RouterVo>>& routers)
{
    qApp->setProperty("routers", QVariant::fromValue(routers));
}

QList<std::shared_ptr<RouterVo>> ContextHolder::routers() const
{
    auto v = qApp->property("routers");
    if (!v.isValid()) {
        qWarning()<< "routers is nullptr";
        return {};
    }
    return v.value<QList<std::shared_ptr<RouterVo>>>();
}

void ContextHolder::setDeptTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect)
{
    qApp->setProperty("deptTreeSelect", QVariant::fromValue(treeSelect));
}

QList<std::shared_ptr<TreeSelect>> ContextHolder::deptTreeSelect() const
{
    auto v = qApp->property("deptTreeSelect");
    if (!v.isValid()) {
        qWarning()<< "deptTreeSelect is nullptr";
        return {};
    }
    return v.value<QList<std::shared_ptr<TreeSelect>>>();
}

void ContextHolder::clearDeptTreeSelect() const {
    qApp->setProperty("deptTreeSelect", QVariant());
}

void ContextHolder::setMenuTreeSelect(const QList<std::shared_ptr<TreeSelect>>& treeSelect)
{
    qApp->setProperty("menuTreeSelect", QVariant::fromValue(treeSelect));
}

QList<std::shared_ptr<TreeSelect>> ContextHolder::menuTreeSelect() const
{
    auto v = qApp->property("menuTreeSelect");
    if (!v.isValid()) {
        qWarning()<< "menuTreeSelect is nullptr";
        return {};
    }
    return v.value<QList<std::shared_ptr<TreeSelect>>>();
}


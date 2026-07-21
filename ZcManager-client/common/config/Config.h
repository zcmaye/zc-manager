#pragma once
#include <QString>
#include <QVariant>
#include <QSettings>

class Config
{
	Config(const QString& filename);
public:
	~Config();

	static std::shared_ptr<Config> instance();

	void setValue(QAnyStringView key, const QVariant& value);
	QVariant value(QAnyStringView key);
	QVariant value(QAnyStringView key,const QVariant& def_value);
	bool contains(QAnyStringView key)const;
	void remove(QAnyStringView key);
public:
	QString profilePath()const;
	QString avatarPath()const;
private:
	QSettings m_settings;
};


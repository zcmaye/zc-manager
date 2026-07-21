#include "Config.h"
#include <QDir>
#include <QFile>
#include <QDebug>


Config::Config(const QString& filename)
	:m_settings(filename,QSettings::IniFormat)
{
}

Config::~Config()
{}

std::shared_ptr<Config> Config::instance()
{
	static std::shared_ptr<Config> config;
	if (!config) {
		config.reset(new Config("config.ini"));
		//有没有D盘
		QStringList drivers = { "F:/","E:/","D:/","C:/" };
		for (auto& d : drivers) {
			if (QFile::exists(d)) {
				QDir dir(d);
				if (dir.mkpath("Zc-Manager-client/profile/avatar")) {
					auto path = QString("%1Zc-Manager-client").arg(d);
					config->setValue("app/profile", path);
					break;
				}
			}
		}

		if (!config->contains("app/host")) {
			config->setValue("app/host", "http://127.0.0.1:8080");
		}
	}
	return config;
}

void Config::setValue(QAnyStringView key, const QVariant& value)
{
	m_settings.setValue(key, value);
}

QVariant Config::value(QAnyStringView key)
{
	return m_settings.value(key);
}

QVariant Config::value(QAnyStringView key, const QVariant& def_value)
{
	return m_settings.value(key, def_value);
}

bool Config::contains(QAnyStringView key) const
{
	return m_settings.contains(key);
}

void Config::remove(QAnyStringView key)
{
	m_settings.remove(key);
}

QString Config::profilePath() const
{
	return m_settings.value("app/profile").toString();
}

QString Config::avatarPath() const
{
	return profilePath() + "/" + "profile/avatar";
}


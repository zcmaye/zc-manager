#pragma once

#include "domain/JsonMapper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

class QJson {
public:
	QJson() = default;

	QJson(std::initializer_list<std::pair<QString, QJsonValue>> args) {
		QJsonObject obj;
		for (auto& [k, v] : args) {
			obj.insert(k, v);
		}
		_jsonValue = obj;
	}

	QJson(const QJsonObject& obj) 
		: _jsonValue(obj) 
	{}

	QJson(const QJsonArray& array)
		: _jsonValue(array) 
	{}

	QJson(const QJsonValue& v)
		: _jsonValue(v) 
	{}

	template<typename T>
	QJson(const T& v) {
		to_json(_jsonValue, v);
	}

	template<typename T>
	QJson(const std::shared_ptr<T>& v) {
		to_json(_jsonValue, *v);
	}

	QByteArray dump(bool isIndent = false)const {
		return _jsonValue.toJson(isIndent ? QJsonDocument::Indented : QJsonDocument::Compact);
	}

	template<typename T>
	operator const T()const {
		T v{};
		from_json(_jsonValue, v);
		return v;
	}

	template<typename T>
	T cast() const {
		return static_cast<T>(*this);
	}

	QJsonObject toObject()const {
		return _jsonValue.toObject();
	}

	QJsonArray toArray()const {
		return _jsonValue.toArray();
	}

	explicit operator QJsonValue()const { return _jsonValue; }
	explicit operator QJsonObject()const { return toObject(); }
	explicit operator QJsonArray()const { return toArray(); }

	static QJson parse(const QByteArray& data) {
		QJsonParseError error;
		auto jdom = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
			qWarning()<< "Json parse error:" << error.errorString();
			throw std::runtime_error(error.errorString().toStdString());
		}
		if (jdom.isNull() || jdom.isEmpty()) {
			return QJson();
		}
		if (jdom.isObject()) {
			return QJson(jdom.object());
		}
		return QJson(jdom.array());
	}

	bool contains(const QString& key)const {
		return !_jsonValue[key].isUndefined();
	}
public:
    const QJsonValue operator[](const QString &key) const{ return _jsonValue[key]; };
    const QJsonValue operator[](QStringView key) const{ return _jsonValue[key]; };
    const QJsonValue operator[](QLatin1StringView key) const{ return _jsonValue[key]; };
    const QJsonValue operator[](qsizetype i) const{ return _jsonValue[i]; };
private:
	QJsonValue _jsonValue;
};

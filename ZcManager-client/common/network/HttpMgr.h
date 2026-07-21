#pragma once

#include "../base/Singleton.hpp"
#include "common/json/QJson.hpp"

#include "HttpEnums.hpp"
#include "HttpMacro.hpp"

#include <QObject>
#include <QNetworkAccessManager> 
#include <QNetworkReply> 

using json_t = QJson;

inline QByteArray json_to_QByteArray(const json_t& json) {
	return QByteArray(json.dump());
}

class HttpMgr :public QObject, public Singleton<HttpMgr>
{
	Q_OBJECT
public:
	using Status = int;

	//Http 请求拦截器
	using RequestInterceptor = std::function<void(QNetworkRequest&)>;
	using ResponseInterceptor = std::function<bool(int status,QByteArray)>;
public:
	~HttpMgr();

	void get(const QUrl& url, ReqId req_id, Modules mod, const QVariantMap& queryParams = {}, const QHttpHeaders& m_headers = {});
	void post(const QUrl& url, const json_t& json, ReqId req_id, Modules mod, const QVariantMap& queryParams = {}, const QHttpHeaders& headers = {});
	void put(const QUrl& url, const json_t& json, ReqId req_id, Modules mod, const QVariantMap& queryParams = {}, const QHttpHeaders& headers = {});
	void del(const QUrl& url, ReqId req_id, Modules mod, const QVariantMap& queryParams = {}, const QHttpHeaders& m_headers = {});

	/**
	 * 下载文件，有数据可读时会触发
	 *  1.sig_http_ready_read和sig_http_download_progress信号.
	 *  2.两个信号必须被处理
	 */
	void download(const QUrl& url, ReqId req_id, Modules mod, const QVariantMap& queryParams = {}, const QHttpHeaders& m_headers = {});
	void upload(const QUrl& url, ReqId req_id, QHttpMultiPart* multipart, Modules mod, const QVariantMap& queryParams = {}, const QHttpHeaders& m_headers = {});
	void upload(const QUrl& url, ReqId req_id, const QStringList& filenames, Modules mod, const QVariantMap& queryParams = {}, const QHttpHeaders& m_headers = {});
	void upload(const QUrl& url, ReqId req_id, const QByteArray& data, Modules mod, bool isFormData = true, const QVariantMap& queryParams = {}, const QHttpHeaders& m_headers = {});

	void debug(bool isDebug) { m_isDebug = isDebug; }
private:
	friend Singleton<HttpMgr>;
	HttpMgr();
	QNetworkAccessManager m_manager;

	QNetworkRequest createRequest(const QUrl& url, const QVariantMap& queryParams = {}, const QHttpHeaders& headers = {});
	void handleFinish(const ReqId& req_id, QNetworkReply* reply, Modules mod);
private slots:
	void slot_http_finish(ReqId req_id, const QByteArray& msg, ErrorCode errCode, Modules mod);
signals:
	void sig_http_finish(ReqId req_id, const QByteArray& msg, ErrorCode errCode, Modules mod);
	void sig_http_ready_read(ReqId req_id, Modules mod, const QByteArray& msg);
	void sig_http_download_progress(ReqId req_id, Modules mod, qint64 bytesReceived, qint64 bytesTotal);
	void sig_http_upload_progress(ReqId req_id, Modules mod, qint64 bytesSent, qint64 bytesTotal);

	//注册模块http相关请求完成发送此信号
	void sig_login_reg_mod_finish(ReqId id, const QByteArray& res, ErrorCode errcc);
	HTTP_MODULE_FINISHED_SIGNAL(personal_info);
	HTTP_MODULE_FINISHED_SIGNAL(mainwindow);
	HTTP_MODULE_FINISHED_SIGNAL(user_mgr);
	HTTP_MODULE_FINISHED_SIGNAL(dept_mgr);
	HTTP_MODULE_FINISHED_SIGNAL(menu_mgr);
	HTTP_MODULE_FINISHED_SIGNAL(post_mgr);
	HTTP_MODULE_FINISHED_SIGNAL(role_mgr);
	HTTP_MODULE_FINISHED_SIGNAL(auth_role_user_mgr);
private:
	bool m_isDebug{ true };
	bool m_enableResponseInterceptor{ true };
	RequestInterceptor m_requestInterceptor;
	ResponseInterceptor m_responseInterceptor;
public:
	void setRequestInterceptor(const RequestInterceptor& interceptor);
	void setResponseInterceptor(const ResponseInterceptor& interceptor);
	void enableResponseInterceptor(bool enable = true);
};


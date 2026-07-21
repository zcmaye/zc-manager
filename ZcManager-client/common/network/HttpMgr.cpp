#include "HttpMgr.h"
#include <QAuthenticator>
#include <QUrlQuery>
#include <QHttpMultipart>
#include <QFile>

#include <QSslConfiguration>
#include <QSslSocket>

#ifndef __EMSCRIPTEN__
static QSslConfiguration sslConfiguration() {
    //设置支持https请求
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    //设置tls协议自动协商
    config.setProtocol(QSsl::AnyProtocol);
    //设置不校验对端证书
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    return config;
}
#endif

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

QNetworkRequest HttpMgr::createRequest(const QUrl& url, const QVariantMap& queryParams, const QHttpHeaders& headers)
{
    QNetworkRequest request;

    QUrl _url = url;
    if (!queryParams.isEmpty()) {
        //设置查询参数
        QUrlQuery query;
        for (auto it = queryParams.begin();it != queryParams.end();++it) {
            query.addQueryItem(it.key(), it.value().toString());
        }
        _url.setQuery(query);
    }
    //设置URL
    request.setUrl(_url);

    //设置请求头
    request.setHeaders(headers);
    //if (!headers.contains(QHttpHeaders::WellKnownHeader::ContentType)) {
    //	//request.setRawHeader("Content-Type", "application/json; charset=utf-8");
    //	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    //}
    //配置SSL
#ifndef __EMSCRIPTEN__
    if (_url.url().startsWith("https")) {
        qDebug().noquote() << "[Ssl配置完成...]" << _url.url();

        request.setSslConfiguration(sslConfiguration());
    }
#endif
    if (m_requestInterceptor)
        m_requestInterceptor(request);
    return request;
}

void HttpMgr::handleFinish(const ReqId& req_id, QNetworkReply* reply, Modules mod)
{
    auto res = reply->readAll();
    if (m_responseInterceptor && m_enableResponseInterceptor) {
        //如果已经拦截了，直接返回
        if (m_responseInterceptor(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), res)) {
            reply->deleteLater();
            if (m_isDebug)
                qDebug().noquote() << "[结束]:" << reply->url() << " [拦截器已拦截]:" << res << " ==>" << reply->errorString();
            return;
        }
    }
    //出错
    if (reply->error() != QNetworkReply::NoError) {
        if (m_isDebug)
            qDebug().noquote() << "[结束]:" << reply->url() << " [失败]:" << res << " ==>" << reply->errorString();
        //发送信号，通知完成
        if (reply->error() == QNetworkReply::NetworkError::ConnectionRefusedError) {
            emit sig_http_finish(req_id, res.isEmpty() ? reply->errorString().toUtf8() : res, ErrorCode::NetworkError, mod);
        }
        else {
            emit sig_http_finish(req_id, res.isEmpty() ? reply->errorString().toUtf8() : res, ErrorCode::Error, mod);
        }
    }
    //无错误
    else {
        if (m_isDebug) {
            auto type = reply->header(QNetworkRequest::ContentTypeHeader);
            if (type == "application/json") {
                qDebug().noquote() << "[结束]:" << reply->url() << " [成功]:" << QString::fromUtf8(res);
            }
            else {
                auto len = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
                qDebug().noquote() << "[结束]:" << reply->url() << " [成功]: Type " << type.toString() << " Bytes " << len;
            }
        }
        //发送信号，通知完成
        emit sig_http_finish(req_id, res, ErrorCode::Success, mod);
    }
    reply->deleteLater();
}

void HttpMgr::setRequestInterceptor(const  RequestInterceptor& interceptor) {
    m_requestInterceptor = interceptor;
}

void HttpMgr::setResponseInterceptor(const ResponseInterceptor& interceptor) {
    m_responseInterceptor = interceptor;
}

void HttpMgr::enableResponseInterceptor(bool enable)
{
    m_enableResponseInterceptor = enable;
}

HttpMgr::~HttpMgr()
{

}

void HttpMgr::get( const QUrl& url, ReqId req_id, Modules mod, const QVariantMap& queryParams, const QHttpHeaders& headers)
{
    auto request = createRequest(url, queryParams, headers);
    if (m_isDebug)
        qDebug().noquote() << "[网址]:" << url.url() << request.url().query();

    auto reply = m_manager.get(request);
    QObject::connect(reply, &QNetworkReply::finished, [=,this]() {
        handleFinish(req_id, reply,mod);
    });
}

void HttpMgr::post(const QUrl& url,const json_t& json, ReqId req_id, Modules mod,  const QVariantMap& queryParams, const QHttpHeaders& headers)
{
    auto request = createRequest(url, queryParams, headers);

    auto data = json_to_QByteArray(json);
    if (!data.isEmpty()) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.size()));
        if (m_isDebug)
            qDebug().noquote() << "[网址]:" << url.url() << "[body]:" << json.dump();
    }
    else {
        if (m_isDebug)
            qDebug().noquote() << "[网址]:" << url.url();
    }

    auto reply = m_manager.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished, [=,this]() {
        handleFinish(req_id, reply,mod);
    });

}

void HttpMgr::put(const QUrl& url, const json_t& json, ReqId req_id, Modules mod, const QVariantMap& queryParams, const QHttpHeaders& headers)
{
    auto data = json_to_QByteArray(json);

    auto request = createRequest(url, queryParams, headers);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.size()));
    if (m_isDebug)
        qDebug().noquote() << "[网址]:" << url.url() << "[body]:" << json.dump();

    auto reply = m_manager.put(request, data);
    QObject::connect(reply, &QNetworkReply::finished, [=,this]() {
        handleFinish(req_id, reply,mod);
    });

}

void HttpMgr::del(const QUrl& url, ReqId req_id, Modules mod, const QVariantMap& queryParams, const QHttpHeaders& headers)
{
    auto request = createRequest(url, queryParams, headers);
    if (m_isDebug)
        qDebug().noquote() << "[网址]:" << request.url()  << request.url().query();

    auto reply = m_manager.deleteResource(request);
    QObject::connect(reply, &QNetworkReply::finished, [=,this]() {
        handleFinish(req_id, reply,mod);
    });
}

void HttpMgr::download(const QUrl& url, ReqId req_id, Modules mod, const QVariantMap& queryParams, const QHttpHeaders& headers)
{
    auto request = createRequest(url, queryParams, headers);
    if (m_isDebug)
        qDebug().noquote() << "[网址]:" << url.url() << request.url().query();

    auto reply = m_manager.get(request);

    QObject::connect(reply, &QNetworkReply::readyRead, [=,this]() {
        emit sig_http_ready_read(req_id, mod, reply->readAll());
    });
    QObject::connect(reply, &QNetworkReply::downloadProgress, [=,this](qint64 bytesReceived, qint64 bytesTotal) {
        emit sig_http_download_progress(req_id, mod, bytesReceived, bytesTotal);
    });
    QObject::connect(reply, &QNetworkReply::finished, [=,this]() {
        handleFinish(req_id, reply, mod);
    });
}

void HttpMgr::upload(const QUrl& url, ReqId req_id, QHttpMultiPart* multipart, Modules mod, const QVariantMap& queryParams, const QHttpHeaders& headers)
{
    auto request = createRequest(url, queryParams, headers);
    if (m_isDebug)
        qDebug().noquote() << "[网址]:" << url.url() << request.url().query();

    auto reply = m_manager.post(request, multipart);

    QObject::connect(reply, &QNetworkReply::uploadProgress, [=,this](qint64 bytesSent, qint64 bytesTotal) {
        emit sig_http_upload_progress(req_id, mod, bytesSent, bytesTotal);
    });
    QObject::connect(reply, &QNetworkReply::finished, [=,this]() {
        handleFinish(req_id, reply, mod);
        multipart->deleteLater();
    });
}

void HttpMgr::upload(const QUrl& url, ReqId req_id, const QStringList& filenames, Modules mod, const QVariantMap& queryParams, const QHttpHeaders& headers)
{
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->setBoundary("-----zcMaye_zheshiqtyellowriver-----");

    for (auto& filename : filenames) {
        //打开文件
        QFile* file = new QFile(filename, multiPart);
        if (!file->open(QIODevice::ReadOnly)) {
            if (m_isDebug)
                qDebug() << "打开文件失败:" << filename;
            multiPart->deleteLater();
            return;
        }

        QHttpPart part;
        part.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg(filename));
        part.setBodyDevice(file);
        multiPart->append(part);
    }

    upload(url, req_id, multiPart, mod, queryParams, headers);
}

void HttpMgr::upload(const QUrl& url, ReqId req_id, const QByteArray& data, Modules mod, bool isFormData, const QVariantMap& queryParams, const QHttpHeaders& headers)
{
    //https://www.cnblogs.com/anyux/p/18451560 区别

    //form-data
    if (isFormData) {
        //构造Part
        QHttpPart part;
        //part.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg("avatar.png"));
        part.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"file\"; filename=\"no-name\""));
        //part.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"file\"; filename=\"%1\"").arg("avatar.png"));
        part.setBody(data);

        QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        multiPart->setBoundary("-----zcMaye_zheshiqtyellowriver-----");
        multiPart->append(part);

        upload(url, req_id, multiPart, mod, queryParams, headers);
    }
    //binary
    else {
        auto request = createRequest(url, queryParams, headers);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        if (m_isDebug)
            qDebug().noquote() << "[网址]:" << url.url() << request.url().query();

        auto reply = m_manager.post(request, data);

        QObject::connect(reply, &QNetworkReply::uploadProgress, [=,this](qint64 bytesReceived, qint64 bytesTotal) {
            emit sig_http_upload_progress(req_id, mod, bytesReceived, bytesTotal);
        });
        QObject::connect(reply, &QNetworkReply::finished, [=,this]() {
            handleFinish(req_id, reply, mod);
        });
    }
}

void HttpMgr::slot_http_finish(ReqId req_id, const QByteArray& msg, ErrorCode errCode, Modules mod)
{
    switch (mod)
    {
    case Modules::LoginReg:
        emit sig_login_reg_mod_finish(req_id, msg, errCode);
        break;
    case Modules::PersonalInfo:
        emit sig_personal_info_mod_finish(req_id, msg, errCode);
        break;
    case Modules::MainWindow:
        emit sig_mainwindow_mod_finish(req_id, msg, errCode);
        break;
    case Modules::UserMgr:
        emit sig_user_mgr_mod_finish(req_id, msg, errCode);
        break;
    case Modules::DeptMgr:
        emit sig_dept_mgr_mod_finish(req_id, msg, errCode);
        break;
    case Modules::MenuMgr:
        emit sig_menu_mgr_mod_finish(req_id, msg, errCode);
        break;
    case Modules::PostMgr:
        emit sig_post_mgr_mod_finish(req_id, msg, errCode);
        break;
    case Modules::RoleMgr:
        emit sig_role_mgr_mod_finish(req_id, msg, errCode);
        break;
    case Modules::RoleAuthUserMgr:
        emit sig_auth_role_user_mgr_mod_finish(req_id, msg, errCode);
        break;
    default:
        break;
    }
}

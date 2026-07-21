#pragma once

/** 模块请求完成信号宏  */
#define HTTP_MODULE_FINISHED_SIGNAL(MODULE_NAME)\
    void sig_##MODULE_NAME##_mod_finish(ReqId id, const QByteArray& res, ErrorCode errc)


/**
 * 具体模块请求处理声明宏(头文件类中声明)
 */
#define HTTP_HANDLER_DECL(MODULE_NAME)\
private slots:\
	void slot_##MODULE_NAME##_mod_finished(ReqId req_id,const QByteArray& msg,ErrorCode errc);\
private:\
	QMap<ReqId, std::function<void(json_t)>> _handlers;\
	void initHandlers()

/**
 * 具体模块请求处理初始化宏(构造函数中调用).
 */
#define HTTP_HANDLER_INIT(MODULE_NAME)\
	initHandlers();\
	connect(HttpMgr::instance().get(), &HttpMgr::sig_##MODULE_NAME##_mod_finish, this,&std::remove_reference_t<decltype(*this)>::slot_##MODULE_NAME##_mod_finished)


/**
 * 具体模块请求处理函数定义(源文件最后实现).
 */
#define HTTP_HANDLER_IMPL(CLASS_NAME,MODULE_NAME)\
	void  CLASS_NAME::slot_##MODULE_NAME##_mod_finished(ReqId req_id,const QByteArray& msg,ErrorCode errc)


/**
 * 具体模块请求处理(在 HTTP_HANDLER_IMPL函数中使用).
 */
#define HTTP_HANDER_INSERT(REQ_ID)\
	_handlers.insert(REQ_ID, [this](const json_t& json)


/**
 * 文件下载进度
 */
#define HTTP_HANDLER_CONN_PROGRESS()\
	connect(HttpMgr::instance().get(), &HttpMgr::sig_http_download_progress, [=,this](ReqId req_id,Modules mod,qint64 bytesReceived, qint64 bytesTotal)

/**
 * 文件下载有数据可读回调
 */
#define HTTP_HANDLER_CONN_READY_READ()\
	connect(HttpMgr::instance().get(), &HttpMgr::sig_http_ready_read, [=,this](ReqId req_id,Modules mod, const QByteArray& msg)


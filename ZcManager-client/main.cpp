#include "MainWindow.h"
#include "common/utils/ExcelUtil.h"
#include <QtWidgets/QApplication>
#include <QFile>

#include "common/network/HttpMgr.h"
#include "ContextHolder.h"

class Register {
public:
    Register()
        : m_request_filter{"/login","/register"}
    {
        initRequestHandler();
    }

    //初始化请求拦截器
    void initRequestHandler() {
        HttpMgr::instance()->setRequestInterceptor([this](QNetworkRequest& request) {
            //跳过不需要验证的
            if (m_request_filter.contains(request.url().path())) {
                return;
            }
            //添加验证头
			QHttpHeaders headers(request.headers());
			headers.append(QHttpHeaders::WellKnownHeader::Authorization, "Bearer " + ContextHolder::instance()->token());

            request.setHeaders(headers);
            });
    }


private:
    QStringList m_request_filter;
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QFile file(":/Resource/qss/style.css");
    if (file.open(QIODevice::ReadOnly)) {
        app.setStyleSheet(file.readAll());
    }

    Register reg;

    MainWindow window;
    window.showMaximized();

	return app.exec();
}

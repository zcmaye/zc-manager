#pragma once

#include <QWidget>
#include "ui_XlsxUploadDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class XlsxUploadDlgClass; };
QT_END_NAMESPACE

class XlsxCard;

/**
 * Xlsx导入类.
 */
class XlsxUploadDlg : public QWidget
{
	Q_OBJECT

public:
	XlsxUploadDlg(QWidget *parent = nullptr);
	~XlsxUploadDlg();

	void setSupportMultiFile(bool supprt);
	bool supportMultiFile()const;


	bool isUpdateSupport()const;
public slots:
	void slot_open_file();
signals:
	void sig_download_tmeplate();
	void sig_upload(const QList<QString>& filenames);
protected:
	bool eventFilter(QObject* watched, QEvent* ev)override;
private:
	Ui::XlsxUploadDlgClass *ui;
	bool m_supportMultiFile;		/*!是否支持多文件*/

	QMap<QString,QString> m_filenames;
	void addFileName(const QString& filename);
	XlsxCard* createXlsxCard(const QString& name, QWidget* parent = nullptr);
	void doAddAnimated(const QString& name);
	void doCloseAnimated(XlsxCard* card);
	void doOpacityAnimated(XlsxCard* card,float start,float end);
};


/**
 * Xlsx导入文件展示卡片.
 */
class XlsxCard : public QWidget 
{
	Q_OBJECT
public:
	XlsxCard(QWidget* parent = nullptr);
	XlsxCard(const QString& text,QWidget* parent = nullptr);

	QString text()const;
protected:
	void paintEvent(QPaintEvent* ev)override;
	void resizeEvent(QResizeEvent* ev)override;
	void mouseReleaseEvent(QMouseEvent* ev)override;
signals:
	void sig_request_close(); 
private:
	QPixmap m_icon;
	QString m_text;
	QPixmap m_closeIcon;
	QRect   m_closeRect;
};

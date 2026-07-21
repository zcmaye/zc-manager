#include "XlsxUploadDlg.h"
#include <QEvent>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QStyle>
#include <QPropertyAnimation>

XlsxUploadDlg::XlsxUploadDlg(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::XlsxUploadDlgClass())
	, m_supportMultiFile(false)
{
	ui->setupUi(this);
	ui->dragArea->installEventFilter(this);
	ui->dowloadTemplateBtn->installEventFilter(this);
	connect(ui->dowloadTemplateBtn, &QPushButton::clicked, this, &XlsxUploadDlg::sig_download_tmeplate);
	connect(ui->okBtn, &QPushButton::clicked, this, [this] {
		emit sig_upload(m_filenames.values());
		});


	ui->dragArea->setAcceptDrops(true);
	auto vlayout = new QVBoxLayout(ui->xlsxCardList);
	vlayout->setContentsMargins(0, 0, 0, 0);
}

XlsxUploadDlg::~XlsxUploadDlg()
{
	delete ui;
}

void XlsxUploadDlg::setSupportMultiFile(bool supprt)
{
	m_supportMultiFile = supprt;
}

bool XlsxUploadDlg::supportMultiFile() const
{
	return m_supportMultiFile;
}

bool XlsxUploadDlg::isUpdateSupport() const
{
	return ui->autoUpdateDataChx->isChecked();
}

void XlsxUploadDlg::slot_open_file()
{
	if (!m_supportMultiFile) {
		auto filename = QFileDialog::getOpenFileName(this, "打开", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "自定义文件 (*.xlsx);;所有文件 (*.*)");
		if (filename.isEmpty()) {
			return;
		}
		addFileName(filename);
	}
	else {
		auto filenames = QFileDialog::getOpenFileNames(this, "打开", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "自定义文件 (*.xlsx);;所有文件 (*.*)");
		if (filenames.isEmpty()) {
			return;
		}
		for (auto& filename : filenames) {
			addFileName(filename);
		}
	}
}

bool XlsxUploadDlg::eventFilter(QObject* watched, QEvent* ev)
{
	if (watched == ui->dragArea) {
		if (ev->type() == QEvent::Enter) {
			setCursor(Qt::CursorShape::PointingHandCursor);
		}
		else if (ev->type() == QEvent::Leave) {
			setCursor(Qt::CursorShape::ArrowCursor);
		}
		else if (ev->type() == QEvent::MouseButtonRelease) {
			slot_open_file();
		}
		else if (ev->type() == QEvent::DragEnter) {
			ev->accept();
		}
		else if (ev->type() == QEvent::Drop) {
			auto dropEv = dynamic_cast<QDropEvent*>(ev);
			dropEv->acceptProposedAction();
			//获取拖拽的文件
			auto mimeData = dropEv->mimeData();
			if (mimeData->hasUrls()) {
				auto urls = mimeData->urls();
				if (!m_supportMultiFile) {
					auto& url = urls.first();
					addFileName(url.toLocalFile());
				}
				else {
					for (auto& url : urls) {
						addFileName(url.toLocalFile());
					}
				}
			}
		}
	}
	else if (watched == ui->dowloadTemplateBtn) {
		if (ev->type() == QEvent::Enter) {
			setCursor(Qt::CursorShape::PointingHandCursor);
		}
		else if (ev->type() == QEvent::Leave) {
			setCursor(Qt::CursorShape::ArrowCursor);
		}
	}
	return false;
}

void XlsxUploadDlg::addFileName(const QString& filename)
{
	if (!supportMultiFile() && !m_filenames.isEmpty()) {
		return;
	}
	//仅支持xlsx格式
	if (!filename.endsWith(".xlsx")) {
		return;
	}
	auto name = QFileInfo(filename).fileName();
	m_filenames.insert(name, filename);
	doAddAnimated(name);
	qDebug() << filename;
}

XlsxCard* XlsxUploadDlg::createXlsxCard(const QString& name, QWidget* parent)
{
	auto card = new XlsxCard(name, parent);
	card->setAttribute(Qt::WA_DeleteOnClose);
	card->move(ui->xlsxCardList->x() + 6, ui->xlsxCardList->y() - card->height());
	card->resize(ui->xlsxCardList->width(), card->height());
	connect(card, &XlsxCard::sig_request_close, [this,name,card] {
		m_filenames.remove(name);
		doCloseAnimated(card);
		});
	return card;
}

void XlsxUploadDlg::doAddAnimated(const QString& name) {
	auto card = createXlsxCard(name, this);
	card->show();
	//计算最终的位置
	auto layout = ui->xlsxCardList->layout();
	if (!layout)
		return;
	QPoint endPos = { card->x(),card->y() + card->height() * (layout->count()+1) };
	//做动画
	auto animation = new QPropertyAnimation(card, "pos", this);
	animation->setStartValue(card->pos());
	animation->setEndValue(endPos); 
	animation->setDuration(200);
	animation->start(QPropertyAnimation::DeleteWhenStopped);
	doOpacityAnimated(card, 0, 1);

	//连接结束信号
	connect(animation, &QPropertyAnimation::finished, this, [this,card] {
		auto layout = ui->xlsxCardList->layout();
		if (!layout)
			return;
		layout->addWidget(card);
		});

	//addXlsxCard(name);
}

void XlsxUploadDlg::doCloseAnimated(XlsxCard* card)
{
	//先把card从布局中拿出来
	auto layout = ui->xlsxCardList->layout();
	if (!layout)
		return;
	auto index = layout->indexOf(card);
	if (index == -1) {
		return;
	}
	auto item = layout->takeAt(index);
	delete item;

	card->setParent(this);
	card->move(ui->xlsxCardList->x(), ui->xlsxCardList->y() + card->height() * index);
	card->show();
	//计算最终的位置
	QPoint endPos = { card->x(),card->y() - card->height() };
	//做动画
	auto animation = new QPropertyAnimation(card, "pos", this);
	animation->setStartValue(card->pos());
	animation->setEndValue(endPos); 
	animation->setDuration(200);
	animation->start(QPropertyAnimation::DeleteWhenStopped);
	doOpacityAnimated(card, 1, 0);

	//连接结束信号
	connect(animation, &QPropertyAnimation::finished, this, [this,card] {
		card->close();
		});
}

void XlsxUploadDlg::doOpacityAnimated(XlsxCard* card, float start, float end)
{
	//做动画
	auto animation = new QVariantAnimation(this);
	animation->setStartValue(start);
	animation->setEndValue(end); 
	animation->setDuration(200);
	animation->start(QPropertyAnimation::DeleteWhenStopped);

	connect(animation, &QVariantAnimation::valueChanged, this, [this,card](const QVariant& v) {
		qDebug() << v.toFloat();
		card->setStyleSheet(QString("background-color:rgba(245, 247, 250,%1)").arg(v.toFloat() * 255));
		});
}


/** XlsxCard */
XlsxCard::XlsxCard(QWidget* parent)
	:XlsxCard({}, parent)
{
}

XlsxCard::XlsxCard(const QString& text, QWidget* parent)
	:QWidget(parent)
	,m_text(text)
{
	setAttribute(Qt::WA_StyledBackground);
	setFixedHeight(28);
	m_icon = style()->standardPixmap(QStyle::StandardPixmap::SP_FileIcon).scaled(QSize(16,16));
	m_closeIcon.load(":/Resource/icons/close.svg");
	m_closeIcon = m_closeIcon.scaled(QSize(16, 16));
	setContentsMargins(9, 9, 9, 9);
}

QString XlsxCard::text() const
{
	return m_text;
}

void XlsxCard::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::TextAntialiasing);

	int th = painter.fontMetrics().height();
	int tw = painter.fontMetrics().horizontalAdvance(m_text);
	auto margins = contentsMargins();
	//绘制icon
	painter.drawPixmap(margins.left(), (height() - m_icon.height()) / 2, m_icon);
	//绘制文本
	painter.drawText(margins.left() + m_icon.width() + 9, height() - (height() - th) / 2 - 3, m_text);
	//绘制关闭按钮
	painter.drawPixmap(m_closeRect, m_closeIcon);
}

void XlsxCard::resizeEvent(QResizeEvent* ev)
{
	auto margins = contentsMargins();
	m_closeRect = {width() - margins.right() - m_closeIcon.width(), (height() - m_closeIcon.height()) / 2,16,16};
}

void XlsxCard::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		if (m_closeRect.contains(ev->pos())) {
			emit sig_request_close();
		}
	}
}

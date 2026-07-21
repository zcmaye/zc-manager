#include "PaginationToolBar.h"
#include <QIntValidator>
#include <QLineEdit>
#include <QKeyEvent>
#include <QStandardItemModel>

PaginationToolBar::PaginationToolBar(QWidget* parent) 
    : QWidget(parent)
    , m_totalRecords(0)
    , m_page(1)
    , m_pageSize(10)
    , m_pageCount(0)
    , m_pageSizeList({10,20,30,50})
{
    initUi();
}


void PaginationToolBar::setPage(int page)
{
    if (page < 1)
        page = 1;
    if (page > m_pageCount)
        page = m_pageCount;

    if (m_page != page) {
        m_page = page;
        updateUI();
        emit pageChanged(m_page);
    }
}

void PaginationToolBar::setPageSize(int size)
{
	qDebug() << size << m_pageSize;
    if (m_pageSize == size) {
        return;
    }

	auto pos = m_pageSizeList.indexOf(size);
	if (pos == -1) {
		qWarning() << size << " The size is not in the pageSizelist.";
		return;
	}

	m_pageSize = size;
	m_pageSizeCombo->setCurrentIndex(pos);

	emit pageSizeChanged(m_pageSize);
	updateUI();
}

void PaginationToolBar::setPageCount(int count)
{
	m_pageCount = count;
	updateUI();
}

int PaginationToolBar::page() const
{
	return m_page;
}

int PaginationToolBar::pageSize() const
{
	return m_pageSize;
}

int PaginationToolBar::totalRecords() const
{
	return m_totalRecords;
}

void PaginationToolBar::setTotalRecords(int total)
{
	if (m_totalRecords != total) {
		m_totalRecords = total;
		updateUI();
	}
}

int PaginationToolBar::pageCount() const
{
	return m_pageCount;
}

void PaginationToolBar::setPageSizeList(const QList<int>& list)
{
	m_pageSizeList = list;
	m_pageSizeCombo->clear();
	for (int i = 0; i < m_pageSizeList.size(); i++) {
		auto pageSize = m_pageSizeList[i];
		m_pageSizeCombo->addItem(QString("%1条/页").arg(pageSize), pageSize);

		if (m_pageSize == pageSize)
			m_pageSizeCombo->setCurrentIndex(i);
	}
	auto model = dynamic_cast<QStandardItemModel*>( m_pageSizeCombo->model());
	for (int i = 0;i < model->rowCount();i++) {
		model->item(i, 0)->setTextAlignment(Qt::AlignCenter);
	}
}

const QList<int>& PaginationToolBar::pageSizeList() const
{
	return m_pageSizeList;
}

void PaginationToolBar::setMessage(const QString& msg)
{
	m_messageLabel->setText(msg);
}

QString PaginationToolBar::message() const
{
	return m_messageLabel->text();
}

bool PaginationToolBar::eventFilter(QObject* watched, QEvent* ev)
{
	if (watched == m_pageNumberEdit) {
		auto text = m_pageNumberEdit->text();
		if (ev->type() == QEvent::FocusOut) {
			if (text.isEmpty()) {
				m_pageNumberEdit->setText(QString::number(page()));
			}
		}
		else if (ev->type() == QEvent::KeyPress) {
			auto keyEv = dynamic_cast<QKeyEvent*>(ev);
			if (keyEv->key() == Qt::Key_Enter || keyEv->key() == Qt::Key_Return) {
				if (text.isEmpty()) {
					m_pageNumberEdit->setText(QString::number(page()));
				}
			}
		}
	}
	else if (watched == m_pageSizeCombo->lineEdit()) {
		if (ev->type() == QEvent::MouseButtonRelease && dynamic_cast<QMouseEvent*>(ev)->button() == Qt::LeftButton) {
			m_pageSizeCombo->showPopup();
		}
	}
	return false;
}

void PaginationToolBar::initUi()
{
	auto hlayout = new QHBoxLayout(this);

	m_messageLabel = new QLabel;
	hlayout->addWidget(m_messageLabel);
	hlayout->addStretch();

	m_totalRecordsLabel = new QLabel("共 0 条", this);
	hlayout->addWidget(m_totalRecordsLabel);

	m_pageSizeCombo = new QComboBox(this);
	m_pageSizeCombo->setEditable(true);
	m_pageSizeCombo->lineEdit()->setReadOnly(true);
	m_pageSizeCombo->lineEdit()->setAlignment(Qt::AlignCenter);
	m_pageSizeCombo->lineEdit()->installEventFilter(this);
	setPageSizeList(m_pageSizeList);
	hlayout->addWidget(m_pageSizeCombo);

	m_prevPageBtn = new QPushButton("<", this);
	hlayout->addWidget(m_prevPageBtn);
	{
		auto hPageLayout = new QHBoxLayout;
		m_morePrevBtn = new QPushButton;
		m_moreNextBtn = new QPushButton;
		for (int i = 0; i < m_blockSize; i++) {
			auto btn = new QPushButton(QString::number(i + 1));
			btn->setCheckable(true);
			btn->setAutoExclusive(true);
			connect(btn, &QPushButton::clicked, this, [this, btn] { setPage(btn->text().toInt()); });

			m_pageButtons.push_back(btn);
			hPageLayout->addWidget(btn);

			if (i == 0) {
				btn->setChecked(true);
				hPageLayout->addWidget(m_morePrevBtn);
			}
			else if (i == /*5*/ m_blockSize - 2)
				hPageLayout->addWidget(m_moreNextBtn);
		}
		hlayout->addLayout(hPageLayout);
	}
	m_nextPageBtn = new QPushButton(">", this);

	hlayout->addWidget(m_nextPageBtn);

	hlayout->addWidget(new QLabel("前往", this));

	m_pageNumberEdit = new QLineEdit(this);
	m_pageNumberEdit->setValidator(new QIntValidator(1, 9999));
	m_pageNumberEdit->setText("1");
	m_pageNumberEdit->setAlignment(Qt::AlignCenter);
	m_pageNumberEdit->installEventFilter(this);
	hlayout->addWidget(m_pageNumberEdit);

	hlayout->addWidget(new QLabel("页", this));

	connect(m_morePrevBtn, &QPushButton::clicked, this, [this] {
		//setPage(page() - 5);
		setPage(page() - (m_blockSize - 2));
		});
	connect(m_moreNextBtn, &QPushButton::clicked, this, [this] {
		//setPage(page() + 5);
		setPage(page() + (m_blockSize - 2));
		});
	connect(m_prevPageBtn, &QPushButton::clicked, this, [this] {
		setPage(page() - 1);
		});
	connect(m_nextPageBtn, &QPushButton::clicked, this, [this] {
		setPage(page() + 1);
		});
	connect(m_pageSizeCombo, &QComboBox::currentIndexChanged, this, [this] {
		setPageSize(m_pageSizeCombo->currentData().toInt());
		});
	connect(m_pageNumberEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
		setPage(text.toInt());
		});
	connect(m_pageNumberEdit, &QLineEdit::returnPressed, this, [this]() {
		auto pageStr = m_pageNumberEdit->text();
		setPage(pageStr.toInt());
		qDebug() << pageStr;
		m_pageNumberEdit->clearFocus();
        });
	connect(this, &PaginationToolBar::pageChanged, [this](int page) {
        for (auto btn : m_pageButtons) {
            if (btn->text().toInt() == page) {
                btn->setChecked(true);
				break;
            }
		}
        });

	m_morePrevBtn->setStyleSheet("QPushButton{icon:url(:/Resource/icons/more.svg);}QPushButton:hover{icon:url(:/Resource/icons/d-arrow-left-hover.svg);}");
	m_moreNextBtn->setStyleSheet("QPushButton{icon:url(:/Resource/icons/more.svg);}QPushButton:hover{icon:url(:/Resource/icons/d-arrow-right-hover.svg);}");
}

void PaginationToolBar::updateUI()
{
    //计算总页数
	m_pageCount = m_totalRecords / m_pageSize + (m_totalRecords % m_pageSize ? 1 : 0);
    //校准页码
    if (m_page > m_pageCount) {
        m_page = 1;
    }

    //更新总记录数
    m_totalRecordsLabel->setText(QString("共 %1 条").arg(m_totalRecords));
    //更新当前页码
	auto validator = static_cast<QIntValidator*>(const_cast<QValidator*>(m_pageNumberEdit->validator()));
    validator->setTop(m_pageCount);
    m_pageNumberEdit->setText(QString::number(m_page));
    //更新上一页/下一页按钮状态
    m_prevPageBtn->setEnabled(m_page > 1);
	m_nextPageBtn->setEnabled(m_page < m_pageCount);

    updatePageButtons();
}

void PaginationToolBar::updatePageButtons()
{
	//总页数小于等于7
	if (m_pageCount <= m_blockSize) {
		m_morePrevBtn->hide();
		m_moreNextBtn->hide();
		for (int i = 0; i < m_pageButtons.size(); i++) {
			if (i < m_pageCount) {
				m_pageButtons[i]->setText(QString::number(i + 1));
				m_pageButtons[i]->show();
			}
			else {
				m_pageButtons[i]->hide();
			}
		}
	}
	else {
		//第一个和最后一个显示固定的
		m_pageButtons.first()->show();
		m_pageButtons.first()->setText(QString::number(1));
		m_pageButtons.last()->show();
		m_pageButtons.last()->setText(QString::number(m_pageCount));
		m_moreNextBtn->show();

		int leftBound = m_blockSize / 2 + 1;
		int rightBound = m_pageCount - m_blockSize / 2;

		//当前页小于4，不需要显示上一个更多
		if (/*m_page < 4*/ m_page < leftBound) {
			m_morePrevBtn->hide();
			m_moreNextBtn->show();
			//跟新按钮的序号
			for (int i = 1; i < m_pageButtons.size() - 1; i++) {
				m_pageButtons[i]->setText(QString::number(i + 1));
			}
		}
		//当前页大于等于最大页码减3，不需要显示下一个更多
		else if (/*m_page > m_pageCount - 3*/ m_page > rightBound) {
			m_morePrevBtn->show();
			m_moreNextBtn->hide();
			//更新按钮的序号
			int n = 0;
			for (int i = m_pageButtons.size() - 1; i > 0; i--,n++) {
				m_pageButtons[i]->setText(QString::number(m_pageCount - n));
			}
		}
		//否则页码在中间currentpage in [4,pageCount-3]
		else {
			//刚好在左边界
			if (m_page == leftBound) {
				m_morePrevBtn->hide();
				m_moreNextBtn->show();
			}
			//刚好在右边界
			else if (m_page == rightBound) {
				m_morePrevBtn->show();
				m_moreNextBtn->hide();
			}
			//在中间
			else {
				m_morePrevBtn->show();
				m_moreNextBtn->show();
			}
			//跟新按钮序号
			for (int i = 1; i < m_pageButtons.size() - 1; i++) {
				//m_pageButtons[i]->setText(QString::number(m_page - 2 + i - 1));
				//5 6 7 8 9
				m_pageButtons[i]->setText(QString::number(m_page - (m_blockSize - 2) / 2 + i - 1));
			}
		}
	}

	//更新一下当前选择的按钮
	//for (auto btn : m_pageButtons) {
	//	if (btn->text().toInt() == m_page) {
	//		btn->setChecked(true);
	//		break;
	//	}
	//}
}

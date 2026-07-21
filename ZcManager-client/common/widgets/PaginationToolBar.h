#pragma once

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QHBoxLayout>

class PaginationToolBar : public QWidget
{
    Q_OBJECT
public:
    explicit PaginationToolBar(QWidget* parent = nullptr);

    /**
     * 获取当前页码.
     */
    int page() const;
    void setPage(int page);

    /**
     * 页大小(每页显示的记录数).
     */
    int pageSize() const;
    void setPageSize(int size);

    /**
     * 总记录数.
     */
    int totalRecords() const;
    void setTotalRecords(int total);

    /**
     * 总页数.
     */
    int pageCount() const;
    void setPageCount(int count);

    /**
     * 设置在下拉框中显示的每页大小.
     */
	void setPageSizeList(const QList<int>& list);
	const QList<int>& pageSizeList()const;

    void setMessage(const QString& msg);
    QString message()const;
protected:
    bool eventFilter(QObject* watched, QEvent* ev)override;
signals:
    /**
     * 当前页码改变.
     */
    void pageChanged(int page);
    /**
     * 页大小改变.
     */
    void pageSizeChanged(int size);
private:
    void initUi();
    void updateUI();
    void updatePageButtons();

    QLabel* m_messageLabel;
    QLabel* m_totalRecordsLabel;
    QComboBox* m_pageSizeCombo;
    QPushButton* m_prevPageBtn;
    QPushButton* m_nextPageBtn;
    QLineEdit* m_pageNumberEdit;

    int m_totalRecords;     /*!总记录数*/
    int m_page;      /*!当前页码*/
    int m_pageSize;         /*!页大小*/
    int m_pageCount;        /*!总页数*/
    QList<int> m_pageSizeList;    /*!用于下拉框选择的页大小列表*/

    int m_blockSize{ 7 };            /*!中间按钮个数，一定要奇数*/
    QList<QPushButton*> m_pageButtons;
    QPushButton* m_morePrevBtn{};   /*!更多按钮*/
    QPushButton* m_moreNextBtn{};   /*!更多按钮*/
};

#pragma once

#include <QLayout>
#include <QList>
#include <QSize>

class FlowLayout : public QLayout {
    Q_OBJECT
public:
    explicit FlowLayout(QWidget* parent = nullptr, int margin = 9, int hSpacing = 6, int vSpacing = 6);
    explicit FlowLayout(int margin, int hSpacing, int vSpacing);
    ~FlowLayout() override;

    void addLayout(QLayout* layout);
    void addItem(QLayoutItem* item) override;
    void insertItem(int index, QLayoutItem* item);

    QSize sizeHint() const override;
    QSize minimumSize() const override;

    // 设置间距和对齐方式
    void setHorizontalSpacing(int spacing);
    int horizontalSpacing()const;

    void setVerticalSpacing(int spacing);
    int verticalSpacing()const;

    /**
     * 设置布局对其方式.
     * @param alignment 只支持Qt::AlignLeft Qt::AlignHCenter Qt::AlignRight 三种
     */
    void setLayoutAlignment(Qt::Alignment alignment);
    Qt::Alignment layoutAlignment()const;

    Qt::Orientations expandingDirections() const override;

    void setVerticalCenter(bool center = true);
    bool verticalCenter()const;

    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;

protected:
    //void setGeometry(const QRect& rect) override;
    void setGeometry(const QRect& rect) override;
private:
    QList<QLayoutItem*> m_itemList;
    int m_hSpacing;
    int m_vSpacing;
    Qt::Alignment m_alignment;
    bool m_verticalCenter;

    int doLayout(const QRect& rect,bool isTest = false)const;
};

#pragma once

#include <QHeaderView>

class CheckBoxHeaderView  : public QHeaderView
{
	Q_OBJECT

public:
	CheckBoxHeaderView(QWidget *parent = nullptr);
	~CheckBoxHeaderView();

	void setCheckState(Qt::CheckState checkState);
	Qt::CheckState checkState()const;
protected:
	void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override;
	void mousePressEvent(QMouseEvent* ev)override;
signals:
	void checkStateChanged(Qt::CheckState state);
private:
	Qt::CheckState m_checkState;
	QSize m_size;
	int m_checkBoxIndex;		/*!选择框所在的列索引*/
};


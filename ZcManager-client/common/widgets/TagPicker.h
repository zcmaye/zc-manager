#pragma once

#include <QWidget>

class QListView;
class QStandardItemModel;
class FlowLayout;
class Tag;

class TagPicker  : public QWidget
{
	Q_OBJECT

public:
	TagPicker(QWidget *parent = nullptr);
	~TagPicker();

	void addItem(const QString& text, const QVariant& userData = QVariant());
	void addItem(const QIcon& icon, const QString& text, const QVariant& userData = QVariant());
	void addItems(const QStringList& texts);

	QStandardItemModel* model()const;

	/**
	 * 能选择的tag数量.
	 */
	int count() const;

	void clear();

	/**
	 * 是否有tag被选中
	 */
	bool hasTags()const;
	/**
	 * 所有选择的tag的数据.
	 */
	QVariantList tagDatas(int role = Qt::UserRole) const;
	/**
	 * 所有选择的tag的行.
	 */
	QList<int> tagIndexes() const;
	/**
	 * 所有选择的tag的.文本
	 */
	QStringList tagTexts() const;

	/**
	 * 选择tag.
	 */
	bool selectTag(const QString& text);
	bool selectTag(int row);

	void clearSelectTags();

	int findData(const QVariant& data, int role = Qt::UserRole, Qt::MatchFlags flags = static_cast<Qt::MatchFlags>(Qt::MatchExactly | Qt::MatchCaseSensitive)) const;
	int findText(const QString& text, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive) const;

	void setPlaceHolderText(const QString& text);
	QString placeHolderText()const;
public slots:
	void setIndexCheckState(const QModelIndex& index,Qt::CheckState checkState);
	void slotTagCloseRequest(const QString& text);
signals:
	void tagClicked(const QModelIndex& index);
	void tagChanged();
protected:
	void paintEvent(QPaintEvent* ev)override;
	void mouseReleaseEvent(QMouseEvent* ev)override;
	void resizeEvent(QResizeEvent* ev)override;
	bool eventFilter(QObject* watched, QEvent* ev)override;

	void showPopup();
	void hidePopup();
	bool isPopup()const;
private:
	QString m_placeHolderText;
	QListView* m_listView;
	QStandardItemModel* m_model;
	FlowLayout* m_flayout;
	QMap<int, Tag*> m_tags;		/*!所有选中的tag以及对应的行*/

	//根据item来计算listview的高度
	int hintHeight()const;
};


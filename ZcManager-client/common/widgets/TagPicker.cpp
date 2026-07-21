#include "TagPicker.h"
#include "common/widgets/FlowLayout.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QListView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

/**
 * 委托.
 */
class TagPickerDelegate : public QStyledItemDelegate
{
public:
	TagPickerDelegate(QObject* parent = nullptr) {
	}
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		auto text = index.data(Qt::DisplayRole).toString();

		int x = option.rect.x();
		int y = option.rect.y();
		int w = option.rect.width();
		int h = option.rect.height();

		painter->save();
		//鼠标悬停背景颜色
		if (option.state.testFlag(QStyle::State_MouseOver)) {
			painter->fillRect(option.rect, QColor("#f5f7fa"));
			auto listView = dynamic_cast<QListView*>(parent());
			if (listView) {
				listView->viewport()->update();
			}
		}
		//绘制文本
		if (index.data(Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked) {
			painter->setPen(QColor("#1890ff"));
			QPixmap icon(":/Resource/icons/tag-picker-checked.svg");
			painter->drawPixmap(QPoint(x + w - 24 - icon.width(), y + (h - icon.height()) / 2), icon);
		}
		else {
			painter->setPen(QColor("#606266"));
		}
		int th = painter->fontMetrics().height();
		painter->drawText(QPoint(x + 24, y + (h - th) + 6), text);
		painter->restore();
	}

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
		return QSize(option.rect.width(),38);
	}
private:
};

/**
 * Tag.
 */
class Tag : public QWidget {
	Q_OBJECT
public:
	Tag(const QString& text, QWidget* parent = nullptr)
		:m_text(text)
	{
		setMouseTracking(true);
	}
	QString text()const { return m_text; }
signals:
	void closeRequest(const QString& text);
protected:
	void paintEvent(QPaintEvent* ev)override {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		//绘制背景
		painter.setBrush(QColor("#f4f4f5"));
		painter.setPen(Qt::NoPen);
		painter.drawRoundedRect(rect(), 3, 3);
		//绘制文本
		int th = painter.fontMetrics().height();
		painter.setPen(QColor("#909399"));
		painter.drawText(QPoint(12, (height() - th) / 2 + th / 2 + 4), m_text);
		//绘制关闭按钮
		if (m_closeRect.contains(m_mousePos)) {
			painter.drawPixmap(m_closeRect, QPixmap(":/Resource/icons/tag-picker-tag-close-hover.svg"));
		}
		else {
			painter.drawPixmap(m_closeRect, QPixmap(":/Resource/icons/tag-picker-tag-close.svg"));
		}
	}
	QSize sizeHint()const override {
		int tw = fontMetrics().horizontalAdvance(m_text);
		int th = fontMetrics().height();
		return QSize(tw + 12 + 26, th + 12);
	}
	void resizeEvent(QResizeEvent* ev)override {
		m_closeRect = QRect{ width() - 6 - 16,(height() - 16) / 2,16,16 };
	}
	void mouseMoveEvent(QMouseEvent*ev)override {
		m_mousePos = ev->pos();
		update();
	}
	void mousePressEvent(QMouseEvent* ev)override {
		if (ev->button() == Qt::LeftButton) {
			if (m_closeRect.contains(ev->pos())) {
				emit closeRequest(m_text);
				ev->accept();
			}
		}
	}
	void mouseReleaseEvent(QMouseEvent* ev)override {
		if (ev->button() == Qt::LeftButton) {
			if (m_closeRect.contains(ev->pos())) {
				return;
			}
		}
		QWidget::mouseReleaseEvent(ev);
	}

private:
	QString m_text;
	mutable QRect m_closeRect;
	QPoint m_mousePos;
};


TagPicker::TagPicker(QWidget *parent)
	: QWidget(parent)
	, m_listView(new QListView(this))
	, m_model(new QStandardItemModel(this))
{
	setAttribute(Qt::WA_StyledBackground);
	m_listView->setWindowFlag(Qt::Popup);
	m_listView->setEditTriggers(QListView::NoEditTriggers);
	m_listView->installEventFilter(this);
	m_listView->setModel(m_model);
	m_listView->setItemDelegateForColumn(0, new TagPickerDelegate(m_listView));
	connect(m_listView, &QListView::clicked, this, &TagPicker::tagClicked);
	connect(this, &TagPicker::tagClicked, this, [this](const QModelIndex& index) {
		//切换选择状态
		auto checkState = index.data(Qt::CheckStateRole).value<Qt::CheckState>();
		auto newCheckState = (checkState == Qt::Checked ? Qt::Unchecked : Qt::Checked);
		setIndexCheckState(index, newCheckState);
		});

	auto container = new QWidget;
	container->setObjectName("TagContainer");
	m_flayout = new FlowLayout(container);
	m_flayout->setContentsMargins(0, 0, 0, 0);

	auto dropDownBtn = new QPushButton;
	dropDownBtn->setObjectName("DropDown");
	dropDownBtn->setMaximumSize(32, 32);
	dropDownBtn->setCheckable(true);
	connect(dropDownBtn, &QPushButton::clicked, this, [this] {
		if (isPopup()) {
			hidePopup();
		}
		else {
			showPopup();
		}
		});

	auto hlayout = new QHBoxLayout(this);
	hlayout->setSpacing(0);
	hlayout->addWidget(container);
	hlayout->addWidget(dropDownBtn);
}

TagPicker::~TagPicker()
{
}

void TagPicker::addItem(const QString& text, const QVariant& userData)
{
	addItem(QIcon(), text, userData);
}

void TagPicker::addItem(const QIcon& icon, const QString& text, const QVariant& userData)
{
	auto item = new QStandardItem(text);
	item->setData(userData, Qt::UserRole);
	item->setIcon(icon);
	m_model->appendRow(item);
	item->setCheckable(true);

}

void TagPicker::addItems(const QStringList& texts)
{
	for (auto& text : texts) {
		addItem(text);
	}
}

QStandardItemModel* TagPicker::model() const
{
	return m_model;
}

int TagPicker::count() const
{
	return m_model->rowCount();
}

void TagPicker::clear()
{
	m_model->clear();
}

bool TagPicker::hasTags() const
{
	return !m_tags.isEmpty();
}

QVariantList TagPicker::tagDatas(int role) const
{
	QVariantList list;
	for (auto it = m_tags.cbegin(); it != m_tags.cend(); it++) {
		auto index = m_model->index(it.key(), 0);
		list.append(m_model->data(index, role));
	}
	return list;
}

QList<int> TagPicker::tagIndexes() const
{
	return m_tags.keys();
}

QStringList TagPicker::tagTexts() const
{
	QStringList list;
	for (auto it = m_tags.cbegin(); it != m_tags.cend(); it++) {
		list.append(it.value()->text());
	}
	return list;
}

bool TagPicker::selectTag(const QString& text)
{
	return selectTag(findText(text));
}

bool TagPicker::selectTag(int row)
{
	if (row == -1)
		return false;
	auto item = m_model->item(row);
	setIndexCheckState(item->index(), Qt::Checked);
	return true;
}

void TagPicker::clearSelectTags()
{
	for (auto it = m_tags.cbegin(); it != m_tags.cend(); it++) {
		auto index = m_model->index(it.key(), 0);
		setIndexCheckState(index, Qt::Unchecked);
		it.value()->deleteLater();
	}
	m_tags.clear();
}

int TagPicker::findData(const QVariant& data, int role, Qt::MatchFlags flags) const
{
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i);
		if (!item)
			continue;
		if (item->data(role) == data) {
			return item->index().row();
		}
	}
	return -1;
}

int TagPicker::findText(const QString& text, Qt::MatchFlags flags) const
{
	auto items = m_model->findItems(text, flags);
	if (items.isEmpty()) {
		return -1;
	}
	return items.first()->index().row();
}

void TagPicker::setPlaceHolderText(const QString& text)
{
	m_placeHolderText = text;
}

QString TagPicker::placeHolderText() const
{
	return m_placeHolderText;
}

void TagPicker::slotTagCloseRequest(const QString& text)
{
	//根据文本找到索引
	auto items = m_model->findItems(text);
	if (items.isEmpty()) {
		qWarning() << "not find item " << text;
		return;
	}
	//找到了
	auto item = items.first();
	//删除tag
	auto index = item->index();
	auto it = m_tags.find(index.row());
	if (it != m_tags.end()) {
		it.value()->deleteLater();
		m_tags.remove(it.key());
	}
	//取消选中
	item->setCheckState(Qt::Unchecked);
	emit tagChanged();
}

void TagPicker::setIndexCheckState(const QModelIndex& index, Qt::CheckState checkState)
{
	auto text = index.data(Qt::DisplayRole).toString();
	//把标签添加到展示框中
	if (checkState == Qt::Checked) {
		auto tag = new Tag(text);
		m_flayout->addWidget(tag);
		m_tags.insert(index.row(), tag);
		connect(tag, &Tag::closeRequest, this, &TagPicker::slotTagCloseRequest);
	}
	//把标签从展示框中移除掉
	else {
		auto it = m_tags.find(index.row());
		if (it != m_tags.end()) {
			it.value()->deleteLater();
			m_tags.remove(it.key());
		}
	}
	//根据tag数量调整Picker高度
	int w = width();
	auto con = findChild<QWidget*>("TagContainer");
	if (con) {
		w = con->width();
	}
	auto margins = layout()->contentsMargins();
	setFixedHeight(m_flayout->heightForWidth(w + 6) + margins.top() + margins.bottom());
	//切换选择状态
	m_model->setData(index, checkState, Qt::CheckStateRole);
	update();
	emit tagChanged();
}

void TagPicker::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	//绘制占位文本
	if (!m_placeHolderText.isEmpty() && m_tags.isEmpty()) {
		auto tw = painter.fontMetrics().horizontalAdvance(m_placeHolderText);
		auto th = painter.fontMetrics().height();
		painter.setPen(QColor("#c0c4cc"));
		painter.drawText(QPoint(9, (height() - th) / 2 + th / 2 + 7), m_placeHolderText);
	}
}

void TagPicker::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		setFocus();
		if (isPopup()) {
			hidePopup();
		}
		else {
			showPopup();
		}
	}
}

void TagPicker::resizeEvent(QResizeEvent* ev)
{
	auto glaobalPos = this->mapToGlobal(QPoint(0, height() + 6));
	m_listView->move(glaobalPos);
	m_listView->setFixedWidth(width());
}

bool TagPicker::eventFilter(QObject* watched, QEvent* ev)
{
	if (watched == m_listView) {
		if (ev->type() == QEvent::MouseButtonRelease) {
			auto mev = dynamic_cast<QMouseEvent*>(ev);
			if (!m_listView->geometry().contains(mev->globalPos())) {
				hidePopup();
			}
		}
	}
	return false;
}

void TagPicker::showPopup() {

	auto glaobalPos = this->mapToGlobal(QPoint(0, height() + 6));
	m_listView->move(glaobalPos);
	m_listView->setFixedWidth(width());
	m_listView->setFixedHeight(hintHeight());

	m_listView->show();
	m_listView->setFocus();
	auto dropDown = findChild<QPushButton*>("DropDown");
	dropDown->setChecked(true);
}

void TagPicker::hidePopup() {
	m_listView->hide();
	auto dropDown = findChild<QPushButton*>("DropDown");
	dropDown->setChecked(false);
}

bool TagPicker::isPopup() const
{
	return m_listView->isVisible();
}

int TagPicker::hintHeight() const
{
	QSize size;
	auto delegate = m_listView->itemDelegateForColumn(0);

	int height = 0;
	for (int i = 0; i < m_model->rowCount(); i++) {
		auto item = m_model->item(i, 0);
		auto index = m_model->index(i, 0);
		if (delegate) {
			size = delegate->sizeHint(QStyleOptionViewItem(),index);
		}
		else {
			size = item->sizeHint();
			if (size.height() == -1)
				size.setHeight(38);
		}
		height += size.height();
	}
	return height + 20;
}


#include "TagPicker.moc"

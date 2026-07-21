#include "IconSelectView.h"
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QListView>
#include <QVBoxLayout>
#include <QDir>

IconSelectView::IconSelectView(QWidget *parent)
	: QWidget(parent)
	, m_model(new QStandardItemModel(this))
	, m_proxyModel(new QSortFilterProxyModel(this))
	, m_searchEdit(new QLineEdit)
	, m_view(new QListView)
{
	m_view->setViewMode(QListView::IconMode);
	m_view->setResizeMode(QListView::ResizeMode::Adjust);
	m_view->setFrameShape(QFrame::NoFrame);
	m_view->setFixedSize(470, 288);

	m_proxyModel->setSourceModel(m_model);
	m_view->setModel(m_proxyModel);

	m_searchEdit->setPlaceholderText("请输入图标名称");
	m_iconAct = m_searchEdit->addAction(QIcon(":/Resource/icons/search-gray.svg"), QLineEdit::LeadingPosition);
	m_searchEdit->setClearButtonEnabled(true);
	connect(m_searchEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
		m_proxyModel->setFilterWildcard(text);
		});
	connect(m_view, &QListView::clicked, this, &IconSelectView::slot_clicked);

	auto vlayout = new QVBoxLayout(this);
	vlayout->addWidget(m_searchEdit);
	vlayout->addWidget(m_view);
}

IconSelectView::~IconSelectView()
{}

void IconSelectView::addIcon(const QString & name, const QIcon & icon)
{
	auto item = new QStandardItem(name);
	item->setIcon(icon);
	item->setSizeHint(QSize(90,50));
	m_model->appendRow(item);
}

void IconSelectView::addIconPath(const QString& path)
{
	QDir dir(path);
	if (!dir.exists()) {
		qWarning() << "icon path " << path << " is not exists!";
		return;
	}

	for(auto& name : dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
		addIcon(QFileInfo(name).baseName(), QIcon(QString("%1/%2").arg(path).arg(name)));
	}
}

QVariant IconSelectView::currentData(int role) const
{
	return m_model->data(m_view->currentIndex(), role);
}

QModelIndex IconSelectView::currentIndex() const
{
	return m_view->currentIndex();
}

void IconSelectView::setCurrentIndex(const QModelIndex& index) 
{
	auto idx =  m_proxyModel->mapFromSource(index);
	m_view->setCurrentIndex(idx);
	slot_clicked(index);
}

static QModelIndex recursionFn(QStandardItem* item,const QVariant& data, const std::function<bool(QVariant, QVariant)>& fun, int role)
{
	for (int i = 0; i < item->rowCount(); i++) {
		auto child = item->child(i);
		if (fun(child->data(role), data)) {
			return child->index();
		}
		auto index = recursionFn(child, data, fun, role);
		if (index.isValid()) {
			return index;
		}
	}
	return QModelIndex();
}


QModelIndex IconSelectView::findData(const QVariant& data, const std::function<bool(QVariant, QVariant)>& fun, int role) const
{
	auto m = m_model;
	if (!m) {
		qWarning() << "model is nullptr";
		return {};
	}

	for (int i = 0; i < m->rowCount(); i++) {
		auto index =  m->index(i, 0);
		if (fun(index.data(role), data)) {
			return index;
		}
		index = recursionFn(m->itemFromIndex(index),data,fun,role);
		if (index.isValid()) {
			return index;
		}
	}
	return QModelIndex();
}

QModelIndex IconSelectView::findText(const QString& text, Qt::MatchFlags flags) const
{
	auto m = m_model;
	if (!m) {
		qWarning() << "model is nullptr";
		return {};
	}

	auto items = m->findItems(text, flags);
	if (items.isEmpty()) {
		return QModelIndex();
	}

	return items.first()->index();
}



void IconSelectView::slot_clicked(const QModelIndex& index)
{
	auto name = index.data(Qt::DisplayRole).toString();
	auto icon = index.data(Qt::DecorationRole).value<QIcon>();
	emit clicked(index);
	emit iconChanged(name, icon);
}


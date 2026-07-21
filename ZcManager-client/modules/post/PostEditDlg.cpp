#include "PostEditDlg.h"
#include "common/notify/NotifyTipManager.h"
#include "common/network/HttpMgr.h"

#include "domain/entity/User.hpp"
#include "domain/entity/Post.hpp"

#include "ContextHolder.h"


PostEditDlg::PostEditDlg(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::PostEditDlgClass())
{
	ui->setupUi(this);
}

PostEditDlg::~PostEditDlg()
{
	delete ui;
}

void PostEditDlg::clear()
{
	ui->postNameEdit->clear();
	ui->postCodeEdit->clear();
	ui->postSortBox->clear();
	ui->remarkEdit->clear();
}

void PostEditDlg::setPost(const std::shared_ptr<Post>& post)
{
	m_post = post;
	if (!m_post) {
		qWarning() << "post is nullptr";
		return;
	}

	ui->postNameEdit->setText(post->post_name);
	ui->postCodeEdit->setText(post->post_code);
	ui->postSortBox->setValue(post->post_sort);
	ui->remarkEdit->setPlainText(post->remark);
	if (m_post->is_active)
		ui->isActive1_RBtn->setChecked(true);
	else
		ui->isActive0_RBtn->setChecked(true);
}


void PostEditDlg::on_okBtn_clicked()
{
	auto postName = ui->postNameEdit->text().trimmed();
	auto postCode = ui->postCodeEdit->text().trimmed();
	auto postSort = ui->postSortBox->value();
	auto status = ui->isActive1_RBtn->isChecked();
	auto remark = ui->remarkEdit->toPlainText().trimmed();

	if (postName.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("岗位名称不能为空~");
		return;
	}

	if (postCode.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("岗位权限不能为空~");
		return;
	}

	if (postSort <= 0) {
		NotifyTipManager::instance()->addNotifyTip("岗位排序不能小于等于0~");
		return;
	}

	QJsonObject jpost;
	jpost["post_id"] = m_post->post_id;
	jpost["post_name"] = postName;
	jpost["post_code"] = postCode;
	jpost["post_sort"] = postSort;
	jpost["is_active"] = status;
	jpost["remark"] = remark;
	jpost["create_by"] = ContextHolder::instance()->self()->user_name;


	HttpMgr::instance()->put(URL("/post/edit"), jpost,ReqId::post_edit,Modules::PostMgr);
}

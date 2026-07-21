#include "PostAddDlg.h"
#include "common/notify/NotifyTipManager.h"
#include "common/network/HttpMgr.h"

#include "domain/entity/User.hpp"
#include "domain/entity/Post.hpp"

#include "ContextHolder.h"


PostAddDlg::PostAddDlg(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::PostAddDlgClass())
{
	ui->setupUi(this);
}

PostAddDlg::~PostAddDlg()
{
	delete ui;
}

void PostAddDlg::clear()
{
	ui->postNameEdit->clear();
	ui->postCodeEdit->clear();
	ui->postSortBox->clear();
	ui->remarkEdit->clear();
}


void PostAddDlg::on_okBtn_clicked()
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
	jpost["post_name"] = postName;
	jpost["post_code"] = postCode;
	jpost["post_sort"] = postSort;
	jpost["is_active"] = status;
	jpost["remark"] = remark;
	jpost["create_by"] = ContextHolder::instance()->self()->user_name;

	HttpMgr::instance()->post(URL("/post/add"), jpost,ReqId::post_add,Modules::PostMgr);
}

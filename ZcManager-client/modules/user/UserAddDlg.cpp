#include "UserAddDlg.h"
#include "common/notify/NotifyTipManager.h"
#include "common/network/HttpMgr.h"

#include "domain/entity/User.hpp"
#include "domain/entity/Post.hpp"
#include "domain/entity/Role.hpp"
#include "domain/vo/TreeSelect.hpp"

#include "ContextHolder.h"

static void setComboBoxEditPlaceHolderText(QComboBox* cmb) {
	if (!cmb->lineEdit())
		return;
	cmb->lineEdit()->setReadOnly(true);
	cmb->lineEdit()->setPlaceholderText(cmb->placeholderText());
}

UserAddDlg::UserAddDlg(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::UserAddDlgClass())
{
	ui->setupUi(this);

	ui->sexCmb->addItem("女", 0);
	ui->sexCmb->addItem("男", 1);
	ui->sexCmb->addItem("未知", 2);
	setComboBoxEditPlaceHolderText(ui->sexCmb);
	setComboBoxEditPlaceHolderText(ui->deptCmb);

	ui->postTagPicker->setPlaceHolderText("请选择岗位");
	ui->roleTagPicker->setPlaceHolderText("请选择角色");
}

UserAddDlg::~UserAddDlg()
{
	delete ui;
}

void UserAddDlg::clear()
{
	ui->nickNameEdit->clear();
	ui->phoneEdit->clear();
	ui->userNameEdit->clear();
	ui->sexCmb->setCurrentIndex(-1);
	//ui->postCmb->setCurrentIndex(-1);

	ui->deptCmb->setCurrentIndex(QModelIndex());
	ui->emailEdit->clear();
	ui->pwdEdit->clear();
	ui->isActive1_RBtn->setChecked(true);
	//ui->roleCmb->setCurrentIndex(-1);

	ui->remarkEdit->clear();
}

template<typename T>
static void recursionFn(const QList<std::shared_ptr<TreeSelect>>& treeSelects, T* parentItem)
{
	for (auto& tree: treeSelects) {
		auto item = new QStandardItem(tree->label);
		item->setData(QVariant::fromValue(tree), Qt::UserRole);
		recursionFn(tree->children, item);
		parentItem->appendRow(item);
	}
}

void UserAddDlg::setDeptTreeSelect(const QList<std::shared_ptr<TreeSelect>>& deptTreeSelect)
{
	ui->deptCmb->clear();

	auto m = dynamic_cast<QStandardItemModel*>(ui->deptCmb->model());
	if (!m) {
		qWarning() << "model is nullptr!";
		return;
	}
	recursionFn(deptTreeSelect, m);
}

void UserAddDlg::setPosts(const QList<std::shared_ptr<Post>>& posts)
{
	ui->postTagPicker->clear();
	ui->postTagPicker->clearSelectTags();
	//添加item
	for (auto& post : posts) {
		ui->postTagPicker->addItem(post->post_name, post->post_id);
	}
}

void UserAddDlg::setRoles(const QList<std::shared_ptr<Role>>& roles)
{
	ui->roleTagPicker->clear();
	ui->roleTagPicker->clearSelectTags();
	for (auto& role: roles) {
		ui->roleTagPicker->addItem(role->role_name, role->role_id);
	}
}

void UserAddDlg::on_okBtn_clicked()
{
	auto nickName= ui->nickNameEdit->text().trimmed();
	auto phone = ui->phoneEdit->text().trimmed();
	auto userName= ui->userNameEdit->text().trimmed();
	auto sex = ui->sexCmb->currentData().toInt();
	//获取岗位Id
	QJsonArray postIds;
	auto tagDatas = ui->postTagPicker->tagDatas();
	for (auto& tagData : tagDatas) {
		postIds.append(tagData.toInt());
	}

	//获取部门Id
	int deptId = 0;
	auto treeSelect = ui->deptCmb->currentData().value<std::shared_ptr<TreeSelect>>();
	if (treeSelect) {
		deptId = treeSelect->id;
	}

	auto email = ui->emailEdit->text().trimmed();
	auto pwd = ui->pwdEdit->text().trimmed();
	auto status = ui->isActive1_RBtn->isChecked();
	//auto roleId = ui->roleCmb->currentData().toInt();
	//获取角色Id
	QJsonArray roleIds;
	tagDatas = ui->roleTagPicker->tagDatas();
	for (auto& tagData : tagDatas) {
		roleIds.append(tagData.toInt());
	}


	auto remark = ui->remarkEdit->toPlainText().trimmed();

	if (nickName.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("用户昵称不能为空~");
		return;
	}

	if (userName.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("用户名称不能为空~");
		return;
	}

	if (pwd.isEmpty()) {
		NotifyTipManager::instance()->addNotifyTip("密码不能为空~");
		return;
	}

	QJsonObject juser;
	juser["nick_name"] = nickName;
	juser["phone_number"] = phone;
	juser["user_name"] = userName;
	juser["sex"] = sex;
	juser["postIds"] = postIds;
	juser["dept_id"] = deptId;
	juser["email"] = email;
	juser["password"] = pwd;
	juser["status"] = status;
	juser["roleIds"] = roleIds;
	juser["remark"] = remark;

	HttpMgr::instance()->post(URL("/user/add"), juser, ReqId::user_add, Modules::UserMgr);
}

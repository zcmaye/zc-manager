#pragma once

#include <QWidget>
#include "ui_PostEditDlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PostEditDlgClass; };
QT_END_NAMESPACE

struct Post;

class PostEditDlg : public QWidget
{
	Q_OBJECT

public:
	PostEditDlg(QWidget *parent = nullptr);
	~PostEditDlg();

	void clear();
	void setPost(const std::shared_ptr<Post>& post);
public slots:
	void on_okBtn_clicked();
private:
	Ui::PostEditDlgClass *ui;
	std::shared_ptr<Post> m_post;
};


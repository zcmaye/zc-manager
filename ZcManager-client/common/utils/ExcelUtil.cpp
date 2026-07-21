#include "ExcelUtil.h"

ExcelUtil::ExcelUtil(EntityType type)
	:m_eType(type)
	,m_dom(new QXlsx::Document)
{
	init(type);
}

ExcelUtil::~ExcelUtil()
{}

bool ExcelUtil::importTemplateExcel(QIODevice* io,const QString & sheetName)
{
	//创建一个工作簿
	QXlsx::Worksheet * sheet = dynamic_cast<QXlsx::Worksheet*>(m_dom->workbook()->addSheet(sheetName));

	//表头格式
	if (m_headerLabels.isEmpty()) {
		throw std::runtime_error("No header labels！");
	}
	QXlsx::Format format;
	format.setPatternBackgroundColor(QColor("#636363"));
	format.setFontColor(Qt::GlobalColor::white);
	format.setFontSize(16);
	format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

	//写入表头
	for (qsizetype i = 0; i < m_headerLabels.size(); i++) {
		sheet->write(1, i + 1, m_headerLabels[i],format);
		sheet->setColumnWidth(i + 1, i + 1, 20);
	}

	//写入测试数据
	if (!m_testValues.isEmpty()) {
		QXlsx::Format vformat;
		vformat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
		vformat.setFontSize(11);
		for (qsizetype i = 0; i < m_testValues.size(); i++) {
			sheet->write(2, i + 1, m_testValues[i], vformat);
		}
	}

	return m_dom->saveAs(io);
}

/*
QList<std::shared_ptr<User>> ExcelUtil::importExcel(const QString& sheetName, QIODevice* io, int titleNum)
{
	if (m_dom) {
		delete m_dom;
	}
	m_dom = new QXlsx::Document(io);

	//获取指定的sheet
	auto sheet = dynamic_cast<QXlsx::Worksheet*>(sheetName.isEmpty() ? m_dom->currentSheet() : m_dom->sheet(sheetName));
	if (!sheet) {
		throw std::runtime_error("文件sheet不存在");
	}

	//获取数据
	//获取总行数
	QXlsx::CellRange range =  sheet->dimension();
	if (range.rowCount() <= 0) {
		return {};
	}

	//获取表头
	QMap<int,QString> cellMap;
	for (int i = 0; i < range.columnCount(); i++) {
		auto cell =  sheet->cellAt(titleNum, i + 1);
		if (!cell) {
			cellMap.insert(i + 1, {});
		}
		else {
			cellMap.insert(i + 1,cell->value().toString());
		}
	}

	QList<std::shared_ptr<User>>  list;
	//遍历每一行数据
	for (int i = titleNum + 1; i <= range.rowCount(); i++) {
		//处理每一列
		auto user = std::make_shared<User>();
		for (int c = 0; c < range.columnCount(); c++) {
			auto cell = sheet->cellAt(i, c + 1);
			if (!cell) {
				continue;
			}
			auto value = cell->value();
			if (value.isNull() || !value.isValid()) {
				continue;
			}
			if (cellMap.value(c + 1) == "用户编号") {
				user->user_id = value.toInt();
			}
			else if (cellMap.value(c + 1) == "部门编号") {
				user->dept_id = value.toInt();
			}
			else if (cellMap.value(c + 1) == "登录名称") {
				user->user_name= value.toString();
			}
			else if (cellMap.value(c + 1) == "用户昵称") {
				user->nick_name = value.toString();
			}
			else if (cellMap.value(c + 1) == "用户邮箱") {
				user->email = value.toString();
			}
			else if (cellMap.value(c + 1) == "手机号码") {
				user->phone_number = value.toString();
			}
			else if (cellMap.value(c + 1) == "用户性别") {
				user->sex = value.toString() == "男" ? 1 : (value.toString() == "女" ? 0 : 2);
			}
			else if (cellMap.value(c + 1) == "用户状态") {
				user->status = value.toString() == "正常" ? 1 : 0;
			}
		}
		list.append(user);
	}

	return list;
}
*/

void ExcelUtil::init(EntityType type)
{
	if (type == EntityType::User) {
		m_headerLabels = { "用户编号","部门编号","登录名称","用户昵称","用户邮箱","手机号码","用户性别","账号状态" };
		m_testValues = { 4,103,"520hello","嘘嘘","hello@qq.com","158xxxxxxx","男","正常" };
	}
}


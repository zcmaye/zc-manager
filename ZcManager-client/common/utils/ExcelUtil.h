#pragma once
#include <QString>
#include "xlsxdocument.h"
#include "xlsxworkbook.h"

#include "domain/entity/User.hpp"

enum class EntityType {
	User,
};

class ExcelUtil 
{
public:
	ExcelUtil(EntityType type);
	~ExcelUtil();


	/**
	 * 导入模板.
	 */
	bool importTemplateExcel(QIODevice* io, const QString& sheetName);

	/**
	 * 解析xlsx文件，返回列表.
	 */
	//QList<std::shared_ptr<User>> importExcel(const QString& sheetName, QIODevice* io, int titleNum);
	template<typename T>
	inline QList<std::shared_ptr<T>> importExcel(const QString& sheetName, QIODevice* io, int titleNum);

	/**
	 * 导出xlsx文件
	 */
	template<typename T>
	void exportExcel(QIODevice*io,const QList<std::shared_ptr<T>>& list,const QString& sheetName);

private:
	/**
	 * 给对象设置值.
	 */
	template<typename T>
	void setValue(std::shared_ptr<T>& object, const QString& field, const QVariant& value);

	template<>
	inline void setValue(std::shared_ptr<User>& user, const QString& field, const QVariant& value);

	/**
	 * 将对象写入Excel.
	 */
	template<typename T>
	void writeValue(QXlsx::Worksheet* sheet,int row, std::shared_ptr<T> object);

	template<>
	inline void writeValue(QXlsx::Worksheet* sheet,int row, std::shared_ptr<User> user);
private:
	EntityType m_eType;
	QXlsx::Document* m_dom;
	QStringList m_headerLabels;
	QVariantList m_testValues;

	void init(EntityType type);
};


template<typename T>
inline QList<std::shared_ptr<T>> ExcelUtil::importExcel(const QString& sheetName, QIODevice* io, int titleNum)
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
	QXlsx::CellRange range = sheet->dimension();
	if (range.rowCount() <= 0) {
		return {};
	}

	//获取表头
	QMap<int, QString> cellMap;
	for (int i = 0; i < range.columnCount(); i++) {
		auto cell = sheet->cellAt(titleNum, i + 1);
		if (!cell) {
			cellMap.insert(i + 1, {});
		}
		else {
			cellMap.insert(i + 1, cell->value().toString());
		}

	}

	QList<std::shared_ptr<T>>  list;
	//遍历每一行数据
	for (int i = titleNum + 1; i <= range.rowCount(); i++) {
		auto object = std::make_shared<T>();
		//处理每一列
		for (int c = 0; c < range.columnCount(); c++) {
			auto cell = sheet->cellAt(i, c + 1);
			if (!cell) {
				continue;
			}
			auto value = cell->value();
			if (value.isNull() || !value.isValid()) {
				continue;
			}
			setValue(object, cellMap.value(c + 1), value);
		}
		list.append(object);
	}
	return list;
}

template<typename T>
inline void ExcelUtil::exportExcel(QIODevice* io, const QList<std::shared_ptr<T>>& list, const QString& sheetName)
{
	if (m_dom) {
		delete m_dom;
	}
	m_dom = new QXlsx::Document(io);

	QXlsx::Worksheet * sheet = dynamic_cast<QXlsx::Worksheet*>(m_dom->workbook()->addSheet(sheetName));
	if (!sheet) {
		throw std::runtime_error("文件sheet不存在");
	}

	int titleNum = 1;
	//写表头
	QStringList headerLabels = {"用户编号","部门编号","用户名","用户昵称","电话号码","邮箱","性别","状态","创建时间"};
	QXlsx::Format format;
	format.setPatternBackgroundColor(QColor("#636363"));
	format.setFontColor(Qt::GlobalColor::white);
	format.setFontSize(16);
	format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

	for (size_t i = 0; i < headerLabels.size(); i++) {
		sheet->write(titleNum, i + 1, headerLabels[i], format);
		sheet->setColumnWidth(i + 1, i + 1, 20);
	}

	//写数据
	for (qsizetype i = 0; i < list.size(); i++) {
		auto& user = list.at(i);
		writeValue(sheet, i + titleNum +1, user);
	}

	m_dom->saveAs(io);
}

template<>
inline void ExcelUtil::setValue(std::shared_ptr<User>& user, const QString& field, const QVariant& value)
{
	if (field == "用户编号") {
		user->user_id = value.toInt();
	}
	else if (field == "部门编号") {
		user->dept_id = value.toInt();
	}
	else if (field == "登录名称") {
		user->user_name = value.toString();
	}
	else if (field == "用户昵称") {
		user->nick_name = value.toString();
	}
	else if (field == "用户邮箱") {
		user->email = value.toString();
	}
	else if (field == "手机号码") {
		user->phone_number = value.toString();
	}
	else if (field == "用户性别") {
		user->sex = value.toString() == "男" ? 1 : (value.toString() == "女" ? 0 : 2);
	}
	else if (field == "账号状态") {
		user->status = value.toString() == "正常" ? 1 : 0;
	}
}

template<>
inline void ExcelUtil::writeValue(QXlsx::Worksheet* sheet, int row, std::shared_ptr<User> user)
{
	QXlsx::Format vformat;
	vformat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
	vformat.setFontSize(11);

	int col = 1;
	sheet->write(row, col++, user->user_id,vformat);
	sheet->write(row, col++, user->dept_id,vformat);
	sheet->write(row, col++, user->user_name,vformat);
	sheet->write(row, col++, user->nick_name,vformat);
	sheet->write(row, col++, user->phone_number,vformat);
	sheet->write(row, col++, user->email,vformat);
	sheet->write(row, col++, user->sexToString(),vformat);
	sheet->write(row, col++, user->statusToString(),vformat);
	sheet->write(row, col++, user->create_time,vformat);
}

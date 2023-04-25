#include "stdafx.h"
#include "Table.h"


Table::Table(QWidget * parent ):QTableWidget(parent){}

Table::Table(int rows, int columns, QWidget * parent):QTableWidget(rows,columns,parent){}

Table::~Table()
{
}

void Table::SetHeaderLabels(std::vector<std::string> labels)
{
	QStringList qLabelList;
	for (std::string label : labels)
	{
		qLabelList.push_back(QString::fromStdString(label));
	}
	setHorizontalHeaderLabels(qLabelList);
}

void Table::Clear(void)
{
}

void Table::setColorStyle(ak::aColorStyle * _colorStyle)
{
}

void Table::AddRow()
{
}

void Table::AddColumn()
{
}

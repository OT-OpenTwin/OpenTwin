#pragma once
#include <qtablewidget.h>

class Table : public QTableWidget {
	Q_OBJECT
private Q_SLOTS:

private:
	QColor			my_colorBack;
	QColor			my_colorFront;
	QColor			my_colorFocusBack;
	QColor			my_colorFocusFront;
	QColor			my_colorSelectedBack;
	QColor			my_colorSelectedFront;

public:
	Table(QWidget* parent = nullptr);
	Table(int rows, int columns, QWidget* parent = nullptr);
	~Table();
	
	void SetHeaderLabels(std::vector<std::string> labels);
	void AddRow();
	void AddColumn();
	void Clear(void);
};

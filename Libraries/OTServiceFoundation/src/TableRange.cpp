#include "OTServiceFoundation/TableRange.h"

ot::TableRange::TableRange()
{

}

ot::TableRange::TableRange(int topRow, int bottomRow, int leftColumn, int rightColumn) :
	_topRow(topRow),_bottomRow (bottomRow),_leftColumn (leftColumn),_rightColumn (rightColumn)
{}

ot::TableRange::TableRange(const TableRange & other)
	: _topRow(other._topRow), _bottomRow(other._bottomRow), _leftColumn(other._leftColumn), _rightColumn(other._rightColumn)
{}

ot::TableRange & ot::TableRange::operator=(const TableRange & other)
{
	_topRow = other._topRow;
	_bottomRow = other._bottomRow;
	_leftColumn = other._leftColumn;
	_rightColumn = other._rightColumn;
	return *this;
}

bool ot::TableRange::operator==(const TableRange & other)
{
	return _topRow == other._topRow && _bottomRow == other._bottomRow && _leftColumn == other._leftColumn && _rightColumn == other._rightColumn;
}

bool ot::TableRange::operator!=(const TableRange & other)
{
	return !(_topRow == other._topRow && _bottomRow == other._bottomRow && _leftColumn == other._leftColumn && _rightColumn == other._rightColumn);
}

void ot::TableRange::addToJsonObject(OT_rJSON_doc & _document, OT_rJSON_val & _object) const
{
	ot::rJSON::add(_document, _object, "tR", _topRow);
	ot::rJSON::add(_document, _object, "bR", _bottomRow);
	ot::rJSON::add(_document, _object, "lC", _leftColumn);
	ot::rJSON::add(_document, _object, "rC", _rightColumn);
}

void ot::TableRange::setFromJsonObject(OT_rJSON_val & _object)
{
	_topRow = ot::rJSON::getInt(_object, "tR");
	_bottomRow = ot::rJSON::getInt(_object, "bR");
	_leftColumn = ot::rJSON::getInt(_object, "lC");
	_rightColumn = ot::rJSON::getInt(_object, "rC");
}

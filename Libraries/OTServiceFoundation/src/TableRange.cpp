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

void ot::TableRange::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	_object.AddMember("lC", _leftColumn, _allocator);
	_object.AddMember("tR", _topRow, _allocator);
	_object.AddMember("rC", _rightColumn, _allocator);
	_object.AddMember("bR", _bottomRow, _allocator);
}

void ot::TableRange::setFromJsonObject(const ConstJsonObject& _object)
{
	_topRow = ot::json::getInt(_object, "tR");
	_bottomRow = ot::json::getInt(_object, "bR");
	_leftColumn = ot::json::getInt(_object, "lC");
	_rightColumn = ot::json::getInt(_object, "rC");
}

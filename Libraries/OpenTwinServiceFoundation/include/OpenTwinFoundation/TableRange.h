/*****************************************************************//**
 * \file   TableRange.h
 * \brief  
 * 
 * \author Wagner
 * \date   February 2023
 *********************************************************************/
#pragma once

// OT header
#include "OpenTwinCore/Serializable.h"
#include "OpenTwinFoundation/FoundationAPIExport.h"

namespace ot
{

	class OT_SERVICEFOUNDATION_API_EXPORT TableRange : public ot::Serializable
	{
	public:
			TableRange();
			TableRange(int topRow, int bottomRow, int leftColumn, int rightColumn);

			TableRange(const TableRange& other);
			TableRange& operator=(const TableRange& other);
			bool operator==(const TableRange& other);
			bool operator!=(const TableRange& other);

			// Inherited via Serializable
			virtual void addToJsonObject(OT_rJSON_doc & _document, OT_rJSON_val & _object) const override;
			virtual void setFromJsonObject(OT_rJSON_val & _object) override;

			int GetTopRow() const { return _topRow; }
			int GetBottomRow() const { return _bottomRow; }
			int GetLeftColumn()const { return _leftColumn; }
			int GetRightColumn() const { return _rightColumn; }

	private:
		int _topRow;
		int _bottomRow;
		int _leftColumn;
		int _rightColumn;
	};
}
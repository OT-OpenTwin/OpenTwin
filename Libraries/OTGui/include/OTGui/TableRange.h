/*****************************************************************//**
 * \file   TableRange.h
 * \brief
 *
 * \author Wagner
 * \date   February 2023
 *********************************************************************/
#pragma once

 // OT header
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT TableRange : public ot::Serializable {
	public:
		TableRange();
		TableRange(int topRow, int bottomRow, int leftColumn, int rightColumn);

		TableRange(const TableRange& other);
		TableRange& operator=(const TableRange& other);
		bool operator==(const TableRange& other);
		bool operator!=(const TableRange& other);

		// Inherited via Serializable

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		int getTopRow() const { return m_topRow; }
		int getBottomRow() const { return m_bottomRow; }
		int getLeftColumn()const { return m_leftColumn; }
		int getRightColumn() const { return m_rightColumn; }

	private:
		int m_topRow;
		int m_bottomRow;
		int m_leftColumn;
		int m_rightColumn;
	};
}
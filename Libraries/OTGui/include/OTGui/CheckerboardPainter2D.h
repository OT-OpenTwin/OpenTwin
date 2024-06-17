//! @file CheckerboardPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Size2D.h"
#include "OTGui/Painter2D.h"

#define OT_FactoryKey_CheckerboardPainter2D "OT_P2DCheck"

namespace ot {

	class OT_GUI_API_EXPORT CheckerboardPainter2D : public ot::Painter2D {
	public:
		CheckerboardPainter2D();
		CheckerboardPainter2D(ot::DefaultColor _primaryColor, ot::DefaultColor _secondaryColor, const Size2D& _cellSize = Size2D(10, 10));
		CheckerboardPainter2D(const ot::Color& _primaryColor, const ot::Color& _secondaryColor, const Size2D& _cellSize = Size2D(10, 10));

		//! \brief Assignment constructor.
		//! The object takes ownership of the provided painter.
		CheckerboardPainter2D(Painter2D* _primaryPainter, Painter2D* _secondaryPainter, const Size2D& _cellSize = Size2D(10, 10));
		virtual ~CheckerboardPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_CheckerboardPainter2D); };

		virtual std::string generateQss(void) const override;

		virtual ot::Color getDefaultColor(void) const override;

		virtual bool isEqualTo(const Painter2D* _other) const override;

		//! \brief Sets the primary painter.
		//! The object takes ownership of the provided painter.
		//! The current painter will be destroyed.
		//! \note Note that the primary and secondary painter must be different objects.
		void setPrimaryPainter(Painter2D* _painter);
		const Painter2D* getPrimaryPainter(void) const { return m_primary; };

		//! \brief Sets the secondary painter.
		//! The object takes ownership of the provided painter.
		//! The current painter will be destroyed.
		//! \note Note that the primary and secondary painter must be different objects.
		void setSecondaryPainter(Painter2D* _painter);
		const Painter2D* getSecondaryPainter(void) const { return m_secondary; };

		void setCellSize(int _size);
		void setCellSize(int _width, int _height);
		void setCellSize(const Size2D& _size) { m_cellSize = _size; };
		const Size2D& getCellSize(void) const { return m_cellSize; };

	private:
		Size2D m_cellSize;
		Painter2D* m_primary;
		Painter2D* m_secondary;

		CheckerboardPainter2D(const CheckerboardPainter2D&) = delete;
		CheckerboardPainter2D& operator = (const CheckerboardPainter2D&) = delete;
	};

}
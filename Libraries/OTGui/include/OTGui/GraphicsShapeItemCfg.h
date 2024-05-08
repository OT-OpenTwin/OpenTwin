//! @file GraphicsShapeItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Path2D.h"
#include "OTGui/Outline.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTCore/OTClassHelper.h"

#define OT_FactoryKey_GraphicsShapeItem "OT_GIShape"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsShapeItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOCOPY(GraphicsShapeItemCfg)
	public:
		GraphicsShapeItemCfg();
		virtual ~GraphicsShapeItemCfg();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsShapeItem); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Set the outline path.
		//! @param _path Outline path to set.
		void setOutlinePath(const Path2D& _path) { m_path = _path; };

		//! @brief Outline path reference.
		Path2D& outlinePath(void) { return m_path; };

		//! @brief Outline path const reference.
		const Path2D& outlinePath(void) const { return m_path; };

		//! @brief Set the background painter.
		//! The item takes ownership of the painter.
		//! @param _painter Painter to set.
		void setBackgroundPainter(Painter2D* _painter);
		
		//! @brief Background painter.
		const Painter2D* backgroundPainter(void) const { return m_backgroundPainter; };

		//! @brief Returns the current background painter and replaces it with the default background painter.
		//! The caller takes ownership of the returned painter.
		Painter2D* takeBackgroundPainter(void);

		//! @brief Set the outline.
		//! @param _outline Outline to set.
		void setOutline(const OutlineF& _outline) { m_outline = _outline; };

		//! @brief Outlin.
		const OutlineF& outline(void) const { return m_outline; };

		//! @brief Set the outline painter.
		//! The item takes ownership of the painter.
		void setOutlinePainter(Painter2D* _painter) { m_outline.setPainter(_painter); };

		//! @brief Outline painter.
		const Painter2D* outlinePainter(void) const { return m_outline.painter(); };

		//! @brief Set the outline width.
		//! @param _w Width to set.
		void setOutlineWidth(double _w) { m_outline.setWidth(_w); };

		//! @brief Outline width.
		double outlineWidth(void) const { return m_outline.width(); };

	private:
		Path2D m_path; //! @brief Outline path.
		Painter2D* m_backgroundPainter; //! @brief Background painter.
		OutlineF m_outline;
	};

}

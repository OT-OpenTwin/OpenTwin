//! @file GraphicsPolygonItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTGui/Outline.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <list>

#define OT_FactoryKey_GraphicsPolygonItem "OT_GIPoly"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsPolygonItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsPolygonItemCfg();
		GraphicsPolygonItemCfg(const GraphicsPolygonItemCfg& _other);
		virtual ~GraphicsPolygonItemCfg();

		//! \brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsPolygonItem); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Will append the provided point to the point list.
		//! @param _pt Point to add.
		void addPoint(const Point2DD& _pt) { m_points.push_back(_pt); };

		//! @brief Set the outline points.
		//! @param _points Outline points to set.
		void setPoints(const std::list<Point2DD>& _points) { m_points = _points; };

		//! @brief Outline points reference.
		std::list<Point2DD>& getPoints(void) { return m_points; };

		//! @brief Outline points const reference.
		const std::list<Point2DD>& getPoints(void) const { return m_points; };

		//! @brief Set the background painter.
		//! The item takes ownership of the painter.
		//! @param _painter Painter to set.
		void setBackgroundPainter(Painter2D* _painter);

		//! @brief Background painter.
		const Painter2D* getBackgroundPainter(void) const { return m_backgroundPainter; };

		//! @brief Returns the current background painter and replaces it with the default background painter.
		//! The caller takes ownership of the returned painter.
		Painter2D* takeBackgroundPainter(void);

		//! @brief Set the outline.
		//! @param _outline Outline to set.
		void setOutline(const OutlineF& _outline) { m_outline = _outline; };

		//! @brief Outlin.
		const OutlineF& getOutline(void) const { return m_outline; };

		//! @brief Set the outline painter.
		//! The item takes ownership of the painter.
		void setOutlinePainter(Painter2D* _painter) { m_outline.setPainter(_painter); };

		//! @brief Outline painter.
		const Painter2D* getOutlinePainter(void) const { return m_outline.painter(); };

		//! @brief Set the outline width.
		//! @param _w Width to set.
		void setOutlineWidth(double _w) { m_outline.setWidth(_w); };

		//! @brief Outline width.
		double getOutlineWidth(void) const { return m_outline.width(); };

		//! \brief Set the fill property.
		//! \see getFillPolygon
		void setFillPolygon(bool _fill) { m_fillPolygon = _fill; };

		//! \brief If fill polygon is enabled the polygon will be filled, otherwise only the outline will be drawn.
		bool getFillPolygon(void) const { return m_fillPolygon; };

	private:
		bool m_fillPolygon; //! \brief Fill polygon.
		std::list<Point2DD> m_points; //! @brief Outline points.
		Painter2D* m_backgroundPainter; //! @brief Background painter.
		OutlineF m_outline; //! @brief Outline style.

		GraphicsPolygonItemCfg& operator = (const GraphicsPolygonItemCfg& _other) = delete;
	};
}

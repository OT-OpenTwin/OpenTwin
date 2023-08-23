//! @file Painter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OpenTwinCore/Serializable.h"
#include "OpenTwinCore/SimpleFactory.h"
#include "OpenTwinCore/Color.h"

#define OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg "OT_P2DCFill"
#define OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg "OT_P2DCGrad"

namespace ot {

	class OT_GUI_API_EXPORT Painter2D : public ot::Serializable, public ot::SimpleFactoryObject {
	public:
		Painter2D() {};
		virtual ~Painter2D() {};

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

	private:
		Painter2D(const Painter2D&) = delete;
		Painter2D& operator = (const Painter2D&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORT FillPainter2D : public ot::Painter2D {
	public:
		FillPainter2D();
		FillPainter2D(const ot::Color& _color);
		virtual ~FillPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg); };

		void setColor(ot::Color& _color) { m_color = _color; };
		const ot::Color& color(void) const { return m_color; };

	private:
		ot::Color m_color;

		FillPainter2D(const FillPainter2D&) = delete;
		FillPainter2D& operator = (const FillPainter2D&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORT LinearGradientPainter2DStep : public ot::Serializable {
	public:
		LinearGradientPainter2DStep();
		LinearGradientPainter2DStep(double _x, double _y, const ot::Color& _color);
		LinearGradientPainter2DStep(const LinearGradientPainter2DStep& _other);
		virtual ~LinearGradientPainter2DStep();

		LinearGradientPainter2DStep& operator = (const LinearGradientPainter2DStep& _other);
		bool operator == (const LinearGradientPainter2DStep& _other) const;
		bool operator != (const LinearGradientPainter2DStep& _other) const;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void setX(double _x) { m_x = _x; };
		double x(void) const { return m_x; };

		void setY(double _y) { m_y = _y; };
		double y(void) const { return m_y; };

		void setColor(const ot::Color& _color) { m_color = _color; };
		const ot::Color& color(void) const { return m_color; };

	private:
		double    m_x;
		double    m_y;
		ot::Color m_color;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORT LinearGradientPainter2D : public ot::Painter2D {
	public:
		LinearGradientPainter2D();
		LinearGradientPainter2D(const std::vector<LinearGradientPainter2DStep>& _steps);
		virtual ~LinearGradientPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg); };

		void addStep(const LinearGradientPainter2DStep& _step);
		void setSteps(const std::vector<LinearGradientPainter2DStep>& _steps) { m_steps = _steps; };
		const std::vector<LinearGradientPainter2DStep>& steps(void) const { return m_steps; };

	private:
		std::vector<LinearGradientPainter2DStep> m_steps;

		LinearGradientPainter2D(const LinearGradientPainter2D&) = delete;
		LinearGradientPainter2D& operator = (const LinearGradientPainter2D&) = delete;
	};
}
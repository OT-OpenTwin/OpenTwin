// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/Painter/Painter2D.h"

namespace ot {

	//! @brief Painter2DContainer is a simple wrapper around a Painter2D pointer.
	//! It ensures that always a painter is set and implements copy and move logic.
	class OT_GUI_API_EXPORT Painter2DContainer : public Serializable {
	public:
		explicit Painter2DContainer();

		//! @brief Assignment constructor.
		//! The container takes ownership of the painter.
		//! @param _initialPainter Existing painter to set.
		explicit Painter2DContainer(Painter2D* _initialPainter);

		//! @brief Copy constructor.
		//! @param _initialPainter Existing painter to create copy of.
		explicit Painter2DContainer(const Painter2D* _initialPainter);

		Painter2DContainer(const Painter2DContainer& _other);
		Painter2DContainer(Painter2DContainer&& _other) noexcept;
		~Painter2DContainer();

		Painter2DContainer& operator=(const Painter2DContainer& _other);
		Painter2DContainer& operator=(Painter2DContainer&& _other) noexcept;

		void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Set the painter.
		//! The container takes ownership of the painter.
		//! @param _painter Existing painter to set.
		void setPainter(Painter2D* _painter);

		const Painter2D* getPainter() const { return m_painter; };
		const Painter2D* operator->() const { return m_painter; };
		const Painter2D* operator*() const { return m_painter; };

	private:
		Painter2D* m_painter;
	};
}
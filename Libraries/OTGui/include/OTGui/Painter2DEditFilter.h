//! @file Painter2DEditFilter.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/ColorStyleTypes.h"

// std header
#include <list>

namespace ot {

	class Painter2DEditFilter : public Serializable {
		OT_DECL_DEFCOPY(Painter2DEditFilter)
		OT_DECL_DEFMOVE(Painter2DEditFilter)
	public:
		enum PainterType {
			None           = 0 << 0, //! @brief No painter type allowed. Set painter won't be editable
			Fill           = 1 << 0, //! @brief Fill painter allowed.
			StyleRef       = 1 << 1, //! @brief Style reference painter allowed.
			LinearGradient = 1 << 2, //! @brief Linear gradient painter allowed.
			RadialGradient = 1 << 3, //! @brief Radial gradient painter allowed.

			All            = Fill | StyleRef | LinearGradient | RadialGradient
		};
		typedef Flags<PainterType> PainterTypes;

		Painter2DEditFilter();
		virtual ~Painter2DEditFilter() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setPainterTypeAllowed(PainterType _painterType, bool _allowed = true) { m_allowedPainters.setFlag(_painterType, _allowed); };
		void setPainterTypes(const PainterTypes& _painterTypes) { m_allowedPainters = _painterTypes; };
		const PainterTypes& getPainterTypes() const { return m_allowedPainters; };

		void setStyleReferences(const std::list<ColorStyleValueEntry>& _colorStyleValueEntries) { m_allowedReferences = _colorStyleValueEntries; };
		const std::list<ColorStyleValueEntry>& getStyleReferences() const { return m_allowedReferences; };

	private:
		PainterTypes                    m_allowedPainters;
		std::list<ColorStyleValueEntry> m_allowedReferences;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::Painter2DEditFilter::PainterType)
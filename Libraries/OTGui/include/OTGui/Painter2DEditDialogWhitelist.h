//! @file Painter2DEditDialogWhitelist.h
//! @author Alexander Kuester (alexk95)
//! @date May 2025
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

	class OT_GUI_API_EXPORT Painter2DEditDialogWhitelist : public Serializable {
	public:
		enum AllowedPainter {
			None           = 0 << 0, //! @brief No painter is allowed (dialog won't open).
			Fill           = 1 << 0, //! @brief Fill painter is allowed.
			LinearGradient = 1 << 1, //! @brief Linear gradient painter is allowed.
			RadialGradient = 1 << 2, //! @brief Radial gradient painter is allowed.
			StyleReference = 1 << 3, //! @brief Style reference painter is allowed.

			//! @brief All painters are allowed.
			AllPainters    = Fill | LinearGradient | RadialGradient | StyleReference
		};
		typedef Flags<AllowedPainter> AllowedPainters;

		//! @brief Creates a default whitelist that allows all painters.
		static Painter2DEditDialogWhitelist createDefault(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		Painter2DEditDialogWhitelist();
		Painter2DEditDialogWhitelist(const Painter2DEditDialogWhitelist& _other);
		Painter2DEditDialogWhitelist(Painter2DEditDialogWhitelist&& _other) noexcept;
		virtual ~Painter2DEditDialogWhitelist();

		Painter2DEditDialogWhitelist& operator = (const Painter2DEditDialogWhitelist& _other);
		Painter2DEditDialogWhitelist& operator = (Painter2DEditDialogWhitelist&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual functions

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Allow the specified painter type.
		//! @param _painter Painter type to allow.
		//! @param _allowed If true the painter is whitelisted, otherwise it is removed from the whitelist.
		void allowPainter(AllowedPainter _painter, bool _allowed = true) { m_allowedPainters.set(_painter); };

		//! @brief Set the allowed painters that may be selected by the user when selecting a painter.
		//! @param _allowedPainters Painter types to allow.
		void setAllowedPainters(const AllowedPainters& _allowedPainters) { m_allowedPainters = _allowedPainters; };

		//! @brief Returns true if the specified painter type is allowed.
		bool isPainterAllowed(AllowedPainter _painter) const { return m_allowedPainters.flagIsSet(_painter); };

		//! @brief Will add all style value entries to the allowed list.
		void allowAllColorStyleValueEntries(void);

		//! @brief Remove all style value entries from the allowed list.
		void clearAllowedColorStyleValueEntries(void) { m_allowedColorStyleValueEntries.clear(); };

		//! @brief Add the specified color style value entry to the whitelist.
		//! @param _entry Entry to add.
		void addAllowedColorStyleValueEntry(const ot::ColorStyleValueEntry& _entry) { m_allowedColorStyleValueEntries.push_back(_entry); };

		//! @brief Set the allowed color style value entries that may be selected by the user when selecting a style reference painter.
		//! @param _allowedColorStyleValueEntries Color style value entries that are allowed to be selected.
		void setAllowedColorStyleValueEntries(const std::list<ot::ColorStyleValueEntry>& _allowedColorStyleValueEntries) { m_allowedColorStyleValueEntries = _allowedColorStyleValueEntries; };
		const std::list<ot::ColorStyleValueEntry>& getAllowedColorStyleValueEntries(void) const { return m_allowedColorStyleValueEntries; };

	private:
		AllowedPainters                     m_allowedPainters; //! @brief Allowed painters that may be selected by the user when selecting a painter.
		std::list<ot::ColorStyleValueEntry> m_allowedColorStyleValueEntries;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::Painter2DEditDialogWhitelist::AllowedPainter)
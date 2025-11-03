// @otlicense
// File: Painter2DDialogFilter.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/ColorStyleTypes.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT Painter2DDialogFilter : public Serializable {
		OT_DECL_DEFCOPY(Painter2DDialogFilter)
		OT_DECL_DEFMOVE(Painter2DDialogFilter)
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

		Painter2DDialogFilter();
		virtual ~Painter2DDialogFilter() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setPainterTypeAllowed(PainterType _painterType, bool _allowed = true) { m_allowedPainters.set(_painterType, _allowed); };
		void setPainterTypes(const PainterTypes& _painterTypes) { m_allowedPainters = _painterTypes; };
		const PainterTypes& getPainterTypes() const { return m_allowedPainters; };

		void setStyleReferences(const std::list<ColorStyleValueEntry>& _colorStyleValueEntries) { m_allowedReferences = _colorStyleValueEntries; };
		void addStyleReference(ColorStyleValueEntry _entry) { m_allowedReferences.push_back(_entry); };
		const std::list<ColorStyleValueEntry>& getStyleReferences() const { return m_allowedReferences; };

	private:
		PainterTypes                    m_allowedPainters;
		std::list<ColorStyleValueEntry> m_allowedReferences;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::Painter2DDialogFilter::PainterType, ot::Painter2DDialogFilter::PainterTypes)
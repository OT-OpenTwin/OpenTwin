// @otlicense
// File: GraphicsPickerManager.h
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
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"
#include "OTWidgets/GraphicsPicker.h"

// std header
#include <map>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsPickerManager {
		OT_DECL_NOCOPY(GraphicsPickerManager)
		OT_DECL_DEFMOVE(GraphicsPickerManager)
	public:
		GraphicsPickerManager(GraphicsPicker* _picker = (GraphicsPicker*)nullptr);
		virtual ~GraphicsPickerManager() {};

		void setCurrentOwner(const BasicServiceInformation& _owner);
		const BasicServiceInformation& getCurrentOwner() const { return m_currentOwner; };

		void setPicker(GraphicsPicker* _picker) { m_picker = _picker; };
		void forgetPicker() { m_picker = nullptr; };

		void addCollections(const std::list<GraphicsPickerCollectionCfg>& _collections, const BasicServiceInformation& _owner);
		void addCollections(std::list<GraphicsPickerCollectionCfg>&& _collections, const BasicServiceInformation& _owner);
		
		const std::list<GraphicsPickerCollectionCfg>& getCollections(const BasicServiceInformation& _owner) const;

		void clear();
		
		void clearPicker();

	private:
		struct PickerInfo {
			PickerInfo() = default;
			PickerInfo(GraphicsPickerCollectionCfg&& _collection) : collection(std::move(_collection)) {}
			
			GraphicsPickerCollectionCfg collection;
			GraphicsPicker::PickerState state;
		};

		void applyCurrentOwner();

		PickerInfo& getOwnerInfo(const BasicServiceInformation& _owner);
		const PickerInfo& getOwnerInfo(const BasicServiceInformation& _owner) const;

		GraphicsPicker* m_picker;
		BasicServiceInformation m_currentOwner;

		std::map<BasicServiceInformation, PickerInfo> m_collections;
		PickerInfo m_emptyInfo;
	};

}

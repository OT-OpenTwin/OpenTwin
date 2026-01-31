// @otlicense
// File: SequenceDiaCallCfgFactory.h
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
#include "OTCore/FactoryTemplate.h"
#include "OTGui/Diagram/SequenceDiaAbstractCallCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT SequenceDiaCallCfgFactory : public FactoryTemplate<SequenceDiaAbstractCallCfg> {
		OT_DECL_NOCOPY(SequenceDiaCallCfgFactory)
		OT_DECL_NOMOVE(SequenceDiaCallCfgFactory)
	public:
		static SequenceDiaCallCfgFactory& instance();

		static SequenceDiaAbstractCallCfg* create(const ConstJsonObject& _object);

	private:
		SequenceDiaCallCfgFactory() {};
		virtual ~SequenceDiaCallCfgFactory() {};

	};

	template <class T>
	class OT_GUI_API_EXPORT SequenceDiaCallCfgFactoryRegistrar : public FactoryRegistrarTemplate<SequenceDiaCallCfgFactory, T> {
	public:
		SequenceDiaCallCfgFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<SequenceDiaCallCfgFactory, T>(_key) {};
	};

}
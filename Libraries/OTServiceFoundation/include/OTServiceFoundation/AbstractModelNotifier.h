// @otlicense
// File: AbstractModelNotifier.h
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
#include "OTCore/OTClassHelper.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

namespace ot {
	
	class OT_SERVICEFOUNDATION_API_EXPORTONLY AbstractModelNotifier {
		OT_DECL_NOCOPY(AbstractModelNotifier)
		OT_DECL_NOMOVE(AbstractModelNotifier)
	public:
		AbstractModelNotifier() = default;
		virtual ~AbstractModelNotifier() = default;
	};

}
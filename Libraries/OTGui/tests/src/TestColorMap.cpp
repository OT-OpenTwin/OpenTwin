// @otlicense
// File: TestColorMap.cpp
// 
// License:
// Copyright 2026 by OpenTwin
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

#include "gtest/gtest.h"

// OpenTwin header
#include "OTGui/ColorMap.h"
#include "OTCore/JSON/JSON.h"

TEST(ColorMapTests, DefaultIsViridis) {
	ot::ColorMap cm;
	EXPECT_EQ(ot::ColorMapPreset::Viridis, cm.getPreset());
	EXPECT_EQ("Viridis", cm.getName());
	EXPECT_FALSE(cm.getStops().empty());
	EXPECT_EQ(0.0f, cm.getStops().front().position);
	EXPECT_EQ(1.0f, cm.getStops().back().position);
}

TEST(ColorMapTests, PresetEndpointsAndNames) {
	const ot::ColorMapPreset presets[] = {
		ot::ColorMapPreset::Viridis,
		ot::ColorMapPreset::Plasma,
		ot::ColorMapPreset::Turbo,
		ot::ColorMapPreset::CoolWarm,
		ot::ColorMapPreset::OkabeIto,
		ot::ColorMapPreset::TolVibrant,
		ot::ColorMapPreset::Rainbow
	};

	for (auto p : presets) {
		ot::ColorMap cm(p);
		EXPECT_EQ(p, cm.getPreset());
		EXPECT_EQ(ot::toString(p), cm.getName());
		EXPECT_GE(cm.getStops().size(), 2u);
		EXPECT_FLOAT_EQ(0.0f, cm.getStops().front().position);
		EXPECT_FLOAT_EQ(1.0f, cm.getStops().back().position);

		// Clamping at boundaries
		EXPECT_EQ(cm.getStops().front().color, cm.getColor(0.0f));
		EXPECT_EQ(cm.getStops().front().color, cm.getColor(-0.5f));
		EXPECT_EQ(cm.getStops().back().color, cm.getColor(1.0f));
		EXPECT_EQ(cm.getStops().back().color, cm.getColor(1.5f));

		// Midpoint produces a valid color
		ot::Color mid = cm.getColor(0.5f);
		EXPECT_TRUE(mid.isValid());
	}
}

TEST(ColorMapTests, CustomStopsAutoSorted) {
	ot::ColorMap cm("CustomRamp", {}, ot::ColorInterpolationSpace::Oklab);
	cm.addStop(0.8f, ot::Color(255, 0, 0));
	cm.addStop(0.2f, ot::Color(0, 255, 0));
	cm.addStop(0.5f, ot::Color(0, 0, 255));

	const auto& stops = cm.getStops();
	ASSERT_EQ(3u, stops.size());
	EXPECT_FLOAT_EQ(0.2f, stops[0].position);
	EXPECT_EQ(ot::Color(0, 255, 0), stops[0].color);

	EXPECT_FLOAT_EQ(0.5f, stops[1].position);
	EXPECT_EQ(ot::Color(0, 0, 255), stops[1].color);

	EXPECT_FLOAT_EQ(0.8f, stops[2].position);
	EXPECT_EQ(ot::Color(255, 0, 0), stops[2].color);

	EXPECT_EQ(ot::ColorMapPreset::Custom, cm.getPreset());
}

TEST(ColorMapTests, JsonRoundTrip) {
	ot::ColorMap original(ot::ColorMapPreset::Plasma);
	original.setInterpolationSpace(ot::ColorInterpolationSpace::Oklab);

	ot::JsonDocument doc;
	original.addToJsonObject(doc, doc.GetAllocator());

	std::string json = doc.toJson();
	EXPECT_FALSE(json.empty());

	ot::JsonDocument importDoc;
	EXPECT_TRUE(importDoc.fromJson(json));
	EXPECT_TRUE(importDoc.IsObject());

	ot::ColorMap reconstructed;
	ot::ConstJsonObject obj = importDoc.getConstObject();
	EXPECT_NO_THROW(reconstructed.setFromJsonObject(obj));

	EXPECT_EQ(original, reconstructed);
	EXPECT_EQ(original.getName(), reconstructed.getName());
	EXPECT_EQ(original.getPreset(), reconstructed.getPreset());
	EXPECT_EQ(original.getInterpolationSpace(), reconstructed.getInterpolationSpace());
	EXPECT_EQ(original.getStops().size(), reconstructed.getStops().size());

	for (size_t i = 0; i < original.getStops().size(); ++i) {
		EXPECT_FLOAT_EQ(original.getStops()[i].position, reconstructed.getStops()[i].position);
		EXPECT_EQ(original.getStops()[i].color, reconstructed.getStops()[i].color);
	}
}

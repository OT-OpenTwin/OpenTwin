// @otlicense
// File: TestColor.cpp
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

#include "gtest/gtest.h"
#include "OTCore/Color.h"

TEST(ColorTests, HsvPureColors) {
	// Pure Red
	ot::Color red(255, 0, 0);
	float h = 0.f, s = 0.f, v = 0.f;
	red.toHsv(h, s, v);
	EXPECT_NEAR(0.f, h, 1e-2f);
	EXPECT_NEAR(1.f, s, 1e-2f);
	EXPECT_NEAR(1.f, v, 1e-2f);

	ot::Color fromRed = ot::Color::fromHsv(0.f, 1.f, 1.f);
	EXPECT_EQ(red, fromRed);

	// Pure Green
	ot::Color green(0, 255, 0);
	green.toHsv(h, s, v);
	EXPECT_NEAR(120.f, h, 1e-2f);
	EXPECT_NEAR(1.f, s, 1e-2f);
	EXPECT_NEAR(1.f, v, 1e-2f);

	ot::Color fromGreen = ot::Color::fromHsv(120.f, 1.f, 1.f);
	EXPECT_EQ(green, fromGreen);

	// Pure Blue
	ot::Color blue(0, 0, 255);
	blue.toHsv(h, s, v);
	EXPECT_NEAR(240.f, h, 1e-2f);
	EXPECT_NEAR(1.f, s, 1e-2f);
	EXPECT_NEAR(1.f, v, 1e-2f);

	ot::Color fromBlue = ot::Color::fromHsv(240.f, 1.f, 1.f);
	EXPECT_EQ(blue, fromBlue);

	// Pure Yellow
	ot::Color yellow(255, 255, 0);
	yellow.toHsv(h, s, v);
	EXPECT_NEAR(60.f, h, 1e-2f);
	EXPECT_NEAR(1.f, s, 1e-2f);
	EXPECT_NEAR(1.f, v, 1e-2f);

	ot::Color fromYellow = ot::Color::fromHsv(60.f, 1.f, 1.f);
	EXPECT_EQ(yellow, fromYellow);

	// Pure Cyan
	ot::Color cyan(0, 255, 255);
	cyan.toHsv(h, s, v);
	EXPECT_NEAR(180.f, h, 1e-2f);
	EXPECT_NEAR(1.f, s, 1e-2f);
	EXPECT_NEAR(1.f, v, 1e-2f);

	ot::Color fromCyan = ot::Color::fromHsv(180.f, 1.f, 1.f);
	EXPECT_EQ(cyan, fromCyan);

	// Pure Magenta
	ot::Color magenta(255, 0, 255);
	magenta.toHsv(h, s, v);
	EXPECT_NEAR(300.f, h, 1e-2f);
	EXPECT_NEAR(1.f, s, 1e-2f);
	EXPECT_NEAR(1.f, v, 1e-2f);

	ot::Color fromMagenta = ot::Color::fromHsv(300.f, 1.f, 1.f);
	EXPECT_EQ(magenta, fromMagenta);

	// Black
	ot::Color black(0, 0, 0);
	black.toHsv(h, s, v);
	EXPECT_NEAR(0.f, v, 1e-2f);
	EXPECT_NEAR(0.f, s, 1e-2f);

	ot::Color fromBlack = ot::Color::fromHsv(0.f, 0.f, 0.f);
	EXPECT_EQ(black, fromBlack);

	// White
	ot::Color white(255, 255, 255);
	white.toHsv(h, s, v);
	EXPECT_NEAR(1.f, v, 1e-2f);
	EXPECT_NEAR(0.f, s, 1e-2f);

	ot::Color fromWhite = ot::Color::fromHsv(0.f, 0.f, 1.f);
	EXPECT_EQ(white, fromWhite);
}

TEST(ColorTests, HsvHueWrapping) {
	ot::Color c1 = ot::Color::fromHsv(360.f, 1.f, 1.f);
	ot::Color c2 = ot::Color::fromHsv(0.f, 1.f, 1.f);
	EXPECT_EQ(c1, c2);

	ot::Color c3 = ot::Color::fromHsv(-120.f, 1.f, 1.f);
	ot::Color c4 = ot::Color::fromHsv(240.f, 1.f, 1.f);
	EXPECT_EQ(c3, c4);
}

TEST(ColorTests, HsvRoundTripArbitrary) {
	ot::Color original(210, 105, 30, 200); // Chocolate
	float h = 0.f, s = 0.f, v = 0.f;
	original.toHsv(h, s, v);

	ot::Color reconstructed = ot::Color::fromHsv(h, s, v, original.a());
	EXPECT_NEAR(original.r(), reconstructed.r(), 1);
	EXPECT_NEAR(original.g(), reconstructed.g(), 1);
	EXPECT_NEAR(original.b(), reconstructed.b(), 1);
	EXPECT_EQ(original.a(), reconstructed.a());
}

TEST(ColorTests, HslPureColors) {
	// Pure Red
	ot::Color red(255, 0, 0);
	float h = 0.f, s = 0.f, l = 0.f;
	red.toHsl(h, s, l);
	EXPECT_NEAR(0.f, h, 1e-2f);
	EXPECT_NEAR(1.f, s, 1e-2f);
	EXPECT_NEAR(0.5f, l, 1e-2f);

	ot::Color fromRed = ot::Color::fromHsl(0.f, 1.f, 0.5f);
	EXPECT_EQ(red, fromRed);

	// Pure Green
	ot::Color green(0, 255, 0);
	green.toHsl(h, s, l);
	EXPECT_NEAR(120.f, h, 1e-2f);
	EXPECT_NEAR(1.f, s, 1e-2f);
	EXPECT_NEAR(0.5f, l, 1e-2f);

	ot::Color fromGreen = ot::Color::fromHsl(120.f, 1.f, 0.5f);
	EXPECT_EQ(green, fromGreen);

	// Pure Blue
	ot::Color blue(0, 0, 255);
	blue.toHsl(h, s, l);
	EXPECT_NEAR(240.f, h, 1e-2f);
	EXPECT_NEAR(1.f, s, 1e-2f);
	EXPECT_NEAR(0.5f, l, 1e-2f);

	ot::Color fromBlue = ot::Color::fromHsl(240.f, 1.f, 0.5f);
	EXPECT_EQ(blue, fromBlue);

	// Pure Black
	ot::Color black(0, 0, 0);
	black.toHsl(h, s, l);
	EXPECT_NEAR(0.f, l, 1e-2f);
	EXPECT_NEAR(0.f, s, 1e-2f);

	ot::Color fromBlack = ot::Color::fromHsl(0.f, 0.f, 0.f);
	EXPECT_EQ(black, fromBlack);

	// Pure White
	ot::Color white(255, 255, 255);
	white.toHsl(h, s, l);
	EXPECT_NEAR(1.f, l, 1e-2f);
	EXPECT_NEAR(0.f, s, 1e-2f);

	ot::Color fromWhite = ot::Color::fromHsl(0.f, 0.f, 1.f);
	EXPECT_EQ(white, fromWhite);
}

TEST(ColorTests, HslRoundTripArbitrary) {
	ot::Color original(210, 105, 30, 200); // Chocolate
	float h = 0.f, s = 0.f, l = 0.f;
	original.toHsl(h, s, l);

	ot::Color reconstructed = ot::Color::fromHsl(h, s, l, original.a());
	EXPECT_NEAR(original.r(), reconstructed.r(), 1);
	EXPECT_NEAR(original.g(), reconstructed.g(), 1);
	EXPECT_NEAR(original.b(), reconstructed.b(), 1);
	EXPECT_EQ(original.a(), reconstructed.a());
}

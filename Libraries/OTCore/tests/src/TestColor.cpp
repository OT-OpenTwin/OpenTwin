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


TEST(ColorTests, OklabPureAndNeutrals) {
	// Pure Black
	ot::Color black(0, 0, 0);
	float L = 0.f, a = 0.f, b = 0.f;
	black.toOklab(L, a, b);
	EXPECT_NEAR(0.f, L, 1e-3f);
	EXPECT_NEAR(0.f, a, 1e-3f);
	EXPECT_NEAR(0.f, b, 1e-3f);

	ot::Color fromBlack = ot::Color::fromOklab(0.f, 0.f, 0.f);
	EXPECT_EQ(black, fromBlack);

	// Pure White
	ot::Color white(255, 255, 255);
	white.toOklab(L, a, b);
	EXPECT_NEAR(1.f, L, 1e-3f);
	EXPECT_NEAR(0.f, a, 1e-3f);
	EXPECT_NEAR(0.f, b, 1e-3f);

	ot::Color fromWhite = ot::Color::fromOklab(1.f, 0.f, 0.f);
	EXPECT_EQ(white, fromWhite);

	// Pure Red (255, 0, 0)
	ot::Color red(255, 0, 0);
	red.toOklab(L, a, b);
	EXPECT_NEAR(0.6279f, L, 1e-2f);
	EXPECT_NEAR(0.2249f, a, 1e-2f);
	EXPECT_NEAR(0.1258f, b, 1e-2f);

	ot::Color fromRed = ot::Color::fromOklab(L, a, b);
	EXPECT_NEAR(red.r(), fromRed.r(), 1);
	EXPECT_NEAR(red.g(), fromRed.g(), 1);
	EXPECT_NEAR(red.b(), fromRed.b(), 1);
}

TEST(ColorTests, OklabRoundTripArbitrary) {
	const ot::Color colors[] = {
		ot::Color(210, 105, 30, 200),  // Chocolate
		ot::Color(65, 105, 225, 255),  // RoyalBlue
		ot::Color(34, 139, 34, 255),   // ForestGreen
		ot::Color(255, 215, 0, 255),   // Gold
		ot::Color(128, 128, 128, 128)  // Gray with alpha
	};

	for (const auto& original : colors) {
		float L = 0.f, a = 0.f, b = 0.f;
		original.toOklab(L, a, b);

		ot::Color reconstructed = ot::Color::fromOklab(L, a, b, original.a());
		EXPECT_NEAR(original.r(), reconstructed.r(), 1);
		EXPECT_NEAR(original.g(), reconstructed.g(), 1);
		EXPECT_NEAR(original.b(), reconstructed.b(), 1);
		EXPECT_EQ(original.a(), reconstructed.a());
	}
}

TEST(ColorTests, OklchPureAndNeutrals) {
	// Pure Black
	ot::Color black(0, 0, 0);
	float L = 0.f, C = 0.f, h = 0.f;
	black.toOklch(L, C, h);
	EXPECT_NEAR(0.f, L, 1e-3f);
	EXPECT_NEAR(0.f, C, 1e-3f);

	ot::Color fromBlack = ot::Color::fromOklch(0.f, 0.f, 0.f);
	EXPECT_EQ(black, fromBlack);

	// Pure White
	ot::Color white(255, 255, 255);
	white.toOklch(L, C, h);
	EXPECT_NEAR(1.f, L, 1e-3f);
	EXPECT_NEAR(0.f, C, 1e-3f);

	ot::Color fromWhite = ot::Color::fromOklch(1.f, 0.f, 0.f);
	EXPECT_EQ(white, fromWhite);

	// Pure Red (255, 0, 0)
	ot::Color red(255, 0, 0);
	red.toOklch(L, C, h);
	EXPECT_NEAR(0.6279f, L, 1e-2f);
	EXPECT_NEAR(0.2577f, C, 1e-2f);
	EXPECT_NEAR(29.23f, h, 1e-1f);

	ot::Color fromRed = ot::Color::fromOklch(L, C, h);
	EXPECT_NEAR(red.r(), fromRed.r(), 1);
	EXPECT_NEAR(red.g(), fromRed.g(), 1);
	EXPECT_NEAR(red.b(), fromRed.b(), 1);
}

TEST(ColorTests, OklchHueWrappingAndRoundTrip) {
	ot::Color original(210, 105, 30, 200); // Chocolate
	float L = 0.f, C = 0.f, h = 0.f;
	original.toOklch(L, C, h);

	ot::Color normalReconstructed = ot::Color::fromOklch(L, C, h, original.a());
	EXPECT_NEAR(original.r(), normalReconstructed.r(), 1);
	EXPECT_NEAR(original.g(), normalReconstructed.g(), 1);
	EXPECT_NEAR(original.b(), normalReconstructed.b(), 1);
	EXPECT_EQ(original.a(), normalReconstructed.a());

	// Test wrapping with +720 degrees
	ot::Color wrappedReconstructed = ot::Color::fromOklch(L, C, h + 720.f, original.a());
	EXPECT_EQ(normalReconstructed, wrappedReconstructed);

	// Test negative hue wrapping with -360 degrees
	ot::Color negWrappedReconstructed = ot::Color::fromOklch(L, C, h - 360.f, original.a());
	EXPECT_EQ(normalReconstructed, negWrappedReconstructed);
}


TEST(ColorTests, LuminanceAndContrastWcag22) {
	ot::Color black(0, 0, 0);
	ot::Color white(255, 255, 255);
	ot::Color red(255, 0, 0);
	ot::Color green(0, 255, 0);
	ot::Color blue(0, 0, 255);

	// Relative Luminance of primaries
	EXPECT_NEAR(0.0, black.relativeLuminance(), 1e-4);
	EXPECT_NEAR(1.0, white.relativeLuminance(), 1e-4);
	EXPECT_NEAR(0.2126, red.relativeLuminance(), 1e-4);
	EXPECT_NEAR(0.7152, green.relativeLuminance(), 1e-4);
	EXPECT_NEAR(0.0722, blue.relativeLuminance(), 1e-4);

	// Contrast ratios
	// Max contrast Black vs White is exactly 21:1
	EXPECT_NEAR(21.0, black.contrastRatio(white), 1e-3);
	EXPECT_NEAR(21.0, white.contrastRatio(black), 1e-3);

	// Same color contrast is 1:1
	EXPECT_NEAR(1.0, red.contrastRatio(red), 1e-3);

	// Contrast symmetry
	EXPECT_NEAR(red.contrastRatio(blue), blue.contrastRatio(red), 1e-6);

	// WCAG AA requirement (>= 4.5:1) for Black on White
	EXPECT_GE(white.contrastRatio(black), 4.5);
}

TEST(ColorTests, ContrastingTextColorWcag22) {
	ot::Color white(255, 255, 255);
	ot::Color black(0, 0, 0);
	ot::Color yellow(255, 255, 0);
	ot::Color navy(0, 0, 128);
	ot::Color darkBlue(10, 20, 80);
	ot::Color lightCyan(224, 255, 255);

	// Light backgrounds must choose Black text
	EXPECT_EQ(ot::Color(ot::DefaultColor::Black), white.getContrastingTextColor());
	EXPECT_EQ(ot::Color(ot::DefaultColor::Black), yellow.getContrastingTextColor());
	EXPECT_EQ(ot::Color(ot::DefaultColor::Black), lightCyan.getContrastingTextColor());

	// Dark backgrounds must choose White text
	EXPECT_EQ(ot::Color(ot::DefaultColor::White), black.getContrastingTextColor());
	EXPECT_EQ(ot::Color(ot::DefaultColor::White), navy.getContrastingTextColor());
	EXPECT_EQ(ot::Color(ot::DefaultColor::White), darkBlue.getContrastingTextColor());

	// Guaranteed contrast ratio >= 4.5:1 (WCAG 2.2 AA)
	EXPECT_GE(white.contrastRatio(white.getContrastingTextColor()), 4.5);
	EXPECT_GE(black.contrastRatio(black.getContrastingTextColor()), 4.5);
	EXPECT_GE(yellow.contrastRatio(yellow.getContrastingTextColor()), 4.5);
	EXPECT_GE(navy.contrastRatio(navy.getContrastingTextColor()), 4.5);
}


TEST(ColorInterpolationTests, BoundaryAndClamping) {
	ot::Color c1(255, 0, 0, 100);
	ot::Color c2(0, 255, 0, 200);

	// Exact boundaries
	EXPECT_EQ(c1, ot::Color::interpolate(c1, c2, 0.0f));
	EXPECT_EQ(c2, ot::Color::interpolate(c1, c2, 1.0f));
	EXPECT_EQ(c1, c1.interpolate(c2, 0.0f));
	EXPECT_EQ(c2, c1.interpolate(c2, 1.0f));

	// Clamping outside [0.0, 1.0]
	EXPECT_EQ(c1, ot::Color::interpolate(c1, c2, -0.5f));
	EXPECT_EQ(c2, ot::Color::interpolate(c1, c2, 1.5f));
	EXPECT_EQ(c1, c1.interpolate(c2, -0.5f));
	EXPECT_EQ(c2, c1.interpolate(c2, 1.5f));
}

TEST(ColorInterpolationTests, AlphaInterpolation) {
	ot::Color c1(100, 100, 100, 50);
	ot::Color c2(100, 100, 100, 150);

	ot::Color mid = ot::Color::interpolate(c1, c2, 0.5f, ot::ColorInterpolationSpace::Rgb);
	EXPECT_EQ(100, mid.a());
}

TEST(ColorInterpolationTests, RgbInterpolation) {
	ot::Color black(0, 0, 0);
	ot::Color white(200, 100, 50);

	ot::Color mid = ot::Color::interpolate(black, white, 0.5f, ot::ColorInterpolationSpace::Rgb);
	EXPECT_NEAR(100, mid.r(), 1);
	EXPECT_NEAR(50, mid.g(), 1);
	EXPECT_NEAR(25, mid.b(), 1);
}

TEST(ColorInterpolationTests, HsvShortestVsLongest) {
	ot::Color red(255, 0, 0);    // Hue 0 deg
	ot::Color green(0, 255, 0);  // Hue 120 deg

	// Shortest arc passes through Yellow (Hue 60 deg)
	ot::Color shortestMid = ot::Color::interpolate(red, green, 0.5f, ot::ColorInterpolationSpace::HsvShortest);
	float h = 0.f, s = 0.f, v = 0.f;
	shortestMid.toHsv(h, s, v);
	EXPECT_NEAR(60.f, h, 2.0f);

	// Longest arc passes through Blue (Hue 240 deg)
	ot::Color longestMid = ot::Color::interpolate(red, green, 0.5f, ot::ColorInterpolationSpace::HsvLongest);
	longestMid.toHsv(h, s, v);
	EXPECT_NEAR(240.f, h, 2.0f);
}

TEST(ColorInterpolationTests, OklabSmoothness) {
	ot::Color blue(0, 0, 255);
	ot::Color yellow(255, 255, 0);

	// Oklab midpoint between blue and yellow
	ot::Color mid = ot::Color::interpolate(blue, yellow, 0.5f, ot::ColorInterpolationSpace::Oklab);
	EXPECT_TRUE(mid.isValid());
	EXPECT_GT(mid.r(), 0);
	EXPECT_GT(mid.g(), 0);
	EXPECT_GT(mid.b(), 0);
}

TEST(ColorInterpolationTests, OklchShortestArc) {
	ot::Color red(255, 0, 0);
	ot::Color green(0, 255, 0);

	ot::Color mid = ot::Color::interpolate(red, green, 0.5f, ot::ColorInterpolationSpace::Oklch);
	EXPECT_TRUE(mid.isValid());

	float L = 0.f, C = 0.f, h = 0.f;
	mid.toOklch(L, C, h);
	EXPECT_GT(L, 0.0f);
	EXPECT_GT(C, 0.0f);
}

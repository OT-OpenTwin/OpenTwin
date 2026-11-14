// @otlicense
// File: TestTableCellContrast.cpp
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

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/ColorMap.h"
#include "OTGui/ColoredTableRange.h"
#include "OTGui/TableRange.h"
#include "OTGui/Widgets/TableCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Widgets/Table.h"

// Qt header
#include <QtCore/qglobal.h>
#include <QtGui/qimage.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtWidgets/qabstractitemdelegate.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qstyleoption.h>

namespace {

	void ensureQApplication() {
		if (!QApplication::instance()) {
			if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
				qputenv("QT_QPA_PLATFORM", "offscreen");
			}
			static int argc = 1;
			static char appName[] = "OTWidgets_tests";
			static char* argv[] = { appName, nullptr };
			static QApplication app(argc, argv);
		}
	}

	class TableCellContrastTest : public ::testing::Test {
	protected:
		void SetUp() override {
			ensureQApplication();
		}
	};

}

TEST_F(TableCellContrastTest, QtFactoryContrastingTextColor) {
	EXPECT_FALSE(ot::QtFactory::getContrastingTextColor(QColor()).isValid());
	EXPECT_EQ(QColor(Qt::white), ot::QtFactory::getContrastingTextColor(QColor(0, 0, 0)));
	EXPECT_EQ(QColor(Qt::white), ot::QtFactory::getContrastingTextColor(QColor(68, 1, 84)));
	EXPECT_EQ(QColor(Qt::black), ot::QtFactory::getContrastingTextColor(QColor(255, 255, 255)));
	EXPECT_EQ(QColor(Qt::black), ot::QtFactory::getContrastingTextColor(QColor(253, 231, 37)));
}

TEST_F(TableCellContrastTest, SetupFromConfigSetsContrastingForeground) {
	ot::TableCfg cfg(2, 2);
	cfg.setCellText(0, 0, "0.0");
	cfg.setCellText(0, 1, "1.0");
	cfg.setCellText(1, 0, "not-a-number");
	cfg.setCellText(1, 1, "");
	cfg.addColoredRange(ot::ColoredTableRange(
		ot::TableRange(ot::TableRangeType::Table),
		ot::ColorMap(ot::ColorMapPreset::Viridis),
		0.0,
		1.0
	));

	ot::Table table(nullptr);
	table.setupFromConfig(cfg);

	ASSERT_NE(table.item(0, 0), nullptr);
	ASSERT_NE(table.item(0, 1), nullptr);
	ASSERT_NE(table.item(1, 0), nullptr);

	// Viridis at 0.0 is dark purple -> white text
	EXPECT_TRUE(table.item(0, 0)->data(Qt::BackgroundRole).isValid());
	EXPECT_EQ(QColor(Qt::white), table.item(0, 0)->foreground().color());

	// Viridis at 1.0 is bright yellow -> black text
	EXPECT_TRUE(table.item(0, 1)->data(Qt::BackgroundRole).isValid());
	EXPECT_EQ(QColor(Qt::black), table.item(0, 1)->foreground().color());

	// Non-numeric cell has no colored background -> neither role should be forced
	EXPECT_FALSE(table.item(1, 0)->data(Qt::BackgroundRole).isValid());
	EXPECT_FALSE(table.item(1, 0)->data(Qt::ForegroundRole).isValid());
}

TEST_F(TableCellContrastTest, CellEditUpdatesAndClearsForeground) {
	ot::TableCfg cfg(1, 1);
	cfg.setCellText(0, 0, "0.0");
	cfg.addColoredRange(ot::ColoredTableRange(
		ot::TableRange(ot::TableRangeType::Table),
		ot::ColorMap(ot::ColorMapPreset::Viridis),
		0.0,
		1.0
	));

	ot::Table table(nullptr);
	table.setupFromConfig(cfg);
	ASSERT_NE(table.item(0, 0), nullptr);
	EXPECT_EQ(QColor(Qt::white), table.item(0, 0)->foreground().color());

	// Editing cell from dark value (0.0) to bright value (1.0) switches text from white to black
	table.item(0, 0)->setText("1.0");
	EXPECT_EQ(QColor(Qt::black), table.item(0, 0)->foreground().color());

	// Editing cell to a non-numeric string clears both BackgroundRole and ForegroundRole
	table.item(0, 0)->setText("abc");
	EXPECT_FALSE(table.item(0, 0)->data(Qt::BackgroundRole).isValid());
	EXPECT_FALSE(table.item(0, 0)->data(Qt::ForegroundRole).isValid());
}

TEST_F(TableCellContrastTest, SelectedCellsBackgroundContrastAndTranslucentBlending) {
	ot::Table table(2, 1, nullptr);
	table.setItem(0, 0, new QTableWidgetItem("X"));

	// Light base palette + low-alpha black tint stays light -> black text
	QPalette lightPalette = table.palette();
	lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
	table.setPalette(lightPalette);

	table.item(0, 0)->setSelected(true);
	table.setSelectedCellsBackground(QColor(0, 0, 0, 64));
	EXPECT_EQ(QColor(Qt::black), table.item(0, 0)->foreground().color());

	// Dark base palette + low-alpha white tint stays dark -> white text
	QPalette darkPalette = table.palette();
	darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
	table.setPalette(darkPalette);

	table.setSelectedCellsBackground(QColor(255, 255, 255, 64));
	EXPECT_EQ(QColor(Qt::white), table.item(0, 0)->foreground().color());

	// Fully transparent background clears roles
	table.setSelectedCellsBackground(QColor(0, 0, 0, 0));
	EXPECT_FALSE(table.item(0, 0)->data(Qt::BackgroundRole).isValid());
	EXPECT_FALSE(table.item(0, 0)->data(Qt::ForegroundRole).isValid());
}

TEST_F(TableCellContrastTest, DelegatePaintsExplicitForegroundOverDarkBackground) {
	ot::TableCfg cfg(1, 1);
	cfg.setCellText(0, 0, "0.0");
	cfg.addColoredRange(ot::ColoredTableRange(
		ot::TableRange(ot::TableRangeType::Table),
		ot::ColorMap(ot::ColorMapPreset::Viridis),
		0.0,
		1.0
	));

	ot::Table table(nullptr);
	table.setupFromConfig(cfg);
	table.resize(120, 40);

	// Set a bright red palette text color so we verify the delegate overrides it with contrasting white text
	QPalette pal = table.palette();
	pal.setColor(QPalette::Text, QColor(255, 0, 0));
	table.setPalette(pal);

	const QModelIndex index = table.model()->index(0, 0);
	QStyleOptionViewItem option;
	option.rect = QRect(0, 0, 120, 40);
	option.palette = pal;
	option.displayAlignment = Qt::AlignCenter;
	option.text = "0.0";

	QImage image(120, 40, QImage::Format_ARGB32_Premultiplied);
	image.fill(Qt::transparent);
	QPainter painter(&image);
	table.itemDelegate()->paint(&painter, option, index);
	painter.end();

	// Ensure no red theme text pixel was drawn onto the dark Viridis cell background
	bool foundBrightRedPixel = false;
	bool foundBrightWhitePixel = false;
	for (int y = 0; y < image.height(); ++y) {
		for (int x = 0; x < image.width(); ++x) {
			const QColor px = image.pixelColor(x, y);
			if (px.red() > 200 && px.green() < 30 && px.blue() < 30) {
				foundBrightRedPixel = true;
			}
			if (px.red() > 220 && px.green() > 220 && px.blue() > 220) {
				foundBrightWhitePixel = true;
			}
		}
	}

	EXPECT_FALSE(foundBrightRedPixel);
	EXPECT_TRUE(foundBrightWhitePixel);
}

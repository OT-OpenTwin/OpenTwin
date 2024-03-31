/*
 *	File:		aTableWidget.h
 *	Package:	akWidgets
 *
 *  Created on: March 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>
#include <akWidgets/aWidget.h>

 // Qt header
#include <qtablewidget.h>				// base class
#include <qwidget.h>
#include <qstring.h>					// QString
#include <qcolor.h>						// QColor
#include <qheaderview.h>				// QHeaderView

// Forward declaration
class QFocusEvent;
class QKeyEvent;

namespace ak {

	//! @brief This class combines the functionallity of a QTableWidget and a ak::ui::core::aWidget
	class UICORE_API_EXPORT aTableWidget : public QTableWidget, public aWidget
	{
		Q_OBJECT
	public:

		//! @brief Constructor
		//! @param _parent The parent QWidget for this table
		aTableWidget(QWidget * _parent = (QWidget *) nullptr);

		//! @brief Constructor
		//! @param _rows The initial row count
		//! @param _columns The initial columns count
		//! @param _parent The parent QWidget for this table
		aTableWidget(int _rows, int _columns, QWidget * _parent = (QWidget *) nullptr);

		//! @brief Deconstructor
		virtual ~aTableWidget();

		// #######################################################################################################

		// Event handling

		//! @brief Emits a key pressend signal a key is pressed
		virtual void keyPressEvent(QKeyEvent * _event) override;

		//! @brief Emits a key released signal a key is released
		virtual void keyReleaseEvent(QKeyEvent * _event) override;

		virtual void focusInEvent(QFocusEvent * _event) override;
		virtual void focusOutEvent(QFocusEvent * _event) override;

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		// ##############################################################################################################

		// Table manipulation

		//! @brief Add a row to the table
		void addRow(void);

		//! @brief Add a column to the table
		void addColumn(void);

		//! @brief Set the displayed text of a cell
		//! @param _row The row of the cell
		//! @param _column The column of the cell
		//! @param _text The text to set
		//! @throw ak::Exception if any error occurs during a subroutine call
		void setCellText(
			int																_row,
			int																_column,
			const QString &													_text = QString("")
		);

		//! @brief Set weather the cell is editable by the user or not
		//! @param _row The row of the cell
		//! @param _column The column of the cell
		//! @param _editable If true, the specified cell will be editable by the user
		//! @throw ak::Exception if any error occurs during a subroutine call
		void setCellEditable(
			int																_row,
			int																_column,
			bool															_editable
		);

		//! @brief Set weather the cell is selectable by the user or not
		//! @param _row The row of the cell
		//! @param _column The column of the cell
		//! @param _selectable If true, the specified cell will be selectable by the user
		//! @throw ak::Exception if any error occurs during a subroutine call
		void setCellSelectable(
			int																_row,
			int																_column,
			bool															_selectable
		);

		//! @brief Set a cells fore color
		//! @param _row The row of the cell
		//! @param _column The column of the cell
		//! @param _color The color to set
		//! @throw ak::Exception if any error occurs during a subroutine call
		void setCellForecolor(
			int																_row,
			int																_column,
			const QColor &													_color
		);

		//! @brief Will set the header of the specified column
		//! @param _column The column index to change the text
		//! @param _text The text to set
		//! @throw ak::Exception If the provided index is out of range or any error occurs during a subroutine call
		void setColumnHeader(
			int																_column,
			const QString &													_text
		);

		//! @brief Set the specifeid columns width
		//! @param _column The column to resize
		//! @param _percent To how many percent of the total table width this item should be resized (1-99)
		//! @throw ak::Exception if the provided index is out of range, the provided value is invalid or any error occurs during a subroutine call
		void setColumnWidthInPercent(
			int																_column,
			int																_percent
		);

		//! @brief Will set the resize mode of the provided column
		//! @param _column The column index of the column to change the resize mode
		//! @param _mode The resize mode to set at the specified column
		//! @throw ak::Exception if the provided index is out of range or any error occurs during a subroutine call
		void setColumnResizeMode(
			int																_column,
			QHeaderView::ResizeMode											_mode
		);


		//! @brief Will set the visible mode for the row header
		//! @param _visible If true, the row headers will be visible
		void setRowHeaderIsVisible(
			bool															_visible
		);

		//! @brief Will set the enabled mode for the row header
		//! @param _enabled If true, the row headers will be enabled
		void setRowHeaderIsEnabled(
			bool															_enabled = true
		);

		//! @brief Will set the visible mode for the column header
		//! @param _visible If true, the column headers will be visible
		void setColumnHeaderIsVisible(
			bool															_visible
		);

		//! @brief Will set the column header enabled in this table
		//! @param _enabled If true, the column header will be enabled
		void setColumnHeaderIsEnabled(
			bool															_enabled = true
		);

		//! @brief Will set the focus on the specified cell
		//! @param _row The row of the cell
		//! @param _celumn The column of the cell
		//! @throw ak::Exception if any error occurs during a subroutine call
		void setCellIsSelected(
			int																_row,
			int																_column,
			bool															_selected = true
		);

		//! @brief clear all cell content and delete all rows and columns
		void clearRows(void);

		// ##############################################################################################################

		// Information

		//! @brief Returns true if the row header is visible
		bool isRowHeaderVisible(void) const;

		//! @brief Returns true if the column header is visible
		bool isColumnHeaderVisible(void) const;

		//! @brief Returns true if the cell provided is selected
		//! @param _row The row index of the cell requested
		//! @param _column The column index of the cell requested
		//! @throw ak::Exception if any error occurs during a subroutine call
		bool isCellSelected(
			int																_row,
			int																_column
		);

		//! @brief Returns the cells text
		//! @param _row The row index of the requested cell
		//! @param _column the column index of the requested cell
		//! @throw ak::Exception if the provided indizes are our of range or any error occurs during a subroutine call
		QString getCellText(
			int																_row,
			int																_column
		) const;

		//! @brief Returns true if the cell is editable
		//! @param _row The row index of the requested cell
		//! @param _column the column index of the requested cell
		//! @throw ak::Exception if the provided indizes are our of range or any error occurs during a subroutine call
		bool isCellEditable(
			int																_row,
			int																_column
		);

		//! @brief Returns true if the cell is selectable
		//! @param _row The row index of the requested cell
		//! @param _column the column index of the requested cell
		//! @throw ak::Exception if the provided indizes are our of range or any error occurs during a subroutine call
		bool isCellSelectable(
			int																_row,
			int																_column
		);

	Q_SIGNALS:
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);
		void focused(void);
		void focusLost(void);
	private:

		//! @brief will throw an exception if the row or column index is out of range
		//! @param _row The row index to check
		//! @param _column The column index to check
		//! @throw ak::Exception if the provided indizes are out of range
		void checkIndex(
			int																_row,
			int																_column
		) const;

		//! @brief Returns the table widget item of the cell provided
		//! @param _row The row of the reqested cell
		//! @param _column The column of the requested cell
		//! @throw ak::Exception if the provided indizes are out of range
		QTableWidgetItem * getCell(
			int																_row,
			int																_column
		) const;
	};
} // namespace ak

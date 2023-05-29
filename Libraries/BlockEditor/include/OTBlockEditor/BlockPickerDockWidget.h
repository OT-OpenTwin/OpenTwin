//! @file BlockPickerDockWidget.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPIExport.h"

// Qt header
#include <QtWidgets/qdockwidget.h>

// std header
#include <list>

namespace ot {

	class BlockPickerWidget;
	class BlockCategoryConfiguration;

	class BLOCK_EDITOR_API_EXPORT BlockPickerDockWidget : public QDockWidget {
		Q_OBJECT
	public:

		//! @brief Constructor
		//! @param _parentWidget The parent widget
		BlockPickerDockWidget(QWidget* _parentWidget = (QWidget*)nullptr);

		//! @brief Constructor
		//! @param _title The initial dock title
		//! @param _parentWidget The parent widget
		BlockPickerDockWidget(const QString& _title, QWidget* _parentWidget = (QWidget*)nullptr);

		//! @brief Constructor
		//! @param _customPickerWidget Provide own block picker widget instance (this dock widget takes ownership)
		//! @param _title The initial dock title
		//! @param _parentWidget The parent widget
		BlockPickerDockWidget(BlockPickerWidget * _customPickerWidget, const QString& _title, QWidget* _parentWidget = (QWidget*)nullptr);

		virtual ~BlockPickerDockWidget();

		virtual void resizeEvent(QResizeEvent* _event) override;

		void addTopLevelCategory(ot::BlockCategoryConfiguration* _topLevelCategory);
		void addTopLevelCategories(const std::list<ot::BlockCategoryConfiguration*>& _topLevelCategories);

		void clear(void);

		ot::BlockPickerWidget* pickerWidget(void) { return m_widget; };


	private:
		inline Qt::Orientation calcWidgetOrientation(void) const { return (this->width() > this->height() ? Qt::Horizontal : Qt::Vertical); };

		BlockPickerWidget* m_widget;
	};

}
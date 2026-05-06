// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"
#include "OTWidgets/Header/HeaderFilter.h"
#include "OTWidgets/Header/HeaderFilterState.h"

// Qt header
#include <QtCore/qsize.h>
#include <QtWidgets/qheaderview.h>

namespace ot {
	
	class HeaderFilter;

	class OT_WIDGETS_API_EXPORT HeaderBase : public QHeaderView
	{
		Q_OBJECT
		OT_DECL_NOCOPY(HeaderBase)
		OT_DECL_NOMOVE(HeaderBase)
		OT_DECL_NODEFAULT(HeaderBase)
	public:
		explicit HeaderBase(Orientation _orientation, QWidget* _parent);
		explicit HeaderBase(Qt::Orientation _orientation, QWidget* _parent);
		virtual ~HeaderBase() = default;
		
		virtual int sizeHintForColumn(int _column) const override;
		virtual int sizeHintForRow(int _row) const override;

		void setFilterButtonSize(const QSize& _size) { m_buttonSize = _size; };
		const QSize& getFilterButtonSize() const { return m_buttonSize; };

		void setFilterButtonPadding(const QSize& _padding) { m_buttonPadding = _padding; };
		const QSize& getFilterButtonPadding() const { return m_buttonPadding; };

		void setDefaultHeaderFlags();

		const HeaderFilterState& getFilterState() const { return m_filterState; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Events

	protected:
		virtual void paintSection(QPainter* _painter, const QRect& _rect, int _logicalIndex) const override;
		virtual QSize sectionSizeFromContents(int _logicalIndex) const override;
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseMoveEvent(QMouseEvent* _event) override;
		virtual void leaveEvent(QEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Filtering

		virtual HeaderFilter::Features getFilterFeatures(int _logicalIndex) const;
		virtual QString getFilterTitle(int _logicalIndex) const;
		virtual QStringList getFilterOptions(int _logicalIndex) const;
		
		virtual QRect filterIconRect(int _logicalIndex) const;

		//! @brief Displays the filter menu for the specified logical index if filtering is enabled for that index.
		//! Uses the virtual methods to setup a HeaderFilter instance.
		//! Sorting will be handled synchronously by calling the sortOrderChangeRequest slot.
		//! Filters will be handled on confirm by calling the menuActionTriggered method with the selected options.
		//! @param _logicalIndex The logical index to show the filter menu for.
		virtual void showFilterMenu(int _logicalIndex);

		//! @brief Is called when the user confirms the filter menu.
		//! @param _filter The filter menu that was confirmed.
		virtual void filterHasChanged(const HeaderFilterState& _filterState);

	Q_SIGNALS:
		void filterChanged(const HeaderFilterState& _filterState);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	protected Q_SLOTS:
		virtual void sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder);
		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private Q_SLOTS:
		void slotSectionClicked(int _logicalIndex);

	private:
		HeaderFilterState m_filterState;

		QPoint m_lastMousePressPos;
		QSize m_buttonSize;
		QSize m_buttonPadding;
	};
}
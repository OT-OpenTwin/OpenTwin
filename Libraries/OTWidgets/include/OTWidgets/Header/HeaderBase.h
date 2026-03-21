// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qsize.h>
#include <QtWidgets/qheaderview.h>

namespace ot {
	
	class OT_WIDGETS_API_EXPORT HeaderBase : public QHeaderView
	{
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

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Events

	protected:
		virtual void paintSection(QPainter* _painter, const QRect& _rect, int _logicalIndex) const override;
		virtual QSize sectionSizeFromContents(int _logicalIndex) const override;
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;
		virtual void mouseMoveEvent(QMouseEvent* _event) override;
		virtual void leaveEvent(QEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Filtering

		virtual bool canFilter(int _logicalIndex) const;
		virtual QRect filterIconRect(int _logicalIndex) const;
		virtual void showFilterMenu(int _logicalIndex);
		inline void setActiveFilterIndex(int _logicalIndex) { m_filterState.activeFilter = _logicalIndex; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	protected Q_SLOTS:
		virtual void sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		struct FilterState
		{
			FilterState() = default;

			int hoveredFilter = -1;
			int pressedFilter = -1;
			int activeFilter = -1;
		};
		FilterState m_filterState;

		QSize m_buttonSize;
		QSize m_buttonPadding;
	};
}
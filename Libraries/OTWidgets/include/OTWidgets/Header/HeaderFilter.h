// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qmenu.h>

// @otlicense
// File: ProjectOverviewFilter.h
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
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtWidgets/qmenu.h>

class QListWidget;
class QListWidgetItem;

namespace ot {

	class Label;
	class LineEdit;
	
	class OT_WIDGETS_API_EXPORT HeaderFilter : public QMenu
	{
		Q_OBJECT
		OT_DECL_NOCOPY(HeaderFilter)
		OT_DECL_NOMOVE(HeaderFilter)
		OT_DECL_NODEFAULT(HeaderFilter)
	public:
		enum class Feature : uint32_t
		{
			NoFeatuers       = 0 << 0,
			SortingEnabled   = 1 << 0,
			FilteringEnabled = 1 << 1
		};
		typedef Flags<Feature, uint32_t> Features;

		explicit HeaderFilter(int _logicalIndex, const Features& _features, QWidget* _parent);
		virtual ~HeaderFilter() = default;

		void setTitle(const QString& _title);

		void setOptions(const QStringList& _options);

		//! @brief Will uncheck all items and check only those that are present in the provided data.
		//! @param _data The filter data to set the state from.
		void updateCheckedState(const QStringList& _checkedItems);

		//! @brief Returns a list of all checked items.
		QStringList saveCheckedState() const;

		int getLogicalIndex() const { return m_logicalIndex; }
		bool wasConfirmed() const { return m_isConfirmed; };

		//! @brief Show the filter menu at the specified position.
		//! Will ensure the menu is properly positioned on screen.
		//! @param _pos Global position where the menu should be shown.
		void showAt(const QPoint& _pos);

		const Features& getFilterFeatures() const { return m_features; };

	Q_SIGNALS:
		void sortOrderChanged(int _logicalIndex, Qt::SortOrder _sortOrder);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotConfirm();
		void slotCancel();
		void slotTextChanged();
		void slotSortAscending();
		void slotSortDescending();
		void slotCheckedChanged(QListWidgetItem* _item);

	protected:
		bool isIncludedInTextFilter(QListWidgetItem* _item) const;

	private:
		int m_logicalIndex;
		bool m_isConfirmed;

		Features m_features;

		Label* m_title;
		LineEdit* m_filterEdit;
		QListWidget* m_optionsList;
	};

}
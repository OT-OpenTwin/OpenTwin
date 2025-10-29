// @otlicense

//! @file ModelLibraryDialogCfg.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/ModelLibraryDialogCfg.h"
#include "OTWidgets/Dialog.h"

// std header
#include <list>
#include <string>

#pragma warning (disable:4251)

class QWidget;
class QGroupBox;
class QGridLayout;

namespace ot {

	class Label;
	class LineEdit;
	class ComboBox;

	class OT_WIDGETS_API_EXPORT ModelLibraryDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NODEFAULT(ModelLibraryDialog)
		OT_DECL_NOCOPY(ModelLibraryDialog)
		OT_DECL_NOMOVE(ModelLibraryDialog)
	public:
		ModelLibraryDialog(ModelLibraryDialogCfg&& _config);
		virtual ~ModelLibraryDialog();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		const std::string& getSelectedName() const { return m_selectedName; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Slots

	private Q_SLOTS:
		void slotConfirm();
		void slotFilterChanged();
		void slotModelChanged();

	private:
		struct FilterInputEntry {
			std::string name;
			Label* label = nullptr;
			LineEdit* edit = nullptr;
		};

		ModelLibraryDialogCfg m_config;

		ComboBox* m_nameEdit;
		std::string m_selectedName;
		std::list<FilterInputEntry> m_filterEntries;

		QGroupBox* m_infoGroup;
		QGridLayout* m_infoLayout;
		std::list<QWidget*> m_infoWidgets;
	};

}

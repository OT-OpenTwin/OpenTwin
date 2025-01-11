//! @file ComboButton.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/PushButton.h"

class QMenu;
class QAction;

namespace ot {


	class OT_WIDGETS_API_EXPORT ComboButton : public PushButton {
		Q_OBJECT
		OT_DECL_NOCOPY(ComboButton)
	public:
		ComboButton(QWidget* _parent = (QWidget*)nullptr);
		ComboButton(const QString& _text, QWidget* _parent = (QWidget*)nullptr);
		ComboButton(const QString& _text, const QStringList& _items, QWidget* _parent = (QWidget*)nullptr);
		virtual ~ComboButton();

		virtual void mousePressEvent(QMouseEvent* _event) override;

		void setItems(const QStringList& _items);

	Q_SIGNALS:
		void textChanged(void);

	private Q_SLOTS:
		void slotActionTriggered(QAction* _action);

	private:
		void ini(void);

		QMenu* m_menu;
	};

}

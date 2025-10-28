// @otlicense

#pragma once

// OpenTwin header
#include "LoginData.h"
#include "OTCore/ProjectInformation.h"
#include "OTWidgets/Dialog.h"

namespace ot {
	class ComboBox;
	class TextEditor;
	class PushButton;
	class PlainTextEdit;
	class ImagePainterWidget;
}

class EditProjectInformationDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(EditProjectInformationDialog)
	OT_DECL_NOMOVE(EditProjectInformationDialog)
	OT_DECL_NODEFAULT(EditProjectInformationDialog)
public:
	EditProjectInformationDialog(const LoginData& _logInData, const ot::ProjectInformation& _project, QWidget* _parent);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected: Events

protected:

	virtual void showEvent(QShowEvent* _event) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotConfirm();
	void slotChanged();
	void slotUploadImage();
	void slotTakeScreenshot();
	void slotRemoveImage();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	void initializeData();
	
	const QSize c_toolButtonSize;
	bool m_firstShow;

	LoginData m_logInData;
	ot::ProjectInformation m_projectInformation;
	
	ot::ImagePainterWidget* m_imageWidget;
	ot::ComboBox* m_category;
	ot::PlainTextEdit* m_tags;

	ot::ComboBox* m_descriptionSyntax;
	ot::TextEditor* m_description;

	ot::PushButton* m_confirmButton;
};

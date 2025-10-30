// @otlicense

#pragma once

// OpenTwin header
#include "LoginData.h"
#include "OTGui/ExtendedProjectInformation.h"
#include "OTWidgets/Dialog.h"

namespace ot {
	class ComboBox;
	class TextEdit;
	class TextEditor;
	class ToolButton;
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
	EditProjectInformationDialog(const std::string& _callbackUrl, const std::string& _callbackAction, const LoginData& _logInData, const std::string& _projectName, QWidget* _parent);

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
	void slotTogglePreview();
	void slotDescriptionChanged();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	void initializeData();
	void updateInformationEntry();
	void updateDescriptionPreview();
	static QWidget* getWidgetForScreenshot();

	std::string m_callbackUrl;
	std::string m_callbackAction;

	const QSize c_toolButtonSize;
	bool m_firstShow;

	LoginData m_logInData;
	ot::ExtendedProjectInformation m_projectInformation;
	
	ot::ImagePainterWidget* m_imageWidget;
	ot::ComboBox* m_projectGroup;
	ot::PlainTextEdit* m_tags;

	ot::ToolButton* m_toggleShowPreviewButton;

	ot::ComboBox* m_descriptionSyntax;
	ot::TextEditor* m_description;
	ot::TextEdit* m_descriptionPreview;

	ot::PushButton* m_confirmButton;
};

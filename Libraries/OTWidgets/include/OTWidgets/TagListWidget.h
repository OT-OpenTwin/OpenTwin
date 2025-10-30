// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qframe.h>

// std header
#include <list>

namespace ot {

	class Label;
	class ComboBox;
	class ToolButton;
	class FlowLayout;

	class OT_WIDGETS_API_EXPORT TagListEntry : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(TagListEntry)
		OT_DECL_NOMOVE(TagListEntry)
	public:
		TagListEntry(QWidget* _parent);
		virtual ~TagListEntry();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void setText(const QString& _text);
		QString getText() const;

	Q_SIGNALS:
		void removeRequested(const QString& _tag);

	private Q_SLOTS:
		void slotRemove();

	private:
		Label* m_text;
		ToolButton* m_removeButton;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT TagListWidget : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOMOVE(TagListWidget)
		OT_DECL_NOCOPY(TagListWidget)
	public:
		TagListWidget(QWidget* _parent = nullptr);
		~TagListWidget();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void addTag(const QString& _tag);
		void setTags(const QStringList& _tags);
		QStringList getTags() const;
		void clearTags();
		
	Q_SIGNALS:
		void tagsChanged();

	public Q_SLOTS:
		void removeTag(const QString& _tag);

	private Q_SLOTS:
		void slotAddTagFromInput();

	private:
		FlowLayout* m_layout;
		ComboBox* m_input;
		std::list<TagListEntry*> m_tags;

	};

}
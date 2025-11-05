// @otlicense
// File: TagListWidget.h
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
		OT_DECL_NODEFAULT(TagListEntry)
	public:
		explicit TagListEntry(QWidget* _parent);
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
		OT_DECL_NODEFAULT(TagListWidget)
	public:
		explicit TagListWidget(QWidget* _parent);
		~TagListWidget();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void addTag(const QString& _tag);
		void setTags(const QStringList& _tags);
		QStringList getTags() const;
		void clear();
		void addOption(const QString& _option);
		void addOptions(const QStringList& _options);
		
		void setEditable(bool _editable);
		bool isEditable() const;

		void setPlaceholderText(const QString& _text);
		QString getPlaceholderText() const;

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
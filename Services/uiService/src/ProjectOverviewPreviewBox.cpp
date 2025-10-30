// @otlicense
// File: ProjectOverviewPreviewBox.cpp
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

// OpenTwin header
#include "ProjectOverviewPreviewBox.h"
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/TextEdit.h"
#include "OTWidgets/ImagePainterWidget.h"
#include "OTWidgets/PixmapImagePainter.h"
#include "OTWidgets/ImagePainterManager.h"

// Qt header
#include <QtCore/qpropertyanimation.h>
#include <QtWidgets/qlayout.h>

ot::ProjectOverviewPreviewBox::ProjectOverviewPreviewBox(QWidget* _parent)
	: QWidget(_parent), m_isExpanded(false)
{
	// Create widgets
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	m_imageWidget = new ot::ImagePainterWidget(this);
	m_imageWidget->setFixedSize(previewImageSize());
	m_imageWidget->setInteractive(false);
	mainLayout->addWidget(m_imageWidget, 0, Qt::AlignCenter);

	QGridLayout* infoLayout = new QGridLayout;
	infoLayout->setContentsMargins(0, 0, 0, 0);
	infoLayout->setColumnStretch(1, 1);
	mainLayout->addLayout(infoLayout);

	int r = 0;
	infoLayout->addWidget(new ot::Label("Name:", this), r, 0);
	m_name = new ot::Label(this);
	infoLayout->addWidget(m_name, r++, 1);

	infoLayout->addWidget(new ot::Label("Type:", this), r, 0);
	m_type = new ot::Label(this);
	infoLayout->addWidget(m_type, r++, 1);

	infoLayout->addWidget(new ot::Label("Group:", this), r, 0);
	m_projectGroup = new ot::Label(this);
	infoLayout->addWidget(m_projectGroup, r++, 1);

	infoLayout->addWidget(new ot::Label("Tags:", this), r, 0);
	m_tags = new ot::Label(this);
	m_tags->setWordWrap(true);
	infoLayout->addWidget(m_tags, r++, 1);

	infoLayout->addWidget(new ot::Label("Access:", this), r, 0);
	m_userGroups = new ot::Label(this);
	m_userGroups->setWordWrap(true);
	infoLayout->addWidget(m_userGroups, r++, 1);

	mainLayout->addWidget(new ot::Label("Description:", this));
	m_description = new ot::TextEdit(this);
	m_description->setReadOnly(true);
	mainLayout->addWidget(m_description, 1);

	// Create animation
	QPropertyAnimation* animation = new QPropertyAnimation(this, "maximumWidth");
	animation->setDuration(300);
	animation->setStartValue(0);
	animation->setEndValue(c_expandedWidth);
	animation->setEasingCurve(QEasingCurve::InOutCubic);
	m_animation.addAnimation(animation);

	animation = new QPropertyAnimation(this, "minimumWidth");
	animation->setDuration(300);
	animation->setStartValue(0);
	animation->setEndValue(c_expandedWidth);
	animation->setEasingCurve(QEasingCurve::InOutCubic);
	m_animation.addAnimation(animation);
	connect(&m_animation, &QParallelAnimationGroup::finished, this, &ProjectOverviewPreviewBox::slotAnimationFinished);

	m_collapseTimer.setSingleShot(true);
	m_collapseTimer.setInterval(500);
	connect(&m_collapseTimer, &QTimer::timeout, this, &ProjectOverviewPreviewBox::slotDelayedCollapse);

	// Start collapsed
	setMinimumWidth(0);
	setMaximumWidth(0);
}

ot::ProjectOverviewPreviewBox::~ProjectOverviewPreviewBox() {
	m_collapseTimer.stop();
	m_animation.stop();
}

void ot::ProjectOverviewPreviewBox::unsetProject() {
	setEnabled(false);
	m_animation.stop();
	m_collapseTimer.stop();
	m_collapseTimer.start();
}

void ot::ProjectOverviewPreviewBox::setProject(const ExtendedProjectInformation& _projectInfo) {
	m_collapseTimer.stop();

	// Set image if available
	if (!_projectInfo.getImageData().empty()) {
		ImagePainter* painter = ImagePainterManager::createFromRawData(_projectInfo.getImageData(), _projectInfo.getImageFormat());
		if (painter) {
			m_imageWidget->setPainter(painter);
			m_imageWidget->setHidden(false);
		}
		else {
			m_imageWidget->setPainter(new PixmapImagePainter(QPixmap()));
			m_imageWidget->setHidden(true);
		}
	}
	else {
		m_imageWidget->setHidden(true);
	}

	// Set description if available
	if (!_projectInfo.getDescription().empty()) {
		switch (_projectInfo.getDescriptionSyntax()) {
		case DocumentSyntax::PlainText:
			m_description->setPlainText(QString::fromStdString(_projectInfo.getDescription()));
			break;

		case DocumentSyntax::Markdown:
			m_description->setMarkdown(QString::fromStdString(_projectInfo.getDescription()));
			break;

		case DocumentSyntax::HTML:
			m_description->setHtml(QString::fromStdString(_projectInfo.getDescription()));
			break;

		default:
			OT_LOG_W("Unsupported document syntax (" + std::to_string(static_cast<int>(_projectInfo.getDescriptionSyntax())) + "). Defaulting to plain text...");
			m_description->setPlainText(QString::fromStdString(_projectInfo.getDescription()));
			break;
		}
	}
	else {
		m_description->setPlainText("");
	}

	// Set general info
	m_name->setText(QString::fromStdString(_projectInfo.getProjectName()));
	m_type->setText(QString::fromStdString(_projectInfo.getProjectType()));
	m_projectGroup->setText(QString::fromStdString(_projectInfo.getProjectGroup()));

	std::string tagsString;
	for (const std::string& tag : _projectInfo.getTags()) {
		if (!tagsString.empty()) tagsString += " ";
		tagsString += tag;
	}
	m_tags->setText(QString::fromStdString(tagsString));

	std::string userGroupsString;
	for (const std::string& group : _projectInfo.getUserGroups()) {
		if (!userGroupsString.empty()) userGroupsString += " ";
		userGroupsString += group;
	}
	m_userGroups->setText(QString::fromStdString(userGroupsString));

	// Expand
	bool showAnim = false;
	if (!m_isExpanded) {
		showAnim = true;
	}
	else if (m_animation.state() == QAbstractAnimation::Running) {
		showAnim = true;
	}

	if (showAnim) {
		m_animation.setDirection(QAbstractAnimation::Forward);
		m_animation.start();
	}
}

void ot::ProjectOverviewPreviewBox::slotDelayedCollapse() {
	m_animation.setDirection(QAbstractAnimation::Backward);
	m_animation.start();
}

void ot::ProjectOverviewPreviewBox::slotAnimationFinished() {
	if (m_animation.direction() == QAbstractAnimation::Forward) {
		m_isExpanded = true;
		setEnabled(true);
	}
	else {
		m_isExpanded = false;
	}
}

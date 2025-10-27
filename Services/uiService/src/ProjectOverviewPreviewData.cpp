#include "ProjectOverviewPreviewData.h"

ot::ProjectOverviewPreviewData::ProjectOverviewPreviewData()
	: m_imageFormat(ot::ImageFileFormat::PNG)
{}

ot::ProjectOverviewPreviewData::ProjectOverviewPreviewData(const ProjectInformation& _basicInfo) 
	: ProjectInformation(_basicInfo), m_imageFormat(ot::ImageFileFormat::PNG)
{}

/*****************************************************************//**
 * \file   FileToTableExtractorRegistrar.h
 * \brief  Class for registrating a new file type. 
 *         If a TableExtractor shall support a new file ending, all it needs is a static FileToTableExtractorRegistrar with the string of the new supported file extension.
 * 
 * \author Wagner
 * \date   April 2023
 *********************************************************************/
#pragma once
#include "FileToTableExtractorFactory.h"
#include <string>

template<class T>
class FileToTableExtractorRegistrar
{
public:
	FileToTableExtractorRegistrar(std::string fileExtension)
	{
		FileToTableExtractorFactory::GetInstance()->RegisterFactoryFunction(fileExtension, [](void)->TableExtractor * {return new T(); });
	}
};

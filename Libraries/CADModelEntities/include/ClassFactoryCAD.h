/*****************************************************************//**
 * \file   ClassFactoryCAD.h
 * \brief  Should NEVER EVER get any attributes. Diamond 
 * 
 * \author Wagner
 * \date   September 2023
 *********************************************************************/
#pragma once

#include "ClassFactoryHandlerAbstract.h"

class __declspec(dllexport) ClassFactoryCAD : public ClassFactoryHandlerAbstract
{
public:
	//ClassFactoryCAD() {};
	virtual ~ClassFactoryCAD() {};

	virtual EntityBase *CreateEntity(const std::string &entityType) override;
};


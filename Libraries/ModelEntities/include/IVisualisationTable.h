#pragma once
#include <string>
#include "OTGui/TableCfg.h"
#include "ContentChangedHandling.h"
#include "OTCore/GenericDataStructMatrix.h"

class __declspec(dllexport) IVisualisationTable
{
public:
	virtual ~IVisualisationTable() {}
	virtual const ot::GenericDataStructMatrix getTable() = 0;
	virtual void setTable(const ot::GenericDataStructMatrix& _table) = 0;
	virtual ot::TableCfg getTableConfig() = 0;
	virtual bool visualiseTable() = 0;
	virtual ot::ContentChangedHandling getTableContentChangedHandling() = 0;
	virtual ot::TableHeaderOrientation getHeaderOrientation() =  0;
};
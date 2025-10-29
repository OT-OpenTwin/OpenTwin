// @otlicense

#include "stdafx.h"
#include "QueuingDatabaseWritingRAII.h"
#include "DataBase.h"

QueuingDatabaseWritingRAII::QueuingDatabaseWritingRAII()
{
	DataBase::instance().setWritingQueueEnabled(true);
}

QueuingDatabaseWritingRAII::~QueuingDatabaseWritingRAII()
{
	DataBase::instance().setWritingQueueEnabled(false);
	DataBase::instance().flushWritingQueue();
}

#include "stdafx.h"
#include "QueuingDatabaseWritingRAII.h"
#include "DataBase.h"

QueuingDatabaseWritingRAII::QueuingDatabaseWritingRAII()
{
	DataBase::GetDataBase()->queueWriting(true);
}

QueuingDatabaseWritingRAII::~QueuingDatabaseWritingRAII()
{
	DataBase::GetDataBase()->queueWriting(false);
	DataBase::GetDataBase()->flushWritingQueue();
}

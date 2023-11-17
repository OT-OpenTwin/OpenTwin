#include "Connection.h"
//Constructor


Connection::Connection()
{
	
}

Connection::~Connection()
{

}


Connection::Connection(const GraphicsConnectionCfg& obj) :GraphicsConnectionCfg(obj)
{

}

void Connection::setNodeNumber(std::string num)
{
	this->nodeNumber = num;
}

std::string Connection::getNodeNumber()
{
	return this->nodeNumber;
}
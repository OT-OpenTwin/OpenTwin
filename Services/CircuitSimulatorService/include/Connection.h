#pragma once

//C++ Header
#include <string>

//Service Header

//Ot Header
#include "OTGui/GraphicsConnectionCfg.h"

class Connection : public ot::GraphicsConnectionCfg
{
public:
	Connection();
	virtual ~Connection();

	Connection(const GraphicsConnectionCfg& obj);

	


	void setNodeNumber(std::string num)
	{
		this->nodeNumber = num;
	}

	std::string getNodeNumber()
	{
		return this->nodeNumber;
	}

private:
	
	std::string nodeNumber;
	


};
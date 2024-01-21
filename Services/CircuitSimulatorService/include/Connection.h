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

	bool operator<(const Connection& other) const {
		if (this->originUid() == other.originUid()) {
			return this->destUid() < other.destUid();
		}
		else {
			return this->originUid() < other.originUid();
		}
	}


	void setNodeNumber(std::string num)
	{
		this->nodeNumber = num;
	}

	std::string getNodeNumber()
	{
		return this->nodeNumber;
	}

	void setID(std::string id)
	{
		this->id = id;
	}

	std::string getID()
	{
		return this->id;
	}
private:
	
	std::string nodeNumber;
	std::string id;
	


};
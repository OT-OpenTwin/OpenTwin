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

	/*bool operator<(const Connection& other) const {
		if (this->getOriginUid() == other.getOriginUid()) {
			return this->getDestinationUid() < other.getDestinationUid();
		}
		else {
			return this->getOriginUid() < other.getOriginUid();
		}
	}*/

	/*bool operator==(const Connection& other) const {
		return this->getOriginUid() != other.getOriginUid() && this->getDestinationUid() != other.getDestinationUid();
	}*/

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
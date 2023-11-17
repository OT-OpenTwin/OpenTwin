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

	//Setter
	void setNodeNumber(std::string);
	//Getter
	std::string getNodeNumber();
private:
	
	std::string nodeNumber;
	


};
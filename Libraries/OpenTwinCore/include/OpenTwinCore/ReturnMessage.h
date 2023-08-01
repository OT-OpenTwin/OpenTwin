#pragma once
#include "OpenTwinCore/CoreAPIExport.h"
#include "rJSON.h"
#include <string>

namespace ot
{
	class ReturnMessage
	{
	public:
		ReturnMessage(const std::string& status, const std::string& what)
			:_status(status), _what(what), _returnMessage("{\"Status\":\"" + status + "\",\"What\":\"" + what + "\"}") {}
		ReturnMessage(OT_rJSON_doc& doc)
		: _status(ot::rJSON::getString(doc,"Status")), _what(ot::rJSON::getString(doc,"What")), _returnMessage(ot::rJSON::toJSON(doc)) {}
	
		operator std::string () const { return _returnMessage; }
		std::string getWhat() const { return _what; }
		std::string getStatus() const { return _status; }

	private:
		const std::string _status;
		const std::string _what;
		const std::string _returnMessage;
	};
}

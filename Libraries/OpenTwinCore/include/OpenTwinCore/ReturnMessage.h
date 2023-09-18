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
			:_status(status), _what(what)
		{
			OT_rJSON_createDOC(doc);
			ot::rJSON::add(doc,"Status", status);
			ot::rJSON::add(doc,"What", what);
			_returnMessage = ot::rJSON::toJSON(doc);
		}
		ReturnMessage(OT_rJSON_doc& doc)
		: _status(ot::rJSON::getString(doc,"Status")), _what(ot::rJSON::getString(doc,"What")), _returnMessage(ot::rJSON::toJSON(doc)) {}
		ReturnMessage(const std::string& jsonStr)
			:_returnMessage(jsonStr)
		{
			auto doc = ot::rJSON::fromJSON(_returnMessage);
			_status = ot::rJSON::getString(doc, "Status");
			_what = ot::rJSON::getString(doc, "What");
		}

		operator std::string () const { return _returnMessage; }
		std::string getWhat() const { return _what; }
		std::string getStatus() const { return _status; }

	private:
		std::string _status;
		std::string _what;
		std::string _returnMessage;
	};

	struct ReturnStatus
	{
		static const std::string Ok() { return "OK"; };
		static const std::string Failed() { return "FAILED"; };
		static const std::string True() { return "TRUE"; };
		static const std::string False() { return "FALSE"; };
	};
}

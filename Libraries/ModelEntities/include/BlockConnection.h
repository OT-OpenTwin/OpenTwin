#pragma once
#include <string>
#include "SerializeWithBSON.h"

namespace ot
{
	class __declspec(dllexport) BlockConnection : public SerializeWithBSON
	{
		public:
			BlockConnection(const std::string& identifierFrom, const std::string& identifierTo);
			BlockConnection();
			std::string getIdentifierFrom() const;
			std::string getIdentifierTo() const;
			virtual bsoncxx::builder::basic::document SerializeBSON() const override;
			virtual void DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage) override;

		private:
			std::string _identifierFrom;
			std::string _identifierTo;

	};
}
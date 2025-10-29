// @otlicense

#pragma once

// RBE header
#include <rbeCore/AbstractConnection.h>

namespace rbeCore {

	class Step;

	class RBE_API_EXPORT HistoryConnection : public AbstractConnection {
	public:
		HistoryConnection(Step * _ref);

		virtual ~HistoryConnection();

		Step * reference(void) { return m_ref; }

		// ###############################################################################

		// Base class functions

		virtual std::string debugInformation(const std::string& _prefix) override;

		virtual void addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) override;

	private:
		Step *		m_ref;

		HistoryConnection() = delete;
		HistoryConnection(HistoryConnection&) = delete;
		HistoryConnection& operator = (HistoryConnection&) = delete;
	};

}
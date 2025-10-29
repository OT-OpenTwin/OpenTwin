// @otlicense

#pragma once

#include "OTCore/JSON.h"
#include "OTGui/PropertyGridCfg.h"

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include <map>
#include <list>

namespace ot { class Property; }

class UserSettings : public QObject {
	Q_OBJECT
	OT_DECL_NOCOPY(UserSettings)
public:
	static UserSettings& instance(void);

	void showDialog(void);

	void showDialog(const QString& _group);

	// #######################################################################################

	// Data management

	void clear(void);

	void addSettings(const std::string& _serviceName, const ot::PropertyGridCfg& _config);

	// #######################################################################################

private Q_SLOTS:
	void slotItemChanged(const std::string& _owner, const ot::Property* _property);

private:
	UserSettings();

};

// @otlicense

// OpenTwin header
#include "OTWidgets/Debugging/WidgetDebugHelper.h"

// std header
#include <sstream>

std::string ot::WidgetDebugHelper::objectHierarchyString(QObject* _object, const std::string& _linePrefix)
{
	std::list<QObject*> hierarchy;
	
	while (_object)
	{
		hierarchy.push_front(_object);
		_object = _object->parent();
	}
	
	int level = 0;
	std::stringstream ss;
	for (QObject* obj : hierarchy)
	{
		ss << _linePrefix;
		for (int i = 0; i < level; i++)
		{
			ss << "  ";
		}
		ss << obj->metaObject()->className() << " (" << obj->objectName().toStdString() << ")" << std::endl;
		level++;
	}

	return ss.str();
}

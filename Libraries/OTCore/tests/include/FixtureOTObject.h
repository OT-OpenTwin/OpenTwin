// @otlicense

#pragma once

#include <gtest/gtest.h>

#include "OTCore/Object/OTObject.h"

class FixtureOTObject : public OTObject
{
public:
	explicit TestOTObject(OTObject* _parentObject = nullptr) : OTObject(_parentObject) { ++g_createdCount; };
	~TestOTObject() override { ++s_destroyedCount; }
	static void resetCounters() { s_createdCount = 0; s_destroyedCount = 0; };
	static int getCreatedCount() { return s_createdCount; };
	static int getDestroyedCount() { return s_destroyedCount; };
private:
	static int s_createdCount;
	static int s_destroyedCount;
};

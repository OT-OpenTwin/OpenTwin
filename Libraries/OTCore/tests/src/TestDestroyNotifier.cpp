// @otlicense
// File: TestDestroyNotifier.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "gtest/gtest.h"
#include "OTCore/Object/ObjectDestroyNotifier.h"
#include "OTCore/Object/ObjectWithDestroyNotifier.h"

namespace intern
{

	class TestDestroyNotifier : public ot::ObjectDestroyNotifier
	{
	public:
		TestDestroyNotifier(int* _value) : m_value(_value) {}

	protected:
		void onObjectPreDestroy(ot::ObjectWithDestroyNotifier* _object) override
		{
			(*m_value)--;
		}

		void onObjectDestroyed(const ot::ObjectWithDestroyNotifier* _object) override
		{
			(*m_value)++;
		}

	private:
		int* m_value;
	};

}


TEST(DestroyNotifierTests, CoreFunctionalityTest)
{
	int myValue = 0;
	{
		intern::TestDestroyNotifier notifier(&myValue);
		ot::ObjectWithDestroyNotifier object;
		object.addDestroyNotifier(&notifier);
		EXPECT_EQ(myValue, 0);

		{
			intern::TestDestroyNotifier notifier2(&myValue);
			object.addDestroyNotifier(&notifier2);
			ot::ObjectWithDestroyNotifier object2;
			object2.addDestroyNotifier(&notifier2);
			EXPECT_EQ(myValue, 0);
			{
				intern::TestDestroyNotifier notifier3(&myValue);
				object.addDestroyNotifier(&notifier3);
				object2.addDestroyNotifier(&notifier3);
				ot::ObjectWithDestroyNotifier object3;
				object3.addDestroyNotifier(&notifier3);
				EXPECT_EQ(myValue, 0);
			}
			EXPECT_EQ(myValue, 1);

			object2.notifyPreDestroy();
			EXPECT_EQ(myValue, 0);

		}
		EXPECT_EQ(myValue, 1);

		object.notifyPreDestroy();
		EXPECT_EQ(myValue, 0);

	}
	EXPECT_EQ(myValue, 1);
}
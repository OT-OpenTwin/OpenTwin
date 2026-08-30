// @otlicense

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
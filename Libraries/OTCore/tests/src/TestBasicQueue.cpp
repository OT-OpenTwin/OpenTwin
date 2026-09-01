// @otlicense

#include "gtest/gtest.h"
#include "OTCore/BasicQueue/BasicQueue.h"
#include "OTCore/BasicQueue/BasicQueueObject.h"
#include "OTCore/BasicQueue/BasicQueueFunctionObject.h"
#include <array>
#include <algorithm>

TEST(BasicQueueTests, CoreFunctionalityTest)
{
	using namespace ot;
	BasicQueue queue;

	int myValue = 0;

	queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 2); return 0; }));
	queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 1); return 0; }, BasicQueueObject::InsertFront));
	queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 3); return 0; }));

	queue.exec();

	EXPECT_EQ(myValue, 3);
	EXPECT_FALSE(queue.hasNext());
}

TEST(BasicQueueTests, ObjectInsertOnExec)
{
	using namespace ot;
	BasicQueue queue;

	int myValue = 0;

	queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 1); return 0; }));
	queue.push(new BasicQueueFunctionObject([&]() { 
		queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 2); return 0; }, BasicQueueObject::InsertFront));
		queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 4); return 0; }));
		return 0; })
	);
	queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 3); return 0; }));

	queue.exec();

	EXPECT_EQ(myValue, 4);
	EXPECT_FALSE(queue.hasNext());
}

TEST(BasicQueueTests, UniqueCollisionOnExec)
{
	using namespace ot;

	/*
	 * Test unique-object collisions while the queue is executing.
	 *
	 * Initial queue:
	 *
	 *     A
	 *     X
	 *     B
	 *
	 * A creates a new object with the same unique key as B.
	 *
	 * All combinations of:
	 *
	 *     - B position: directly behind A / behind a marker
	 *     - Unique behavior:
	 *         KeepExisting
	 *         ReplaceExisting
	 *         RemoveExisting
	 *     - Insert order of the replacement object:
	 *         InsertFront
	 *         InsertBack
	 *
	 * are tested.
	 *
	 * B additionally creates C during execution. This verifies that
	 * the unique-object bookkeeping is still valid after the collision.
	 */

	const std::array<BasicQueueObject::UniqueInsertBahavior, 3> behaviors =
	{
		BasicQueueObject::KeepExisting,
		BasicQueueObject::ReplaceExisting,
		BasicQueueObject::RemoveExisting
	};

	const std::array<BasicQueueObject::InsertOrder, 2> insertOrders =
	{
		BasicQueueObject::InsertOrder::InsertBack,
		BasicQueueObject::InsertOrder::InsertFront
	};

	const std::array<bool, 2> bPositions =
	{
		false,
		true
	};

	for (const auto behavior : behaviors)
	{
		for (const auto insertOrder : insertOrders)
		{
			for (const bool putMarkerBeforeB : bPositions)
			{
				BasicQueue queue;

				std::vector<std::string> executionTrace;

				/*
				 * B creates C when it executes.
				 *
				 * C has a different unique key, so this additionally
				 * verifies that the unique map remains usable after
				 * the collision operation.
				 */
				auto createC = [&]()
					{
						return (new BasicQueueFunctionObject(
							[&]()
							{
								executionTrace.push_back("C");
								return 0;
							}
						))->makeQueueUnique(
							"C",
							BasicQueueObject::KeepExisting
						);
					};

				/*
				 * Existing B.
				 *
				 * This is the object against which A will perform the
				 * unique collision.
				 */
				auto createExistingB = [&]()
					{
						return (new BasicQueueFunctionObject(
							[&]()
							{
								executionTrace.push_back("B");

								/*
								 * B creates another unique object while
								 * executing.
								 */
								queue.push(createC());

								return 0;
							}
						))->makeQueueUnique(
							"B",
							BasicQueueObject::KeepExisting
						);
					};

				/*
				 * New B created by A.
				 *
				 * Its behavior and insertion order are varied.
				 */
				auto createNewB = [&]()
					{
						return (new BasicQueueFunctionObject(
							[&]()
							{
								executionTrace.push_back("B_new");
								return 0;
							},
							insertOrder
						))->makeQueueUnique(
							"B",
							behavior
						);
					};

				/*
				 * A creates the colliding B while executing.
				 */
				auto createA = [&]()
					{
						return (new BasicQueueFunctionObject(
							[&]()
							{
								executionTrace.push_back("A");

								queue.push(createNewB());

								return 0;
							}
						))->makeQueueUnique(
							"A",
							BasicQueueObject::KeepExisting
						);
					};

				/*
				 * Construct the initial queue.
				 *
				 * Either:
				 *
				 *     A B X
				 *
				 * or:
				 *
				 *     A X B
				 *
				 * This makes the position of the existing unique object
				 * relevant for ReplaceExisting and RemoveExisting.
				 */
				queue.push(createA());

				if (putMarkerBeforeB)
				{
					queue.push(new BasicQueueFunctionObject(
						[&]()
						{
							executionTrace.push_back("X");
							return 0;
						}
					));

					queue.push(createExistingB());
				}
				else
				{
					queue.push(createExistingB());

					queue.push(new BasicQueueFunctionObject(
						[&]()
						{
							executionTrace.push_back("X");
							return 0;
						}
					));
				}

				queue.exec();

				/*
				 * Determine the expected result.
				 *
				 * A has executed and has now inserted B_new.
				 *
				 * KeepExisting:
				 * ----------------
				 * Existing B remains in its original position.
				 * B_new is deleted and never executes.
				 *
				 * ReplaceExisting:
				 * ----------------
				 * Existing B is replaced at exactly the same list
				 * position. B_new therefore executes at B's position.
				 *
				 * RemoveExisting:
				 * ----------------
				 * Existing B is removed completely and B_new is
				 * inserted according to B_new's insertion order.
				 *
				 * B_new itself does not create C. Only the original B
				 * does, which makes it possible to distinguish the
				 * three behaviors.
				 */

				std::vector<std::string> expectedTrace;
				expectedTrace.push_back("A");

				if (behavior == BasicQueueObject::KeepExisting)
				{
					/*
					 * The original B remains.
					 *
					 * Its position is unaffected by the rejected B_new.
					 */
					if (putMarkerBeforeB)
					{
						expectedTrace.push_back("X");
						expectedTrace.push_back("B");
					}
					else
					{
						expectedTrace.push_back("B");
						expectedTrace.push_back("X");
					}

					/*
					 * Original B creates C at the back.
					 */
					expectedTrace.push_back("C");
				}
				else if (behavior == BasicQueueObject::ReplaceExisting)
				{
					/*
					 * B is replaced in-place by B_new.
					 *
					 * Therefore B itself never executes and C is
					 * never created.
					 */
					if (putMarkerBeforeB)
					{
						expectedTrace.push_back("X");
						expectedTrace.push_back("B_new");
					}
					else
					{
						expectedTrace.push_back("B_new");
						expectedTrace.push_back("X");
					}
				}
				else if (behavior == BasicQueueObject::RemoveExisting)
				{
					/*
					 * B is removed first.
					 *
					 * The remaining queue contains only X.
					 *
					 * B_new is then inserted according to its own
					 * insertion order.
					 */
					if (insertOrder == BasicQueueObject::InsertOrder::InsertFront)
					{
						expectedTrace.push_back("B_new");

						/*
						 * X remains in the queue.
						 */
						expectedTrace.push_back("X");
					}
					else
					{
						expectedTrace.push_back("X");
						expectedTrace.push_back("B_new");
					}
				}

				ASSERT_EQ(executionTrace.size(), expectedTrace.size())
					<< "Unexpected trace size. "
					<< "Behavior=" << static_cast<int>(behavior)
					<< ", InsertOrder="
					<< static_cast<int>(insertOrder)
					<< ", B position="
					<< (putMarkerBeforeB ? "A-X-B" : "A-B-X");

				EXPECT_EQ(executionTrace, expectedTrace)
					<< "Behavior=" << static_cast<int>(behavior)
					<< ", InsertOrder="
					<< static_cast<int>(insertOrder)
					<< ", B position="
					<< (putMarkerBeforeB ? "A-X-B" : "A-B-X");

				EXPECT_FALSE(queue.hasNext());
			}
		}
	}
}

TEST(BasicQueueTests, UniqueObjects)
{
	using namespace ot;
	BasicQueue queue;

	int myValue = 0;

	queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 2); return 0; }));
	queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, -1); return 0; }))->makeQueueUnique("A"));
	queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, -1); return 0; }))->makeQueueUnique("B"));
	queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 3); return 0; }))->makeQueueUnique("C"));
	queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, -1); return 0; }))->makeQueueUnique("D"));
	queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 5); return 0; }));

	queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, -1); return 0; }))->makeQueueUnique("C", BasicQueueObject::KeepExisting));
	queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 6); return 0; }))->makeQueueUnique("B", BasicQueueObject::RemoveExisting));
	queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 4); return 0; }))->makeQueueUnique("D", BasicQueueObject::ReplaceExisting));
	queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 1); return 0; }, BasicQueueObject::InsertFront))->makeQueueUnique("A", BasicQueueObject::RemoveExisting));
	queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, 7); return 0; }));

	queue.exec();

	EXPECT_EQ(myValue, 7);
	EXPECT_FALSE(queue.hasNext());
}

TEST(BasicQueueTests, MutiqueueUniqueObjects)
{
	using namespace ot;
	BasicQueue queue;

	int myValue = 0;

	for (int i = 0; i < 10; i++) {
		myValue = i;

		queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, i + 2); return 0; }));
		queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, -1); return 0; }))->makeQueueUnique("A"));
		queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, -1); return 0; }))->makeQueueUnique("B"));
		queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, i + 3); return 0; }))->makeQueueUnique("C"));
		queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, -1); return 0; }))->makeQueueUnique("D"));
		queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, i + 5); return 0; }));
		queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, -1); return 0; }))->makeQueueUnique("C", BasicQueueObject::KeepExisting));
		queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, i + 6); return 0; }))->makeQueueUnique("B", BasicQueueObject::RemoveExisting));
		queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, i + 4); return 0; }))->makeQueueUnique("D", BasicQueueObject::ReplaceExisting));
		queue.push((new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, i + 1); return 0; }, BasicQueueObject::InsertFront))->makeQueueUnique("A", BasicQueueObject::RemoveExisting));
		queue.push(new BasicQueueFunctionObject([&]() { myValue++; EXPECT_EQ(myValue, i + 7); return 0; }));

		queue.exec();
		EXPECT_EQ(myValue, i + 7);
		EXPECT_FALSE(queue.hasNext());
	}
}
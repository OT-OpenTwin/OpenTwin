// @otlicense

#include "gtest/gtest.h"
#include "OTCore/BasicQueue/BasicQueue.h"
#include "OTCore/BasicQueue/BasicQueueObject.h"
#include "OTCore/BasicQueue/BasicQueueFunctionObject.h"

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
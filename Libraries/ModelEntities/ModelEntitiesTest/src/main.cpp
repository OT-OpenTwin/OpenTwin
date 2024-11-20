#include <gtest/gtest.h>

int main(int argc, char** argv) {
	char* allDLLPaths = std::getenv("OT_ALL_DLLD");
	char* Path = std::getenv("PATH");

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
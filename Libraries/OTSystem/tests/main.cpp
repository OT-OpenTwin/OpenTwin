#include <gtest/gtest.h>

// ------------------------------------------------------------
// Entry point for the GoogleTest-based test binary.
// ------------------------------------------------------------
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
